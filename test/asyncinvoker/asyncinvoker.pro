CONFIG(release, debug|release): TARGET = TestAsyncInvoker
else:CONFIG(debug, debug|release): TARGET = TestAsyncInvokerd

include(../test.pri)

HEADERS += testasyncinvoker.h

SOURCES += testasyncinvoker.cpp
