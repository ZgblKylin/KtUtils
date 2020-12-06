#ifndef KT_UTILS_FUTURE_HELPER_H
#define KT_UTILS_FUTURE_HELPER_H

#include "../global.h"

namespace KtUtils {
namespace impl {
template <typename Func>
using ResultType = decltype(std::declval<Func>()());

class KT_UTILS_EXPORT FutureBase {
 public:
  FutureBase();
  virtual ~FutureBase() = default;
  FutureBase(const FutureBase& other) = default;
  FutureBase(FutureBase&& other) = default;
  FutureBase& operator=(const FutureBase& other) = default;
  FutureBase& operator=(FutureBase&& other) = default;

  void SuccessOn(const std::function<bool()>& func);

  void ExecuteOn(nullptr_t);
  void ExecuteOn(QObject* receiver,
                 Qt::ConnectionType type = Qt::AutoConnection);
  void ExecuteOn(QThreadPool* pool);

  void FailOn(const std::function<bool()>& func);
  void OnFailed(const std::function<void()>& func);

  void Finally(const std::function<void(void)>& func);

  bool IsRunning() const;
  void Run();

  bool IsFinished() const;
  void Wait(QEventLoop::ProcessEventsFlags flags);
  bool WaitFor(int msec,
               QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
  bool WaitUntil(const QDateTime& dateTime,
                 QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);

  void Cancel();
  bool IsCanceled() const;

  void Suspend();
  void Resume();
  bool IsSuspended() const;

 protected:
  friend class Future;
  struct Private;

  explicit FutureBase(FutureBase* prev, std::function<void(void)>&& func);

  std::shared_ptr<Private> d = nullptr;
};
}  // namespace impl
}  // namespace KtUtils

#endif  // KT_UTILS_FUTURE_HELPER_H
