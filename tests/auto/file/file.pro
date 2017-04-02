
QT += testlib
QT -= gui
CONFIG += testcase
TEMPLATE = app
TARGET = testFile
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += file/FileUtils.h

SOURCES += file/FileUtils.cpp
SOURCES += test_File.cpp
