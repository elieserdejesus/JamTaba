QT += core gui widgets

CONFIG += testcase
TEMPLATE = app
TARGET = testPluginScanDialog

INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
INCLUDEPATH += ../../../src/Common/gui

VPATH += ../../../src/Common

SOURCES += test_pluginScanDialog.cpp

HEADERS += gui/PluginScanDialog.h

SOURCES += gui/PluginScanDialog.cpp

FORMS += gui/PluginScanDialog.ui

