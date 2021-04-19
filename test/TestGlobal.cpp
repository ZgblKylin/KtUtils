#include "TestGlobal.hpp"
#include <iomanip>
#include <sstream>
#include <QtConcurrent/QtConcurrent>
#include <QtTest/QtTest>

using namespace std::chrono;
using namespace std::literals::chrono_literals;
using namespace KtUtils;

void WaitFunc(volatile bool* waitFlag, int timeout) {
  *waitFlag = false;
  QElapsedTimer timer;
  timer.start();
  Wait([&waitFlag, &timer, timeout] {
    if (timer.elapsed() > timeout) {
      *waitFlag = true;
    }
    return *waitFlag;
  });
}

void TestGlobal::Wait() {
  static constexpr int kTimeout = 100;
  volatile bool waitFlag = false;
  auto future = QtConcurrent::run(&WaitFunc, &waitFlag, kTimeout);
  QElapsedTimer timer;
  timer.start();
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9));
  QVERIFY(timer.elapsed() < (kTimeout + 100));
  future.waitForFinished();
}

void WaitForFunc(volatile bool* waitFlag, int ms,
                 const std::function<bool(void)> isValid = {}) {
  *waitFlag = ::WaitFor(ms, QEventLoop::AllEvents, isValid);
}

void TestGlobal::WaitFor() {
  static constexpr int kTimeout = 100;
  volatile bool waitFlag = false;
  QElapsedTimer timer;
  timer.start();
  auto future = QtConcurrent::run(&WaitForFunc, &waitFlag, kTimeout,
                                  std::function<bool(void)>{});
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9));
  QVERIFY(timer.elapsed() < (kTimeout + 100));
  future.waitForFinished();
}

void TestGlobal::WaitFor_valid() {
  static constexpr int kTimeout = 100;
  volatile bool waitFlag = false;
  QElapsedTimer validTimer;
  validTimer.start();
  auto valid = [&validTimer] { return validTimer.elapsed() >= (kTimeout / 2); };
  QElapsedTimer timer;
  timer.start();
  auto future = QtConcurrent::run(&WaitForFunc, &waitFlag, kTimeout, valid);
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9 / 2));
  QVERIFY(timer.elapsed() < kTimeout);
  future.waitForFinished();
}

void TestGlobal::WaitFor_invalid() {
  static constexpr int kTimeout = 100;
  volatile bool waitFlag = true;
  auto invalid = [] { return false; };
  QElapsedTimer timer;
  timer.start();
  auto future = QtConcurrent::run(&WaitForFunc, &waitFlag, kTimeout, invalid);
  QTRY_VERIFY_WITH_TIMEOUT(!waitFlag, 200);
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9));
  QVERIFY(timer.elapsed() < (kTimeout + 100));
  future.waitForFinished();
}

template <typename Duration>
void WaitForStdFunc(volatile bool* waitFlag, Duration duration,
                    const std::function<bool(void)> isValid = {}) {
  *waitFlag = ::WaitFor(duration, QEventLoop::AllEvents, isValid);
}

void TestGlobal::WaitFor_std() {
  static constexpr auto kDuration = 100ms;
  volatile bool waitFlag = false;
  auto now = steady_clock::now();
  auto future =
      QtConcurrent::run(&WaitForStdFunc<decltype(kDuration)>, &waitFlag,
                        kDuration, std::function<bool(void)>{});
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  duration<double, std::milli> elapsed = steady_clock::now() - now;
  QVERIFY(elapsed >= (kDuration * 0.9));
  QVERIFY(elapsed <= (kDuration + 100ms));
  future.waitForFinished();
}

void TestGlobal::WaitFor_std_valid() {
  static constexpr auto kDuration = 100ms;
  volatile bool waitFlag = false;
  auto validTimer = steady_clock::now();
  auto valid = [&validTimer] {
    return (steady_clock::now() - validTimer) >= (kDuration / 2);
  };
  auto now = steady_clock::now();
  auto future = QtConcurrent::run(&WaitForStdFunc<decltype(kDuration)>,
                                  &waitFlag, kDuration, valid);
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  duration<double, std::milli> elapsed = steady_clock::now() - now;
  QVERIFY(elapsed >= (kDuration * 0.9 / 2));
  QVERIFY(elapsed < kDuration);
  future.waitForFinished();
}

void TestGlobal::WaitFor_std_invalid() {
  static constexpr auto kDuration = 100ms;
  volatile bool waitFlag = true;
  auto invalid = [] { return false; };
  auto now = steady_clock::now();
  auto future = QtConcurrent::run(&WaitForStdFunc<decltype(kDuration)>,
                                  &waitFlag, kDuration, invalid);
  QTRY_VERIFY_WITH_TIMEOUT(!waitFlag, 200);
  duration<double, std::milli> elapsed = steady_clock::now() - now;
  QVERIFY(elapsed >= (kDuration * 0.9));
  QVERIFY(elapsed <= (kDuration + 100ms));
  future.waitForFinished();
}

