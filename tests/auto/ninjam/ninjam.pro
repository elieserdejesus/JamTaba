
#TODO create a test-common.pri to share common tests configuration

QT += testlib core network
QT -= gui
CONFIG += testcase c++11
TEMPLATE = app
TARGET = testNinjam
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += TestMessagesSerialization.h
HEADERS += TestServerMessagesHandler.h
HEADERS += TestServerClientCommunication.h

HEADERS += log/logging.h
HEADERS += TestServerInfo.h
HEADERS += ninjam/client/ServerInfo.h
HEADERS += ninjam/client/User.h
HEADERS += ninjam/client/UserChannel.h
HEADERS += ninjam/client/Service.h
HEADERS += ninjam/Ninjam.h
HEADERS += ninjam/server/Server.h

SOURCES += log/logging.cpp
SOURCES += ninjam/Ninjam.cpp
SOURCES += TestServerInfo.cpp
SOURCES += ninjam/client/ServerInfo.cpp
SOURCES += ninjam/client/User.cpp
SOURCES += ninjam/client/UserChannel.cpp
SOURCES += ninjam/client/Service.cpp
SOURCES += ninjam/client/ServerMessages.cpp
SOURCES += ninjam/client/ServerMessagesHandler.cpp
SOURCES += ninjam/client/ClientMessages.cpp
SOURCES += ninjam/server/Server.cpp

SOURCES += TestServerMessagesHandler.cpp
SOURCES += TestMessagesSerialization.cpp
SOURCES += TestServerClientCommunication.cpp

SOURCES += test_Ninjam.cpp

RESOURCES += ninjamTestsResources.qrc
