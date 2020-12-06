#include <KtUtils/future.h>

namespace KtUtils {
namespace impl {
struct FutureBase::Private : public std::enable_shared_from_this<FutureBase> {
  FutureBase* q = nullptr;
  std::shared_ptr<Private> previous;

  // Status
  enum Status { Unstarted = 0, Running = 1, Finished = 2 };
  std::atomic<Status> status = ATOMIC_VAR_INIT(Unstarted);
  std::shared_ptr<std::atomic_bool> canceled = nullptr;
  std::shared_ptr<std::atomic_bool> suspended = nullptr;

  // future
  std::function<void(void)> function{};
  std::function<void(void)> finally{};

  // slot
  QObject* receiver = nullptr;
  Qt::ConnectionType connectionType = Qt::AutoConnection;

  // thread pool
  QThreadPool* threadPool = nullptr;

  // success on
  std::list<std::function<bool(void)>> success_on{};
  std::weak_ptr<Private> then;

  // fail on
  std::list<std::function<bool(void)>> fail_on{};
  std::function<void(void)> on_failed{};

  Private(FutureBase* prev, std::function<void(void)>&& func,
          FutureBase* parent);
  Private(const Private&) = delete;
  Private& operator=(const Private&) = delete;

  void Invoke() noexcept;
  bool CheckSuccessOn();
  bool CheckFailOn();
  void OnFailed() noexcept;
  void Finally() noexcept;
};

FutureBase::Private::Private(FutureBase* prev, std::function<void(void)>&& func,
                             FutureBase* parent)
    : q(parent), previous(prev->d) {
  if (previous) {
    previous->then = shared_from_this();
    canceled = previous->canceled;
    suspended = previous->suspended;
  } else {
    canceled = std::make_shared<std::atomic_bool>(false);
    suspended = std::make_shared<std::atomic_bool>(false);
  }

  function = [this, func] {
    if (!func) {
      return Finally();
    }
    try {
      func();
      if (success_on.empty() && fail_on.empty()) {
        return Finally();
      }
      while (true) {
        if (CheckSuccessOn() || CheckFailOn()) {
          return Finally();
        }
      }
    } catch (...) {
      OnFailed();
    }
    Finally();
  };
}

void FutureBase::Private::Invoke() noexcept {
  Status expexted = Unstarted;
  if (!status.compare_exchange_strong(expexted, Running)) {
    return;
  }
  if (canceled->load(std::memory_order_acquire)) {
    function = [] {};
  }
  if (!function) {
    return;
  } else if (receiver) {
    QMetaObject::invokeMethod(receiver, function, connectionType);
  } else if (threadPool) {
    QtConcurrent::run(threadPool, function);
  } else {
    function();
  }
}

bool FutureBase::Private::CheckSuccessOn() {
  for (auto&& success : success_on) {
    if (suspended->load(std::memory_order_acquire)) {
      return false;
    }
    if (canceled->load(std::memory_order_acquire) || success()) {
      return true;
    }
  }
  return false;
}

bool FutureBase::Private::CheckFailOn() {
  for (auto&& fail : fail_on) {
    if (suspended->load(std::memory_order_acquire)) {
      return false;
    }
    if (canceled->load(std::memory_order_acquire) || fail()) {
      OnFailed();
      return true;
    }
  }
  return false;
}

void FutureBase::Private::OnFailed() noexcept {
  if (on_failed) {
    try {
      on_failed();
    } catch (...) {
    }
  }
}

void FutureBase::Private::Finally() noexcept {
  try {
    finally();
  } catch (...) {
  }
  status.store(Finished, std::memory_order_release);

  // Then
  auto ptr = then.lock();
  if (ptr) {
    if (!ptr->receiver && !ptr->threadPool) {
      if (receiver) {
        ptr->q->ExecuteOn(receiver, connectionType);
      } else if (threadPool) {
        ptr->q->ExecuteOn(threadPool);
      }
    }
    ptr->q->Run();
  }
}

FutureBase::FutureBase() : FutureBase(nullptr, std::function<void(void)>{}) {}

FutureBase::FutureBase(FutureBase* prev, std::function<void(void)>&& func)
    : d(std::make_shared<Private>(
          prev, std::forward<std::function<void(void)>>(func), this)) {}

void FutureBase::SuccessOn(const std::function<bool()>& func) {
  d->success_on.emplace_back([func]() -> bool { return func(); });
}

void FutureBase::ExecuteOn(nullptr_t) {
  d->receiver = nullptr;
  d->connectionType = Qt::AutoConnection;
  d->threadPool = nullptr;
}

void FutureBase::ExecuteOn(QObject* receiver, Qt::ConnectionType type) {
  ExecuteOn(nullptr);
  d->receiver = receiver;
  d->connectionType = type;
}

void FutureBase::ExecuteOn(QThreadPool* pool) {
  ExecuteOn(nullptr);
  d->threadPool = pool;
}

void FutureBase::FailOn(const std::function<bool()>& func) {
  d->fail_on.emplace_back([func]() -> bool { return func(); });
}

void FutureBase::OnFailed(const std::function<void(void)>& func) {
  d->on_failed = func;
}

void FutureBase::Finally(const std::function<void(void)>& func) {
  d->finally = func;
}

bool FutureBase::IsRunning() const {
  return d->status.load(std::memory_order_acquire) == Private::Running;
}

void FutureBase::Run() {
  if (d->previous) {
    d->previous->q->Run();
  } else {
    d->Invoke();
  }
}

bool FutureBase::IsFinished() const {
  return d->status.load(std::memory_order_acquire) == Private::Finished;
}

void FutureBase::Wait(QEventLoop::ProcessEventsFlags flags) {
  while (!IsFinished()) {
    QCoreApplication::processEvents(flags);
  }
}

bool FutureBase::WaitFor(int msec, QEventLoop::ProcessEventsFlags flags) {
  if (msec < 0) {
    Wait(flags);
    return true;
  }

  QDeadlineTimer timer{msec};
  while (!IsFinished() && !timer.hasExpired()) {
    QCoreApplication::processEvents(flags, timer.remainingTime());
  }
  return IsFinished();
}

bool FutureBase::WaitUntil(const QDateTime& dateTime,
                           QEventLoop::ProcessEventsFlags flags) {
  auto now = QDateTime::currentDateTime();
  if (now >= dateTime) {
    return IsRunning();
  }
  return WaitFor(now.msecsTo(dateTime));
}

void FutureBase::Cancel() {
  d->canceled->store(true, std::memory_order_release);
}

bool FutureBase::IsCanceled() const {
  return d->canceled->load(std::memory_order_acquire);
}

void FutureBase::Suspend() {
  if (d->previous) {
    d->previous->q->Suspend();
  }
  d->suspended->store(true, std::memory_order_release);
}

void FutureBase::Resume() {
  if (d->previous) {
    d->previous->q->Resume();
  }
  d->suspended->store(false, std::memory_order_release);
}

bool FutureBase::IsSuspended() const {
  return d->suspended->load(std::memory_order_acquire);
}
}  // namespace impl
}  // namespace KtUtils
