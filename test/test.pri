include($$PWD/../KtUtilsconf.pri)

QT += core gui testlib
CONFIG += testcase

win32: {
  contains(KtUtils_CONFIG, KtUtils_Shared_Library) {
    LIBS += -L$$PWD/../bin/
  } else {
    LIBS += -L$$PWD/../lib/
  }
} else:unix: {
  LIBS += -L$$PWD/../lib/
}
CONFIG(release, debug|release): LIBS += -lKtUtils
else:CONFIG(debug, debug|release): LIBS += -lKtUtilsd

DESTDIR = $$PWD/../bin

INCLUDEPATH += $$PWD/../include
