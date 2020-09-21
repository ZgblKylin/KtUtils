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
}  // namespace KtUtils
