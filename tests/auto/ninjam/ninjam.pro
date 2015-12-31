
#TODO create a test-common.pri to share common tests configuration

QT += testlib core network
QT -= gui
CONFIG += testcase
TEMPLATE = app
TARGET = testNinjam
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += ninjam/Server.h
HEADERS += ninjam/User.h
HEADERS += ninjam/UserChannel.h
HEADERS += ninjam/Service.h

HEADERS += TestServerMessagesHandler.h
HEADERS += TestServerMessages.h
HEADERS += TestServer.h

SOURCES += log/logging.cpp
SOURCES += ninjam/Server.cpp
SOURCES += ninjam/User.cpp
SOURCES += ninjam/UserChannel.cpp
SOURCES += ninjam/Service.cpp
SOURCES += ninjam/ServerMessages.cpp
SOURCES += ninjam/ServerMessagesHandler.cpp
SOURCES += ninjam/ClientMessages.cpp

SOURCES += TestServerMessages.cpp
SOURCES += TestServer.cpp
SOURCES += TestServerMessagesHandler.cpp

SOURCES += test_Ninjam.cpp

RESOURCES += \
    ninjamTestsResources.qrc
