#pragma once
#ifndef KTUTILS_TEST_GLOBAL_HPP
#define KTUTILS_TEST_GLOBAL_HPP

class TestGlobal : public QObject
{
  Q_OBJECT

 private Q_SLOTS:
  void Wait();

  void WaitFor();
  void WaitFor_valid();
  void WaitFor_invalid();

  void WaitFor_std();
  void WaitFor_std_valid();
  void WaitFor_std_invalid();

  void WaitUntil();
  void WaitUntil_valid();
  void WaitUntil_invalid();

  void WaitUntil_std();
  void WaitUntil_std_valid();
  void WaitUntil_std_invalid();
};

#endif  // KTUTILS_TEST_GLOBAL_HPP
