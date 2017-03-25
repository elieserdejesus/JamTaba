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
HEADERS += looper/Looper.h

SOURCES += TestSamplesBuffer.cpp
SOURCES += TestLooper.cpp
SOURCES += audio/core/SamplesBuffer.cpp
SOURCES += audio/core/AudioPeak.cpp
SOURCES += looper/Looper.cpp
SOURCES += looper/LooperStates.cpp
SOURCES += looper/LooperLayer.cpp

SOURCES += test_Audio.cpp
