
QT += testlib
QT -= gui
CONFIG += testcase
TEMPLATE = app
TARGET = persistence
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

# Input
HEADERS += log/logging.h
HEADERS += persistence/UsersDataCache.h
SOURCES += log/logging.cpp
SOURCES += persistence/UsersDataCache.cpp
SOURCES += tst_UsersDataCache.cpp
