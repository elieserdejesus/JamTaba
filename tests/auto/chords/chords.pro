
#TODO create a test-common.pri to share common tests configuration

QT += testlib core network
QT -= gui
CONFIG += testcase c++11
TEMPLATE = app
TARGET = testChords
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common

VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += TestChatChordsProgressionParser.h
HEADERS += gui/chat/NinjamVotingMessageParser.h

SOURCES += log/logging.cpp
SOURCES += gui/chords/ChatChordsProgressionParser.cpp
SOURCES += gui/chords/Chord.cpp
SOURCES += gui/chords/ChordProgressionMeasure.cpp
SOURCES += gui/chords/ChordProgression.cpp
SOURCES += gui/BpiUtils.cpp
SOURCES += TestChatChordsProgressionParser.cpp
SOURCES += gui/chat/NinjamVotingMessageParser.cpp

SOURCES += test_Chords.cpp
