QT += testlib
QT -= gui
CONFIG += testcase
CONFIG += c++11
TEMPLATE = app
TARGET = audio

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += TestSamplesBuffer.h
HEADERS += TestLooper.h
HEADERS += audio/core/SamplesBuffer.h
HEADERS += audio/core/AudioPeak.h
HEADERS += audio/looper/AudioLooper.h

SOURCES += TestSamplesBuffer.cpp
SOURCES += TestLooper.cpp
SOURCES += audio/core/SamplesBuffer.cpp
SOURCES += audio/core/AudioPeak.cpp
SOURCES += audio/looper/AudioLooper.cpp
SOURCES += audio/looper/AudioLooperStates.cpp
SOURCES += audio/looper/AudioLooperLayer.cpp

SOURCES += test_Audio.cpp
