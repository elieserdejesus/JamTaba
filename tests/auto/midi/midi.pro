QT += testlib
QT -= gui
CONFIG += testcase
TEMPLATE = app
TARGET = midi

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += midi/MidiMessage.h
SOURCES += midi/MidiMessage.cpp

SOURCES += test_MidiMessage.cpp
