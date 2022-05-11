#include <KtUtils/Global>

namespace KtUtils {
void Wait(const std::function<bool(void)>& isValid,
          QEventLoop::ProcessEventsFlags flags) {
  if (isValid) {
    while (!isValid()) {
      QCoreApplication::processEvents(flags, 10);
    }
  }
}

bool WaitFor(double timeout_milliseconds, QEventLoop::ProcessEventsFlags flags,
             const std::function<bool(void)>& isValid) {
  QElapsedTimer timer;
  timer.start();
  Wait(
      [&] {
        return (isValid && isValid()) ||
               (timer.nsecsElapsed() >= (timeout_milliseconds * 1e6));
      },
      flags);
  if (isValid) {
    return isValid();
  } else {
    return true;
  }
}

bool WaitUntil(const QDateTime& timeout_time,
               QEventLoop::ProcessEventsFlags flags,
               const std::function<bool(void)>& isValid) {
  return WaitFor(QDateTime::currentDateTime().msecsTo(timeout_time), flags,
                 isValid);
}
}  // namespace KtUtils
