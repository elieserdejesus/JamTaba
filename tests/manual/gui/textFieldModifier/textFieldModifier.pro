QT += core gui widgets
CONFIG += testcase
TEMPLATE = app

TARGET = testTextFieldModifier

INCLUDEPATH += .
INCLUDEPATH += ../../../../src/Common
INCLUDEPATH += ../../../../src/Plugins
INCLUDEPATH += ../../../../src/Common/gui
INCLUDEPATH += ../../../../src/Common/gui/chat

VPATH += ../../../../src/Common
VPATH += ../../../../src/Plugins

HEADERS += log/logging.h

SOURCES += log/logging.cpp
SOURCES += gui/IconFactory.cpp
SOURCES += TopLevelTextEditorModifier.cpp

SOURCES += test_TextFieldModifier.cpp

RESOURCES += resources.qrc
