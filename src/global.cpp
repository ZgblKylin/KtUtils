#include <KtUtils/Global>

namespace KtUtils {
void Wait(const std::function<bool(void)>& isValid,
          QEventLoop::ProcessEventsFlags flags) {
  if (!isValid) {
    return;
  }
  QEventLoop eventLoop;
  while (!isValid()) {
    eventLoop.processEvents(flags);
  }
}
}  // namespace KtUtils
