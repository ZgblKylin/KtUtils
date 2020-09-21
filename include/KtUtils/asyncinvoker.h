#ifndef KT_UTILS_ASYNCINVOKER_H
#define KT_UTILS_ASYNCINVOKER_H

#include <functional>
#include <chrono>
#include "global.h"

namespace KtUtils {
/* ======== Declaration ======== */
struct AsyncInvokeEventData;
/** \brief Provide functionaliity like QTimer::singleShot with any functor. */
class KT_UTILS_EXPORT AsyncInvoker {
 public:
  class Future;

  /**
   * \brief
   * Invoke function asynchronously in specified thread and specified lifetime.
   * \warning
   * Function will NOT be executed in these cases:\n
   *   - IsAlive return false when invoke event scheduled.\n
   *   - Thread not running when invoke event scheduled.\n
   * \param function  Function to be invoked in target thread.
   * \param thread    Thread to be invoked in, nullptr for Qt's main thread.
   * \param delay     Milliseconds delay before execution, it's NOT guaranteed
   *                  to be precisely.
   * \param isAlive   Callback to judge if function's lifetime is valid.
   *                  For example, watch target object with QPointer,
   *                  QWeakPointer or std::weak_ptr.
   * \param type      Invoke type, behaves like signals&slots,
   *                  Qt::UniqueConnection is not supported.
   * \return Future handle to wait and acquire return value.
   */
  static Future Invoke(
      std::function<QVariant(void)> function, QThread* thread = nullptr,
      int delay_ms = 0,
      std::function<bool(void)> isAlive = [] { return true; });

  /**
   * \overload Invoke
   * \brief
   * Invoke function with receiver's lifetime in its thread.\n
   * Return type of function MUST be meta type.
   */
  template <typename Func, typename... Args>
  static Future Invoke(QObject* receiver, int delay_ms, const Func& funcion,
                       Args&&... args);

  /**
   * \overload Invoke
   * \brief
   * Invoke function in specified thread.\n
   * Return type of function MUST be meta type.
   */
  template <typename Func, typename... Args>
  static Future Invoke(QThread* thread, int delay_ms, const Func& funcion,
                       Args&&... args);

  /**
   * \overload Invoke
   * \brief
   * Invoke function with receiver's lifetime in specified thread.\n
   * Return type of function MUST be meta type.
   */
  template <typename Func, typename... Args>
  static Future Invoke(QObject* receiver, QThread* thread, int delay_ms,
                       const Func& funcion, Args&&... args);

  /** Future class provide same functionality as std::shared_future<QVariant>,
   * but NOT block the event loop when waiting. */
  class KT_UTILS_EXPORT Future {
   public:
    explicit Future(QSharedPointer<AsyncInvokeEventData> data = nullptr);
    Future(const Future&) = default;
    Future(Future&&) = default;
    Future& operator=(const Future&) = default;
    Future& operator=(Future&&) = default;
    ~Future();

    /** Return the underlying std::shared_future */
    std::shared_future<QVariant> future() const;

    bool valid() const;
    std::future_status status() const;

