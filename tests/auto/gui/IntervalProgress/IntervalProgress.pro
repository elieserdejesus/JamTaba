QT += core gui widgets
TEMPLATE = app
TARGET = testIntervalProgress

INCLUDEPATH += .
INCLUDEPATH += ../../../../src/Common
VPATH += ../../../../src/Common

HEADERS += gui/intervalProgress/IntervalProgressDisplay.h
HEADERS += TestMainWindow.h

SOURCES += gui/intervalProgress/IntervalProgressDisplay.cpp
SOURCES += gui/intervalProgress/LinearPaintStrategy.cpp
SOURCES += gui/intervalProgress/EllipticalPaintStrategy.cpp
SOURCES += gui/intervalProgress/CircularPaintStrategy.cpp
SOURCES += gui/intervalProgress/PiePaintStrategy.cpp
SOURCES += TestMainWindow.cpp

SOURCES += test_IntervalProgress.cpp

