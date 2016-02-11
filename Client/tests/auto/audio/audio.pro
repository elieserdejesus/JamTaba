QT += testlib
QT -= gui
CONFIG += testcase
TEMPLATE = app
TARGET = audio

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += audio/core/SamplesBuffer.h
SOURCES += audio/core/SamplesBuffer.cpp

HEADERS += audio/core/AudioPeak.h
SOURCES += audio/core/AudioPeak.cpp

SOURCES += test_Audio.cpp
