QT += core network gui

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

TARGET = TestVideoXmit

INCLUDEPATH += "../../../libs/includes/vorbis"
INCLUDEPATH += "../../../libs/includes/ogg"
INCLUDEPATH += ../../../src/Common


VPATH += ../../../src/Common
VPATH += ../../../src/Common/ninjam

LIBS += -L"$$PWD/../../../libs/static/win64-msvc/" -logg -lvorbis

SOURCES += main.cpp

SOURCES += ninjam/Service.cpp
SOURCES += ninjam/Server.cpp
SOURCES += ninjam/UserChannel.cpp
SOURCES += ninjam/User.cpp
SOURCES += ninjam/ServerMessages.cpp
SOURCES += ninjam/ClientMessages.cpp
SOURCES += ninjam/ServerMessagesHandler.cpp

SOURCES += audio/core/SamplesBuffer.cpp
SOURCES += audio/core/AudioPeak.cpp
SOURCES += audio/vorbis/VorbisEncoder.cpp

SOURCES += log/Logging.cpp

HEADERS += ninjam/Service.h

RESOURCES += resources.qrc

