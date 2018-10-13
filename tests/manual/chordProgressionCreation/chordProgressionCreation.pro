QT += core gui widgets network

CONFIG += testcase
TEMPLATE = app
TARGET = chordsProgressionCreation
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
INCLUDEPATH += ../../../src/Common/gui
INCLUDEPATH += ../../../src/Common/gui/chat
VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += gui/chords/ChordProgressionCreationDialog.h


SOURCES += log/logging.cpp

SOURCES += chordProgressionCreation.cpp
SOURCES += gui/chords/ChordProgressionCreationDialog.cpp

FORMS += gui/chords/ChordProgressionCreationDialog.ui


