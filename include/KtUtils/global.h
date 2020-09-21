#ifndef KT_UTILS_GLOBAL_H
#define KT_UTILS_GLOBAL_H

#include <chrono>
#include <functional>

#define QT_MESSAGELOGCONTEXT
#include <Qt>
#include <QtCore/QtGlobal>
#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtSvg/QtSvg>

#ifndef KT_UTILS_SHARED_LIBRARY
#define KT_UTILS_EXPORT
#else
#ifdef KT_UTILS_BUILD_SHARED_LIBRARY
#define KT_UTILS_EXPORT Q_DECL_EXPORT
#else
#define KT_UTILS_EXPORT Q_DECL_IMPORT
#endif
#endif

namespace KtUtils {
/**
 * \brief Wait when specific criteria is satisfied, but NOT blocking
 *                Qt's event loop.
 * \param isValid Callback to tell if specific criteria is satisfied and we can
 *                stop waiting.
 * \param flags   Flags for run Qt's eventloop, e.g. using
 *                QEventLoop::ExcludeUserInputEvents to wait without allowing
 *                user input.
 */
void KT_UTILS_EXPORT
Wait(const std::function<bool(void)>& isValid,
     QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);

/**
 * \brief Wait for given time interval or specific criteria satisfied(if given)
 *        WITHOUT blocking Qt's event loop.
 * \param timeout_milliseconds  Maximum interval to wait for.
 * \param flags   Flags for run Qt's eventloop, e.g. using
 *                QEventLoop::ExcludeUserInputEvents to wait without allowing
 *                user input.
 * \param isValid Callback to tell if specific criteria is satisfied and we can
 *                stop waiting. Immediately return when valid, even not reach
 *                timeout.
 * \return false if isValid is given and return false until timeout, otherwise
 *         true.
 */
bool KT_UTILS_EXPORT
WaitFor(int timeout_milliseconds,
        QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents,
        const std::function<bool(void)>& isValid = {});

/**
 * \brief Wait until given time or specific criteria satisfied(if given) WITHOUT
 *        blocking Qt's event loop.
 * \param timeout_time  Deadline time point to wait until.
 * \param flags   Flags for run Qt's eventloop, e.g. using
 *                QEventLoop::ExcludeUserInputEvents to wait without allowing
 *                user input.
 * \param isValid Callback to tell if specific criteria is satisfied and we can
 *                stop waiting. Immediately return when valid, even not reach
 *                timeout.
 * \return false if isValid is given and return false until timeout, otherwise
 *         true.
 */
bool KT_UTILS_EXPORT
WaitUntil(const QDateTime& timeout_time,
          QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents,
          const std::function<bool(void)>& isValid = {});

/** \overload WaitFor */
template <class Rep, class Period>
bool WaitFor(const std::chrono::duration<Rep, Period>& timeout_duration,
             QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents,
             const std::function<bool(void)>& isValid = {});

/** \overload WaitUntil */
template <class Clock, class Duration>
bool WaitUntil(const std::chrono::time_point<Clock, Duration>& timeout_time,
               QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents,
               const std::function<bool(void)>& isValid = {});
/* ======== Declaration ======== */

/* ======== Definition ======== */
inline bool WaitFor(int timeout_milliseconds,
                    QEventLoop::ProcessEventsFlags flags,
                    const std::function<bool(void)>& isValid) {
  return WaitUntil(std::chrono::steady_clock::now() +
                       std::chrono::milliseconds(timeout_milliseconds),
                   flags, isValid);
}

inline bool WaitUntil(const QDateTime& timeout_time,
                      QEventLoop::ProcessEventsFlags flags,
                      const std::function<bool(void)>& isValid) {
  return WaitUntil(
      std::chrono::system_clock::time_point() +
          std::chrono::milliseconds(timeout_time.toMSecsSinceEpoch()),
      flags, isValid);
}

template <class Rep, class Period>
inline bool WaitFor(const std::chrono::duration<Rep, Period>& timeout_duration,
                    QEventLoop::ProcessEventsFlags flags,
                    const std::function<bool(void)>& isValid) {
  return WaitUntil(std::chrono::steady_clock::now() + timeout_duration, flags,
                   isValid);
}

template <class Clock, class Duration>
inline bool WaitUntil(
    const std::chrono::time_point<Clock, Duration>& timeout_time,
    QEventLoop::ProcessEventsFlags flags,
    const std::function<bool(void)>& isValid) {
  auto isValidOrTimeout = [timeout_time, &isValid] {
    if (isValid) {
      if (isValid()) {
        return true;
      }
    }
    return Clock::now() >= timeout_time;
  };
  Wait(isValidOrTimeout, flags);
  if (isValid) {
    return isValid();
  } else {
    return true;
  }
}
}  // namespace KtUtils

#endif  // KT_UTILS_GLOBAL_H