    /**
     * \brief Wait and get return value
     * \param flags Flags to be passed to Qt's event loop when waiting.
     * \return Return value for the invoked function, invalid QVariant for:
     *           - Function returns void.\n
     *           - Function not invoked:\n
     *             - isAlive return false before execute function.\n
     *             - Receiver went out lifetime before execute function.
     * \warning Will block infinitely if receiver thread is NOT running.
     * \sa std::shared_future::get
     */
    const QVariant& get(
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

    /**
     * \overload get
     * \sa QVariant::value
     */
    template <typename T>
    T get(QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

    /**
     * \brief Wait until future is valid.
     * \param flags Flags to be passed to Qt's event loop when waiting.
     * \warning Will block infinitely if receiver thread is NOT running.
     * \sa std::shared_future::wait, KtUtils::Wait */
    void wait(
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

    /** \sa std::shared::future::wait_for, KtUtils::WaitFor */
    template <class Rep, class Period>
    std::future_status wait_for(
        const std::chrono::duration<Rep, Period>& timeout_duration,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

    /** \sa std::shared_future::wait_until, KtUtils::WaitUntil */
    template <class Clock, class Duration>
    std::future_status wait_until(
        const std::chrono::time_point<Clock, Duration>& timeout_time,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

    /** \overload wait_for */
    std::future_status wait_for(
        int timeout_milliseconds,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

    /** \overload wait_until */
    std::future_status wait_until(
        const QDateTime& timeout_time,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) const;

   private:
    QSharedPointer<AsyncInvokeEventData> d_;
  };
};
/* ======== Declaration ======== */

/* ======== Definition ======== */
namespace impl {
template <typename T>
struct ApplyReturnValue {
  mutable QVariant* data_;
  explicit ApplyReturnValue(QVariant* data) : data_(data) {}
};
template <typename T, typename U>
inline void operator,(T&& value, const ApplyReturnValue<U>& container) {
  container.data_->setValue(std::forward<T>(value));
}
template <typename T>
inline void operator,(T, const ApplyReturnValue<void>&) {}
}  // namespace impl

template <typename Func, typename... Args>
inline AsyncInvoker::Future AsyncInvoker::Invoke(QObject* receiver,
                                                 int delay_ms,
                                                 const Func& function,
                                                 Args&&... args) {
  if (!receiver) {
    receiver = qApp;
  }
  return Invoke(receiver, receiver->thread(), delay_ms, function,
                std::forward<Args>(args)...);
}

template <typename Func, typename... Args>
inline AsyncInvoker::Future AsyncInvoker::Invoke(QThread* thread, int delay_ms,
                                                 const Func& function,
                                                 Args&&... args) {
  return Invoke(thread, thread, delay_ms, function,
                std::forward<Args>(args)...);
}

template <typename Func, typename... Args>
inline AsyncInvoker::Future AsyncInvoker::Invoke(QObject* receiver,
                                                 QThread* thread, int delay_ms,
                                                 const Func& func,
                                                 Args&&... args) {
  if (!receiver) {
    receiver = qApp;
  }
  QPointer<QObject> pointer{receiver};
  auto isAlive = [pointer] { return bool(pointer); };
  auto f = std::bind(func, std::forward<Args>(args)...);
  auto function = [f] {
    using return_t = decltype(func(std::forward<Args>(args)...));
    QVariant ret;
    f(), impl::ApplyReturnValue<return_t>(&ret);
    return ret;
  };
  return Invoke(std::move(function), thread, delay_ms, std::move(isAlive));
}

template <typename T>
inline T AsyncInvoker::Future::get(QEventLoop::ProcessEventsFlags flags) const {
  return get(flags).value<T>();
}

inline std::future_status AsyncInvoker::Future::wait_for(
    int timeout_milliseconds, QEventLoop::ProcessEventsFlags flags) const {
  return wait_until(std::chrono::steady_clock::now() +
                        std::chrono::milliseconds(timeout_milliseconds),
                    flags);
}

inline std::future_status AsyncInvoker::Future::wait_until(
    const QDateTime& timeout_time, QEventLoop::ProcessEventsFlags flags) const {
  return wait_until(
      std::chrono::system_clock::time_point() +
          std::chrono::milliseconds(timeout_time.toMSecsSinceEpoch()),
      flags);
}

template <class Rep, class Period>
inline std::future_status AsyncInvoker::Future::wait_for(
    const std::chrono::duration<Rep, Period>& timeout_duration,
    QEventLoop::ProcessEventsFlags flags) const {
  std::chrono::duration<double, std::milli> duration = timeout_duration;
  return wait_until(steady_clock::now() + timeout_duration, flags);
}

template <class Clock, class Duration>
inline std::future_status AsyncInvoker::Future::wait_until(
    const std::chrono::time_point<Clock, Duration>& timeout_time,
    QEventLoop::ProcessEventsFlags flags) const {
  WaitUntil(timeout_time, flags, [this] { return valid(); });
  return status();
}
/* ======== Definition ======== */
}  // namespace KtUtils

#endif  // KT_UTILS_ASYNCINVOKER_H
