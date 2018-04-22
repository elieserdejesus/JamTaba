QT += core gui widgets

CONFIG += testcase
TEMPLATE = app
TARGET = testInstrumentsMenu

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
INCLUDEPATH += ../../../src/Common/gui

VPATH += ../../../src/Common

SOURCES += log/logging.cpp

HEADERS += gui/widgets/InstrumentsMenu.h
SOURCES += gui/widgets/InstrumentsMenu.cpp

SOURCES += test_InstrumentsMenu.cpp

RESOURCES += ../../../src/resources/jamtaba.qrc

