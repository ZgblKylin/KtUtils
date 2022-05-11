#ifndef KTUTILS_GLOBAL_HPP
#define KTUTILS_GLOBAL_HPP

#include <chrono>
#include <functional>

#define QT_MESSAGELOGCONTEXT
#include <QtCore/QtCore>
#include <QtConcurrent/QtConcurrent>
#include <QtGui/QtGui>
#include <QtSvg/QtSvg>

#ifdef KTUTILS_SHARED_LIBRARY
#define KTUTILS_EXPORT Q_DECL_IMPORT
#elif defined(KTUTILS_BUILD_SHARED_LIBRARY)
#define KTUTILS_EXPORT Q_DECL_EXPORT
#else
#define KTUTILS_EXPORT
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
KTUTILS_EXPORT void Wait(
    const std::function<bool(void)>& isValid,
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
KTUTILS_EXPORT bool WaitFor(
    double timeout_milliseconds,
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
KTUTILS_EXPORT bool WaitUntil(
    const QDateTime& timeout_time,
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
/* ================ Declaration ================ */

/* ================ Definition ================ */
template <class Rep, class Period>
inline bool WaitFor(const std::chrono::duration<Rep, Period>& timeout_duration,
                    QEventLoop::ProcessEventsFlags flags,
                    const std::function<bool(void)>& isValid) {
  return WaitFor(
      std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(
          timeout_duration)
          .count(),
      flags, isValid);
}

template <class Clock, class Duration>
inline bool WaitUntil(
    const std::chrono::time_point<Clock, Duration>& timeout_time,
    QEventLoop::ProcessEventsFlags flags,
    const std::function<bool(void)>& isValid) {
  return WaitFor(timeout_time - Clock::now(), flags, isValid);
}
}  // namespace KtUtils

#endif  // KTUTILS_GLOBAL_HPP
