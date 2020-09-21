#include <KtUtils/AsyncInvoker>
#include <atomic>

using namespace std::chrono;

namespace KtUtils {
/* ======== AsyncInvokeEvent ======== */
struct AsyncInvokeEventData {
  std::function<QVariant(void)> Function;
  int delay_ms;
  std::function<bool(void)> IsAlive;
  std::promise<QVariant> promise;
  std::shared_future<QVariant> future;
  std::atomic_bool executed = ATOMIC_VAR_INIT(false);
};

class AsyncInvokeEvent : public QEvent {
 public:
  AsyncInvokeEvent(std::function<QVariant(void)>&& function, int delay_ms,
                   std::function<bool(void)>&& IsAlive);
  AsyncInvokeEvent(AsyncInvokeEvent&&) = default;
  ~AsyncInvokeEvent() override;

  void Invoke();

  static const int kEventType;

  QSharedPointer<AsyncInvokeEventData> d_;
};

const int AsyncInvokeEvent::kEventType = QEvent::registerEventType();

AsyncInvokeEvent::AsyncInvokeEvent(std::function<QVariant(void)>&& function,
                                   int delay_ms,
                                   std::function<bool(void)>&& isAlive)
    : QEvent(QEvent::Type(kEventType)), d_(new AsyncInvokeEventData) {
  d_->Function = function;
  d_->delay_ms = delay_ms;
  d_->IsAlive = isAlive;
  d_->future = d_->promise.get_future();
}

AsyncInvokeEvent::~AsyncInvokeEvent() {}

void AsyncInvokeEvent::Invoke() {
  QVariant ret;
  if (!d_->IsAlive || d_->IsAlive()) {
    ret = d_->Function();
  }
  d_->promise.set_value(ret);
  d_->executed.store(true);
}
/* ======== AsyncInvokeEvent ======== */

/* ======== AsyncInvokeEvent ======== */
class AsyncInvokerEventFilter : public QObject {
 public:
  explicit AsyncInvokerEventFilter(QThread* thread);
  ~AsyncInvokerEventFilter();

 protected:
  bool event(QEvent* event) override;
  void timerEvent(QTimerEvent* event) override;

 private:
  QHash<int, AsyncInvokeEvent*> events_;
};

AsyncInvokerEventFilter::AsyncInvokerEventFilter(QThread* thread) {
  moveToThread(thread);
}

AsyncInvokerEventFilter::~AsyncInvokerEventFilter() { qDeleteAll(events_); }

bool AsyncInvokerEventFilter::event(QEvent* event) {
  bool ret = QObject::event(event);
  if (event->type() == AsyncInvokeEvent::kEventType) {
    AsyncInvokeEvent* e = static_cast<AsyncInvokeEvent*>(event);
    int id = startTimer(e->d_->delay_ms);
    events_[id] = new AsyncInvokeEvent(std::move(*e));
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
/* ======== AsyncInvokeEvent ======== */

/* ======== AsyncInvoker ======== */
AsyncInvoker::Future AsyncInvoker::Invoke(
    std::function<QVariant(void)> function, QThread* thread, int delay_ms,
    std::function<bool(void)> isAlive) {
  if (!thread) {
    thread = qApp->thread();
  }

  AsyncInvokerEventFilter* filter;
  {
    static std::atomic_flag flag = ATOMIC_FLAG_INIT;
    static QHash<QThread*, AsyncInvokerEventFilter*> filters;
    while (flag.test_and_set(std::memory_order_seq_cst)) {
    }
    auto it = filters.find(thread);
    if (it == filters.end()) {
      it = filters.insert(thread, new AsyncInvokerEventFilter{thread});
    }
    filter = *it;
    flag.clear(std::memory_order_release);
  }

  auto event =
      new AsyncInvokeEvent{std::move(function), delay_ms, std::move(isAlive)};
  QCoreApplication::postEvent(filter, event);
  return Future{event->d_};
}
/* ======== AsyncInvoker ======== */

/* ======== AsyncInvoker::Future ======== */
AsyncInvoker::Future::Future(QSharedPointer<AsyncInvokeEventData> data)
    : d_(data) {}

AsyncInvoker::Future::~Future() {}

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
/* ======== AsyncInvoker::Future ======== */
}  // namespace KtUtils
