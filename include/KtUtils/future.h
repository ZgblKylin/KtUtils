#ifndef KT_UTILS_FUTURE_H
#define KT_UTILS_FUTURE_H

#include "global.h"
#include "impl/asyncinvoker_helper.h"

namespace KtUtils {
/* ================ Declaration ================ */
template <typename T>
class Future : public impl::FutureBase {
 public:
  Future();
  ~Future() override = default;
  Future(const Future<T>&&) = default;
  Future(Future<T>&&) = default;
  Future<T>& operator=(const Future<T>&) = default;
  Future<T>& operator=(Future<T>&&) = default;

  template <typename Signal>
  Future<T>& SuccessOn(QObject* sender, Signal&& signal);
  Future<T>& SuccessOn(const std::function<bool()>& func);

  Future<T>& void ExecuteOn(nullptr_t);
  template <typename Func>
  Future<impl::ResultType<Func>> Then(Func&& func);
  Future<T>& void ExecuteOn(QObject* receiver,
                            Qt::ConnectionType type = Qt::AutoConnection);
  template <typename Slot>
  Future<impl::ResultType<Func>> Then(
      QObject* receiver, Slot&& slot,
      Qt::ConnectionType type = Qt::AutoConnection);
  Future<T>& void ExecuteOn(QThreadPool* pool);
  template <typename Func>
  Future<impl::ResultType<Func>> Then(QThreadPool* pool, Func&& func);

  template <typename Signal>
  Future<T>& FailOn(QObject* sender, Signal&& signal);
  Future<T>& FailOn(const std::function<bool()>& func);
  Future<T>& OnFailed(const std::function<void()>& func);

  Future<T>& Finally(const std::function<void(void)>& func);

  bool IsRunning() const;
  void Run();

