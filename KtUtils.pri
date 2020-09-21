include($$PWD/KtUtilsconf.pri)

QT += core gui svg
CONFIG += c++11

INCLUDEPATH += $$PWD/include

HEADERS += \
  $$PWD/include/KtUtils/asyncinvoker.h \
  $$PWD/include/KtUtils/global.h \
  $$PWD/include/KtUtils/iconhelper.h

SOURCES += \
  $$PWD/src/asyncinvoker.cpp \
  $$PWD/src/global.cpp \
  $$PWD/src/iconhelper.cpp
