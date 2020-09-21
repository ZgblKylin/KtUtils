#ifndef KT_UTILS_TEST_ASYNC_INVOKER_H
#define KT_UTILS_TEST_ASYNC_INVOKER_H

#include <KtUtils/Global>

class TestAsyncInvoker : public QObject
{
  Q_OBJECT

 private Q_SLOTS:
  void initTestCase();
  void cleanupTestCase();

  void init();

  static QVariant InvokeFunc_variant();
  static int InvokeFunc_args(int value);

  void InvokeFuncThreadDelayAlive();
  void InvokeFuncThreadDelayAlive_thread();
  void InvokeFuncThreadDelayAlive_thread_delay();
  void InvokeFuncThreadDelayAlive_thread_delay_alive();
  void InvokeFuncThreadDelayAlive_thread_delay_notalive();

  void InvokeReceiverDelayFunc();
  void InvokeReceiverDelayFunc_receiver();
  void InvokeReceiverDelayFunc_receiver_delay();

  void InvokeThreadDelayFunc();
  void InvokeThreadDelayFunc_receiver();
  void InvokeThreadDelayFunc_receiver_delay();

  void InvokeObjectThreadDelayFunc();
  void InvokeObjectThreadDelayFunc_receiver();
  void InvokeObjectThreadDelayFunc_receiver_thread();
  void InvokeObjectThreadDelayFunc_receiver_thread_delay();

  void Future_future();
  void Future_get_QVariant();
  void Future_get_T();
  void Future_get_invalid();
  void Future_wait();
  void Future_wait_for();
  void Future_wait_for_Qt();
  void Future_wait_until();
  void Future_wait_until_Qt();

 private:

  QThread* thread_ = nullptr;
  QObject* object_ = nullptr;
  static volatile QThread* invokeThread_;
};

#endif  // KT_UTILS_TEST_ASYNC_INVOKER_H
