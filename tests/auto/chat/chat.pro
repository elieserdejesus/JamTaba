
#TODO create a test-common.pri to share common tests configuration

QT += testlib core widgets network

CONFIG += testcase c++11
TEMPLATE = app
TARGET = testChat
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common

VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += TestChatMessages.h
HEADERS += TestChatVotingMessages.h
HEADERS += gui/chat/NinjamChatMessageParser.h

SOURCES += log/logging.cpp
SOURCES += TestChatMessages.cpp
SOURCES += gui/chords/ChatChordsProgressionParser.cpp
SOURCES += gui/chords/Chord.cpp
SOURCES += gui/chords/ChordProgressionMeasure.cpp
SOURCES += gui/chords/ChordProgression.cpp
SOURCES += gui/BpiUtils.cpp
SOURCES += TestChatVotingMessages.cpp
SOURCES += gui/chat/NinjamChatMessageParser.cpp

SOURCES += test_Chat.cpp
