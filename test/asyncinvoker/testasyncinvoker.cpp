#include "testasyncinvoker.h"
#include <QtTest/QtTest>
#include <KtUtils/AsyncInvoker>

using namespace std::chrono;
using namespace std::literals::chrono_literals;
using namespace KtUtils;

volatile QThread* TestAsyncInvoker::invokeThread_ = nullptr;

void TestAsyncInvoker::initTestCase() {
  thread_ = new QThread(this);
  thread_->start();

  object_ = new QObject;
  object_->moveToThread(thread_);
}

void TestAsyncInvoker::cleanupTestCase() {
  object_->deleteLater();
  thread_->quit();
  thread_->wait();
}

void TestAsyncInvoker::init() { invokeThread_ = nullptr; }

QVariant TestAsyncInvoker::InvokeFunc_variant() {
  invokeThread_ = QThread::currentThread();
  return 42;
}

int TestAsyncInvoker::InvokeFunc_args(int value) {
  invokeThread_ = QThread::currentThread();
  return value;
}

void TestAsyncInvoker::InvokeFuncThreadDelayAlive() {
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(&TestAsyncInvoker::InvokeFunc_variant);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, QThread::currentThread());
}

void TestAsyncInvoker::InvokeFuncThreadDelayAlive_thread() {
  QElapsedTimer timer;
  timer.start();
  auto future =
      AsyncInvoker::Invoke(&TestAsyncInvoker::InvokeFunc_variant, thread_);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, thread_);
}

void TestAsyncInvoker::InvokeFuncThreadDelayAlive_thread_delay() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(&TestAsyncInvoker::InvokeFunc_variant,
                                     thread_, kDelay);
  QCOMPARE(future.get<int>(), 42);
  QCOMPARE(invokeThread_, thread_);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::InvokeFuncThreadDelayAlive_thread_delay_alive() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(&TestAsyncInvoker::InvokeFunc_variant,
                                     thread_, kDelay, [] { return true; });
  QCOMPARE(future.get<int>(), 42);
  QCOMPARE(invokeThread_, thread_);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::InvokeFuncThreadDelayAlive_thread_delay_notalive() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(&TestAsyncInvoker::InvokeFunc_variant,
                                     thread_, kDelay, [] { return false; });
  future.get();
  QCOMPARE(invokeThread_, nullptr);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::InvokeReceiverDelayFunc() {
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(static_cast<QObject*>(nullptr), 0,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, QThread::currentThread());
}

void TestAsyncInvoker::InvokeReceiverDelayFunc_receiver() {
  QElapsedTimer timer;
  timer.start();
  auto future =
      AsyncInvoker::Invoke(object_, 0, &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, thread_);
}

void TestAsyncInvoker::InvokeReceiverDelayFunc_receiver_delay() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  QCOMPARE(invokeThread_, thread_);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::InvokeThreadDelayFunc() {
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(static_cast<QThread*>(nullptr), 0,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, QThread::currentThread());
}

void TestAsyncInvoker::InvokeThreadDelayFunc_receiver() {
  QElapsedTimer timer;
  timer.start();
  auto future =
      AsyncInvoker::Invoke(thread_, 0, &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, thread_);
}

void TestAsyncInvoker::InvokeThreadDelayFunc_receiver_delay() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  QCOMPARE(invokeThread_, thread_);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::InvokeObjectThreadDelayFunc() {
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(nullptr, nullptr, 0,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, QThread::currentThread());
}

void TestAsyncInvoker::InvokeObjectThreadDelayFunc_receiver() {
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, nullptr, 0,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, QThread::currentThread());
}

void TestAsyncInvoker::InvokeObjectThreadDelayFunc_receiver_thread() {
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(nullptr, thread_, 0,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  qInfo().nospace() << (timer.nsecsElapsed() / 1e6) << "ms";
  QCOMPARE(invokeThread_, thread_);
}

void TestAsyncInvoker::InvokeObjectThreadDelayFunc_receiver_thread_delay() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<int>(), 42);
  QCOMPARE(invokeThread_, thread_);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::Future_future() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.future().get().value<int>(), 42);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::Future_get_QVariant() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get().value<int>(), 42);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::Future_get_T() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  QCOMPARE(future.get<QString>(), "42");
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::Future_get_invalid() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(&TestAsyncInvoker::InvokeFunc_variant,
                                     thread_, kDelay, [] { return false; });
  QVERIFY(!future.get().isValid());
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
}

void TestAsyncInvoker::Future_wait() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  future.wait();
  QVERIFY(timer.elapsed() >= (kDelay * 0.9));
  QVERIFY(timer.elapsed() < (kDelay + 100));
  timer.restart();
  QCOMPARE(future.get<QString>(), "42");
  qInfo().nospace() << "get " << (timer.nsecsElapsed() / 1e6) << "ms";
}

void TestAsyncInvoker::Future_wait_for() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  future.wait_for(milliseconds(kDelay / 2));
  QVERIFY(timer.elapsed() >= (kDelay * 0.9 / 2));
  QVERIFY(timer.elapsed() < kDelay);
  timer.restart();
  QCOMPARE(future.get<QString>(), "42");
  qInfo().nospace() << "get " << (timer.nsecsElapsed() / 1e6) << "ms";
}

void TestAsyncInvoker::Future_wait_for_Qt() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  future.wait_for(kDelay / 2);
  QVERIFY(timer.elapsed() >= (kDelay * 0.9 / 2));
  QVERIFY(timer.elapsed() < kDelay);
  timer.restart();
  QCOMPARE(future.get<QString>(), "42");
  qInfo().nospace() << "get " << (timer.nsecsElapsed() / 1e6) << "ms";
}

void TestAsyncInvoker::Future_wait_until() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  future.wait_until(steady_clock::now() + milliseconds(kDelay / 2));
  qDebug() << timer.nsecsElapsed() / 1e6;
  QVERIFY(timer.elapsed() >= (kDelay * 0.9 / 2));
  QVERIFY(timer.elapsed() < kDelay);
  timer.restart();
  QCOMPARE(future.get<QString>(), "42");
  qInfo().nospace() << "get " << (timer.nsecsElapsed() / 1e6) << "ms";
}

void TestAsyncInvoker::Future_wait_until_Qt() {
  static constexpr int kDelay = 100;
  QElapsedTimer timer;
  timer.start();
  auto future = AsyncInvoker::Invoke(object_, thread_, kDelay,
                                     &TestAsyncInvoker::InvokeFunc_args, 42);
  future.wait_until(QDateTime::currentDateTime().addMSecs(kDelay / 2));
  QVERIFY(timer.elapsed() >= (kDelay * 0.9 / 2));
  QVERIFY(timer.elapsed() < kDelay);
  timer.restart();
  QCOMPARE(future.get<QString>(), "42");
  qInfo().nospace() << "get " << (timer.nsecsElapsed() / 1e6) << "ms";
}

QTEST_GUILESS_MAIN(TestAsyncInvoker)
