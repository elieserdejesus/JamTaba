QT += core gui widgets

CONFIG += testcase
TEMPLATE = app
TARGET = testMultiStateButton

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common/gui

VPATH += ../../../src/Common

SOURCES += teste_multiStateButton.cpp

HEADERS += gui/widgets/MultiStateButton.h

SOURCES += gui/widgets/MultiStateButton.cpp

