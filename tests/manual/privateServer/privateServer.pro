QT += core gui widgets network
TEMPLATE = app
TARGET = testPrivateServer

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += gui/PrivateServerWindow.h
HEADERS += ninjam/server/Server.h

SOURCES += gui/PrivateServerWindow.cpp

SOURCES += ninjam/server/Server.cpp
SOURCES += ninjam/Ninjam.cpp
SOURCES += ninjam/client/ClientMessages.cpp
SOURCES += ninjam/client/ServerMessages.cpp
SOURCES += ninjam/client/User.cpp
SOURCES += ninjam/client/UserChannel.cpp

FORMS += gui/PrivateServerWindow.ui


SOURCES += test_PrivateServer.cpp

