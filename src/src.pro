include(../KtUtils.pri)

TEMPLATE = lib

contains(KtUtils_CONFIG, KtUtils_Shared_Library) {
DEFINES += KT_UTILS_SHARED_LIBRARY
  DEFINES += KT_UTILS_BUILD_SHARED_LIBRARY
  win32: DESTDIR = $$PWD/../bin
  else: DESTDIR = $$PWD/../lib
} else {
  CONFIG += static
  DESTDIR = $$PWD/../lib
}

CONFIG(release, debug|release): TARGET = KtUtils
else:CONFIG(debug, debug|release): TARGET = KtUtilsd

PRECOMPILED_HEADER += $$PWD/../include/KtUtils/global.h
