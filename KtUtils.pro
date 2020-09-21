include(KtUtilsconf.pri)

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src

contains(KtUtils_CONFIG, KtUtils_Tests) {
  SUBDIRS += test
}
