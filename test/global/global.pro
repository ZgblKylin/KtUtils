CONFIG(release, debug|release): TARGET = TestGlobal
else:CONFIG(debug, debug|release): TARGET = TestGlobald

include(../test.pri)

HEADERS += testglobal.h

SOURCES += testglobal.cpp
