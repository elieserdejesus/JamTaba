
#TODO create a test-common.pri to share common tests configuration

QT += testlib core widgets network

CONFIG += testcase c++11
TEMPLATE = app
TARGET = testEmoji
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common

VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += TestEmojiParser.h
SOURCES += gui/chat/EmojiManager.h

SOURCES += log/logging.cpp
SOURCES += TestEmojiParser.cpp
SOURCES += gui/chat/EmojiManager.cpp

SOURCES += test_Emoji.cpp
