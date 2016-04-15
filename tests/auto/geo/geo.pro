
QT += testlib
QT -= gui
CONFIG += testcase
TEMPLATE = app
TARGET = geo
INCLUDEPATH += .
INCLUDEPATH += ../../../src/Common
VPATH += ../../../src/Common

HEADERS += log/logging.h
HEADERS += geo/IpToLocationResolver.h

SOURCES += log/logging.cpp
SOURCES += geo/IpToLocationResolver.cpp
SOURCES += tst_GeoLocation.cpp
