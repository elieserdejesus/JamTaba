QT += core gui widgets
TEMPLATE = app
TARGET = testPeakMeters

INCLUDEPATH += .
INCLUDEPATH += ../../../../src/Common
VPATH += ../../../../src/Common

HEADERS += gui/widgets/PeakMeter.h

SOURCES += gui/widgets/PeakMeter.cpp

SOURCES += test_PeakMeters.cpp
