
#TODO create a test-common.pri to share common tests configuration

QT += testlib core network
QT -= gui
CONFIG += testcase c++11
TEMPLATE = app
TARGET = testNinjam
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += ninjam/client/Server.h
HEADERS += ninjam/client/User.h
HEADERS += ninjam/client/UserChannel.h
HEADERS += ninjam/client/Service.h

HEADERS += TestServerMessagesHandler.h
HEADERS += TestServerMessages.h
HEADERS += TestServer.h

SOURCES += log/logging.cpp
SOURCES += ninjam/client/Server.cpp
SOURCES += ninjam/client/User.cpp
SOURCES += ninjam/client/UserChannel.cpp
SOURCES += ninjam/client/Service.cpp
SOURCES += ninjam/client/ServerMessages.cpp
SOURCES += ninjam/client/ServerMessagesHandler.cpp
SOURCES += ninjam/client/ClientMessages.cpp

SOURCES += TestServerMessages.cpp
SOURCES += TestServer.cpp
SOURCES += TestServerMessagesHandler.cpp

SOURCES += test_Ninjam.cpp

RESOURCES += ninjamTestsResources.qrc
