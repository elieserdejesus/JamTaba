QT += core gui widgets network
TEMPLATE = app
TARGET = testPrivateServer

ROOT_PATH = "../../.."

INCLUDEPATH += .
INCLUDEPATH += $$ROOT_PATH/src/Common
INCLUDEPATH += $$ROOT_PATH/libs/includes/miniupnp

DEFINES += MINIUPNP_STATICLIB

VPATH += $$ROOT_PATH/src/Common

HEADERS += gui/PrivateServerWindow.h
HEADERS += ninjam/server/Server.h
HEADERS += upnp/UPnPManager.h

SOURCES += gui/PrivateServerWindow.cpp

SOURCES += ninjam/server/Server.cpp
SOURCES += ninjam/Ninjam.cpp
SOURCES += ninjam/client/ClientMessages.cpp
SOURCES += ninjam/client/ServerMessages.cpp
SOURCES += ninjam/client/User.cpp
SOURCES += ninjam/client/UserChannel.cpp

SOURCES += upnp/UPnPManager.cpp

FORMS += gui/PrivateServerWindow.ui


SOURCES += test_PrivateServer.cpp

CONFIG(release, debug|release): LIBS += -L$$PWD/$$ROOT_PATH/libs/static/win64-msvc -lminiupnpc
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/$$ROOT_PATH/libs/static/win64-msvc -lminiupnpcd

LIBS += -lIPHlpApi # used by miniupnp lib

