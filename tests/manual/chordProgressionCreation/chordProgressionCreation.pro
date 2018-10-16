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
HEADERS += gui/chords/ChordProgression.h
HEADERS += gui/chords/ChordProgressionMeasure.h
HEADERS += gui/chords/Chord.h
HEADERS += gui/BpiUtils.h
HEADERS += gui/GuiUtils.h

SOURCES += log/logging.cpp

SOURCES += chordProgressionCreation.cpp
SOURCES += gui/chords/ChordProgression.cpp
SOURCES += gui/chords/ChordProgressionMeasure.cpp
SOURCES += gui/chords/Chord.cpp
SOURCES += gui/chords/ChordProgressionCreationDialog.cpp
SOURCES += gui/BpiUtils.cpp
SOURCES += gui/GuiUtils.cpp

FORMS += gui/chords/ChordProgressionCreationDialog.ui