  bool IsFinished() const;
  void Wait(QEventLoop::ProcessEventsFlags flags);
  bool WaitFor(int msec,
               QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
  template <typename Rep, typename Period = std::ratio<1>>
  bool WaitFor(const std::chrono::duration<Rep, Period>& duration,
               QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
  bool WaitUntil(const QDateTime& dateTime,
                 QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
  template <typename Clock, typename Duration = typename Clock::duration>
  bool WaitUntil(const std::chrono::time_point<Clock, Duration>& timePoint,
                 QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);

  void Cancel();
  bool IsCanceled() const;

  void Suspend();
  void Resume();
  bool IsSuspended() const;

  T Result();

 private:
  friend class impl::FutureBase;
  std::shared_future<T> future_{};
  explicit Future(impl::FutureBase* prev, std::function<void(void)>&& func,
                  std::shared_future<T>&& future);
};

template <typename Func>
Future<impl::ResultType<Func>> Async(Func&& func);

template <typename Slot>
Future<impl::ResultType<Func>> Async(
    QObject* receiver, Slot&& slot,
    Qt::ConnectionType type = Qt::AutoConnection);

template <typename Func>
Future<impl::ResultType<Func>> Async(QThreadPool* pool, Func&& func);
/* ================ Declaration ================ */

/* ================ Definition ================ */
template <typename T>
inline T Future<T>::Future() {}

template <typename T>
inline Future<T>::Future(impl::FutureBase* prev,
                         std::function<void(void)>&& func,
                         std::shared_future<T>&& future)
    : impl::FutureBase(prev, func), future_(future) {}

template <typename T>
template <typename Signal>
inline Future<T>& Future<T>::SuccessOn(QObject* sender, Signal&& signal) {
  auto successed = std::make_shared<std::atomic_bool>(false);
  QObject::connect(
      sender, signal, sender,
      [successed] { successed->store(true, std::memory_order_release); },
      Qt::DirectConnection);
  return SuccessOn(
      [successed] { return successed->load(std::memory_order_acquire); });
}

template <typename T>
inline Future<T>& Future<T>::SuccessOn(const std::function<bool()>& func) {
  impl::FutureBase::SuccessOn(func);
  return *this;
}

template <typename T>
inline Future<T>& Future<T>::ExecuteOn(nullptr_t) {
  impl::FutureBase::ExecuteOn(nullptr_t);
  return *this;
}

template <typename T>
template <typename Func>
inline Future<impl::ResultType<Func>> Future<T>::Then(Func&& func) {
  auto promise = std::make_shared<std::promise<impl::ResultType<Func>>>();
  return Future<impl::ResultType<Func>>(
      this,
      [func, promise] {
        if (func) {
          promise_->set_value(func());
        }
      },
      promise->get_future());
}

template <typename T>
inline Future<T>& Future<T>::ExecuteOn(QObject* receiver,
                                       Qt::ConnectionType type) {
  impl::FutureBase::ExecuteOn(receiver, type);
  return *this;
}

template <typename T>
template <typename Slot>
inline Future<impl::ResultType<Func>> Future<T>::Then(QObject* receiver,
                                                      Slot&& slot,
                                                      Qt::ConnectionType type) {
  Future<impl::ResultType<Func>> ret = Then(std::forward(slot));
  ret.ExecuteOn(receiver, type);
  return ret;
}

template <typename T>
inline Future<T>& Future<T>::ExecuteOn(QThreadPool* pool) {
  impl::FutureBase::ExecuteOn(pool);
  return *this;
}

template <typename T>
template <typename Func>
inline Future<impl::ResultType<Func>> Future<T>::Then(QThreadPool* pool,
                                                      Func&& func) {
  Future<impl::ResultType<Func>> ret = Then(std::forward(func));
  ret.ExecuteOn(pool);
  return ret;
}

template <typename T>
template <typename Signal>
inline Future<T>& Future<T>::FailOn(QObject* sender, Signal&& signal) {
  auto failed = std::make_shared<std::atomic_bool>(false);
  QObject::connect(
      failed, signal, failed,
      [failed] { failed->store(true, std::memory_order_release); },
      Qt::DirectConnection);
  return FailedOn([failed] { return failed->load(std::memory_order_acquire); });
}

template <typename T>
inline Future<T>& Future<T>::FailOn(const std::function<bool()>& func) {
  impl::FutureBase::FailOn(func);
  return *this;
}

template <typename T>
inline Future<T>& Future<T>::OnFailed(const std::function<void()>& func) {
  impl::FutureBase::OnFailed(func);
  return *this;
}

template <typename T>
inline inline Future<T>& Future<T>::Finally(
    const std::function<void(void)>& func) {
  impl::FutureBase::Finally(func);
  return *this;
}

template <typename T>
inline bool Future<T>::IsRunning() const {
  return impl::FutureBase::IsRunning();
}

template <typename T>
inline void Future<T>::Run() {
  impl::FutureBase::Run();
}

template <typename T>
inline bool Future<T>::IsFinished() const {
  return impl::FutureBase::IsFinished();
}

template <typename T>
inline void Future<T>::Wait(QEventLoop::ProcessEventsFlags flags) {
  impl::FutureBase::Wait();
}

template <typename T>
inline bool Future<T>::WaitFor(int msec, QEventLoop::ProcessEventsFlags flags) {
  return impl::FutureBase::WaitFor(msec, flags);
}

template <typename T>
template <typename Rep, typename Period = std::ratio<1>>
inline bool Future<T>::WaitFor(
    const std::chrono::duration<Rep, Period>& duration,
    QEventLoop::ProcessEventsFlags flags) {
  return WaitFor(
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(),
      flags);
}

template <typename T>
inline bool Future<T>::WaitUntil(const QDateTime& dateTime,
                                 QEventLoop::ProcessEventsFlags flags) {
  return impl::FutureBase::WaitUntil(dateTime, flags);
}

template <typename T>
template <typename Clock, typename Duration = typename Clock::duration>
inline bool Future<T>::WaitUntil(
    const std::chrono::time_point<Clock, Duration>& timePoint,
    QEventLoop::ProcessEventsFlags flags) {
  auto now = std::chrono::steady_clock::now();
  if (now >= timePoint) {
    return IsRunning();
  }
  return WaitFor(timePoint - now);
}

template <typename T>
inline void Future<T>::Cancel() {
  impl::FutureBase::Cancel();
}

template <typename T>
inline bool Future<T>::IsCanceled() const {
  return impl::FutureBase::IsCanceled();
}

template <typename T>
inline void Future<T>::Suspend() {
  impl::FutureBase::Suspend();
}

template <typename T>
inline void Future<T>::Resume() {
  impl::FutureBase::Resume();
}

template <typename T>
inline bool Future<T>::IsSuspended() const {
  return impl::FutureBase::IsSuspended();
}

template <typename T>
inline T Future<T>::Result() {
  return future_->get();
}

template <typename Func>
inline Future<impl::ResultType<Func>> Async(Func&& func) {
  return Future<void>().Then(std::forward(func));
}

template <typename Slot>
inline Future<impl::ResultType<Func>> Async(QObject* receiver, Slot&& slot,
                                            Qt::ConnectionType type) {
  return Future<void>().Then(receiver, std::forward(slot), type);
}

template <typename Func>
inline Future<impl::ResultType<Func>> Async(QThreadPool* pool, Func&& func) {
  return Future<void>().Then(pool, std::forward(func));
}
/* ================ Definition ================ */
}  // namespace KtUtils

#endif  // KT_UTILS_FUTURE_H