void WaitUntilFunc(volatile bool* waitFlag, const QDateTime& dateTime,
                   const std::function<bool(void)> isValid = {}) {
  *waitFlag = ::WaitUntil(dateTime, QEventLoop::AllEvents, isValid);
}

void TestGlobal::WaitUntil() {
  volatile bool waitFlag = false;
  static constexpr int kTimeout = 100;
  const auto kDateTime = QDateTime::currentDateTime().addMSecs(kTimeout);
  QElapsedTimer timer;
  timer.start();
  auto future = QtConcurrent::run(&WaitUntilFunc, &waitFlag, kDateTime,
                                  std::function<bool(void)>{});
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9));
  QVERIFY(timer.elapsed() < (kTimeout + 100));
  future.waitForFinished();
}

void TestGlobal::WaitUntil_valid() {
  volatile bool waitFlag = false;
  static constexpr int kTimeout = 100;
  const auto kDateTime = QDateTime::currentDateTime().addMSecs(kTimeout);
  QElapsedTimer validTimer;
  validTimer.start();
  auto valid = [&validTimer] { return validTimer.elapsed() >= 50; };
  QElapsedTimer timer;
  timer.start();
  auto future = QtConcurrent::run(&WaitUntilFunc, &waitFlag, kDateTime, valid);
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  QVERIFY((timer.elapsed() >= 50) && (timer.elapsed() < 100));
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9) / 2);
  QVERIFY(timer.elapsed() < (kTimeout));
  future.waitForFinished();
}

void TestGlobal::WaitUntil_invalid() {
  volatile bool waitFlag = true;
  static constexpr int kTimeout = 100;
  const auto kDateTime = QDateTime::currentDateTime().addMSecs(kTimeout);
  auto invalid = [] { return false; };
  QElapsedTimer timer;
  timer.start();
  auto future =
      QtConcurrent::run(&WaitUntilFunc, &waitFlag, kDateTime, invalid);
  QTRY_VERIFY_WITH_TIMEOUT(!waitFlag, 200);
  QVERIFY(timer.elapsed() >= (kTimeout * 0.9));
  QVERIFY(timer.elapsed() < (kTimeout + 100));
  future.waitForFinished();
}

void WaitUntilStdFunc(volatile bool* waitFlag,
                      steady_clock::time_point timePoint,
                      const std::function<bool(void)> isValid = {}) {
  *waitFlag = ::WaitUntil(timePoint, QEventLoop::AllEvents, isValid);
}

void TestGlobal::WaitUntil_std() {
  volatile bool waitFlag = false;
  static constexpr auto kDuration = 100ms;
  const auto kTimePoint = steady_clock::now() + kDuration;
  auto now = steady_clock::now();
  auto future = QtConcurrent::run(&WaitUntilStdFunc, &waitFlag, kTimePoint,
                                  std::function<bool(void)>{});
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  duration<double, std::milli> elapsed = steady_clock::now() - now;
  QVERIFY(elapsed >= (kDuration * 0.9));
  QVERIFY(elapsed <= (kDuration + 100ms));
  future.waitForFinished();
}

void TestGlobal::WaitUntil_std_valid() {
  volatile bool waitFlag = false;
  static constexpr auto kDuration = 100ms;
  const auto kTimePoint = steady_clock::now() + kDuration;
  steady_clock::time_point validTime = steady_clock::now();
  auto valid = [&validTime] {
    return (steady_clock::now() - validTime) >= (kDuration / 2);
  };
  auto now = steady_clock::now();
  auto future =
      QtConcurrent::run(&WaitUntilStdFunc, &waitFlag, kTimePoint, valid);
  QTRY_VERIFY_WITH_TIMEOUT(waitFlag, 200);
  duration<double, std::milli> elapsed = steady_clock::now() - now;
  QVERIFY(elapsed >= (kDuration * 0.9 / 2));
  QVERIFY(elapsed < kDuration);
  future.waitForFinished();
}

void TestGlobal::WaitUntil_std_invalid() {
  volatile bool waitFlag = true;
  static constexpr auto kDuration = 100ms;
  const auto kTimePoint = steady_clock::now() + kDuration;
  auto invalid = [] { return false; };
  auto now = steady_clock::now();
  auto future =
      QtConcurrent::run(&WaitUntilStdFunc, &waitFlag, kTimePoint, invalid);
  QTRY_VERIFY_WITH_TIMEOUT(!waitFlag, 200);
  duration<double, std::milli> elapsed = steady_clock::now() - now;
  QVERIFY(elapsed >= (kDuration * 0.9));
  QVERIFY(elapsed <= (kDuration + 100ms));
  future.waitForFinished();
}

QTEST_GUILESS_MAIN(TestGlobal)
