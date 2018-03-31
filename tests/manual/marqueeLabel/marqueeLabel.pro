QT += core gui widgets

CONFIG += testcase
TEMPLATE = app
TARGET = testMarquee

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common/gui

VPATH += ../../../src/Common

SOURCES += test_Marquee.cpp

HEADERS += gui/widgets/MarqueeLabel.h

SOURCES += gui/widgets/MarqueeLabel.cpp

