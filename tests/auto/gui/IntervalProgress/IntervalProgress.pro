QT += core gui widgets
TEMPLATE = app
TARGET = testIntervalProgress

INCLUDEPATH += .
INCLUDEPATH += ../../../../src/Common
VPATH += ../../../../src/Common

HEADERS += gui/IntervalProgressDisplay.h
HEADERS += TestMainWindow.h

SOURCES += gui/IntervalProgressDisplay.cpp
SOURCES += TestMainWindow.cpp

SOURCES += test_IntervalProgress.cpp

