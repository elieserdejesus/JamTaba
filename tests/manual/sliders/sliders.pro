QT += core gui widgets
TEMPLATE = app
TARGET = testSliders

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += gui/widgets/Slider.h

SOURCES += gui/widgets/Slider.cpp

SOURCES += test_Sliders.cpp
