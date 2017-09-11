QT += core gui widgets

CONFIG += testcase
TEMPLATE = app
TARGET = testMap

INCLUDEPATH += .
INCLUDEPATH += ../../../../src/Common
INCLUDEPATH += ../../../../src/Common/gui

VPATH += ../../../../src/Common

SOURCES += test_Map.cpp

HEADERS += gui/widgets/MapWidget.h
HEADERS += gui/widgets/MapMarker.h

SOURCES += gui/widgets/MapWidget.cpp
SOURCES += gui/widgets/MapMarker.cpp

RESOURCES += resource.qrc



