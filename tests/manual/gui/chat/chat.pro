QT += core gui widgets network
CONFIG += testcase
TEMPLATE = app
TARGET = testChat
INCLUDEPATH += .
INCLUDEPATH += ../../../../src/Common
INCLUDEPATH += ../../../../src/Common/gui
VPATH += ../../../../src/Common

HEADERS += log/logging.h
HEADERS += gui/ChatMessagePanel.h
HEADERS += gui/ChatPanel.h
HEADERS += gui/UsersColorsPool.h

SOURCES += log/logging.cpp
SOURCES += gui/ChatMessagePanel.cpp
SOURCES += gui/ChatPanel.cpp
SOURCES += gui/UsersColorsPool.cpp

SOURCES += test_Chat.cpp

FORMS += gui/ChatMessagePanel.ui
FORMS += gui/ChatPanel.ui

RESOURCES += resources.qrc
