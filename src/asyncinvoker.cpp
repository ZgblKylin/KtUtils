#include <KtUtils/AsyncInvoker>
#include <atomic>

using namespace std::chrono;

namespace KtUtils {
/* ================ AsyncInvokeData ================ */
struct AsyncInvokeData {
  std::function<QVariant(void)> Function;
  int delay_ms = 0;
  std::function<bool(void)> IsAlive;
  std::promise<QVariant> promise;
  std::shared_future<QVariant> future;
  std::atomic_bool executed = ATOMIC_VAR_INIT(false);

  void Invoke();
};

void AsyncInvokeData::Invoke() {
  QVariant ret;
  if (!IsAlive || IsAlive()) {
    ret = Function();
  }
  promise.set_value(ret);
  executed.store(true);
}
/* ================ AsyncInvokeData ================ */

/* ================ AsyncInvokeEvent ================ */
class AsyncInvokeEvent : public QEvent {
 public:
  AsyncInvokeEvent();
  ~AsyncInvokeEvent() override = default;

  static const int kEventType;
  QSharedPointer<AsyncInvokeData> d;
};

const int AsyncInvokeEvent::kEventType = QEvent::registerEventType();

AsyncInvokeEvent::AsyncInvokeEvent()
    : QEvent(QEvent::Type(kEventType)), d(new AsyncInvokeData) {}
/* ================ AsyncInvokeEvent ================ */

/* ================ AsyncInvokeEvent ================ */
class AsyncInvokerEventFilter : public QObject {
 public:
  explicit AsyncInvokerEventFilter(QThread* thread);

 protected:
  bool event(QEvent* event) override;
  void timerEvent(QTimerEvent* event) override;

 private:
  QHash<int, QSharedPointer<AsyncInvokeData>> events_;
};

AsyncInvokerEventFilter::AsyncInvokerEventFilter(QThread* thread) {
  moveToThread(thread);
}

bool AsyncInvokerEventFilter::event(QEvent* event) {
  bool ret = QObject::event(event);
  if (event->type() == AsyncInvokeEvent::kEventType) {
    AsyncInvokeEvent* e = static_cast<AsyncInvokeEvent*>(event);
    if (e->d->delay_ms > 0) {
      // Deferred event, invoke in timerEvent
      int id = startTimer(e->d->delay_ms);
      events_[id] = e->d;
    } else {
      e->d->Invoke();
    }
  }
  event->accept();
  return ret;
}

void AsyncInvokerEventFilter::timerEvent(QTimerEvent* event) {
  int id = event->timerId();
  killTimer(id);

  auto it = events_.find(id);
  if (it == events_.end()) {
    return;
  }
  it.value()->Invoke();
  events_.erase(it);
}
/* ================ AsyncInvokeEvent ================ */

/* ================ AsyncInvoker ================ */
AsyncInvoker::Future AsyncInvoker::Invoke(
    const std::function<QVariant(void)>& function, QThread* thread,
    int delay_ms, const std::function<bool(void)>& isAlive) {
  if (!thread) {
    thread = qApp->thread();
  }

  AsyncInvokerEventFilter* filter;
  {
    // Find event filter for given thread
    static std::atomic_flag flag = ATOMIC_FLAG_INIT;
    static QHash<QThread*, AsyncInvokerEventFilter*> filters;
    while (flag.test_and_set(std::memory_order_seq_cst)) { // Spin-lock
    }
    auto it = filters.find(thread);
    if (it == filters.end()) {
      it = filters.insert(thread, new AsyncInvokerEventFilter{thread});
    }
    filter = *it;
    flag.clear(std::memory_order_release);
  }

  auto event = new AsyncInvokeEvent;
  event->d->Function = function;
  event->d->delay_ms = delay_ms;
  event->d->IsAlive = isAlive;
  event->d->future = event->d->promise.get_future();
  QCoreApplication::postEvent(filter, event);
  return Future{event->d};
}
/* ================ AsyncInvoker ================ */

/* ================ AsyncInvoker::Future ================ */
AsyncInvoker::Future::Future(const QSharedPointer<AsyncInvokeData>& data)
    : d_(data) {}

AsyncInvoker::Future::~Future() {
  // Forward-declared AsyncInvokeData to be destructed by shared pointer here
}

std::shared_future<QVariant> AsyncInvoker::Future::future() const {
  return d_->future;
}

bool AsyncInvoker::Future::valid() const {
  return d_->future.valid() && d_->executed.load();
}

std::future_status AsyncInvoker::Future::status() const {
  if (!d_->future.valid()) {
    return std::future_status::deferred;
  } else if (!d_->executed.load()) {
    return std::future_status::timeout;
  } else {
    return std::future_status::ready;
  }
}

const QVariant& AsyncInvoker::Future::get(
    QEventLoop::ProcessEventsFlags flags) const {
  wait(flags);
  return d_->future.get();
}

void AsyncInvoker::Future::wait(QEventLoop::ProcessEventsFlags flags) const {
  Wait([this] { return valid(); }, flags);
}
/* ================ AsyncInvoker::Future ================ */
}  // namespace KtUtils
