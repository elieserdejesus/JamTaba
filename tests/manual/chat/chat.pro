QT += core gui widgets network

CONFIG += testcase
TEMPLATE = app
TARGET = testChat
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
INCLUDEPATH += ../../../src/Common/gui
INCLUDEPATH += ../../../src/Common/gui/chat
VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += gui/chat/ChatMessagePanel.h
HEADERS += gui/chat/ChatPanel.h
HEADERS += gui/chat/EmojiWidget.h
HEADERS += gui/chat/ChatTextEditor.h
HEADERS += gui/UsersColorsPool.h

SOURCES += log/logging.cpp
SOURCES += gui/chat/ChatMessagePanel.cpp
SOURCES += gui/chat/ChatPanel.cpp
SOURCES += gui/chat/ChatTextEditor.cpp
SOURCES += gui/chat/EmojiWidget.cpp
SOURCES += gui/chat/EmojiManager.cpp
SOURCES += gui/IconFactory.cpp
SOURCES += gui/UsersColorsPool.cpp

SOURCES += test_Chat.cpp

FORMS += gui/chat/ChatMessagePanel.ui
FORMS += gui/chat/ChatPanel.ui

RESOURCES += resources.qrc
