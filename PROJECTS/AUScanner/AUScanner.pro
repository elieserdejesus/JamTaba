QT += core
QT -= gui

TARGET = AUScanner
CONFIG -= app_bundle #in MAC create just a binary, not a complete bundle

#when debugging the AUScanner executable is generated in the Standalone folder
CONFIG(debug, debug|release){
    macx: DESTDIR = $$OUT_PWD/../Standalone/Jamtaba2.app/Contents/MacOS
    !macx:DESTDIR = $$OUT_PWD/../Standalone/debug
    message("Generating AUScanner executable in" $$DESTDIR)
}

TEMPLATE = app

LIBS += -framework CoreAudio
LIBS += -framework AudioToolbox
LIBS += -framework AudioUnit
LIBS += -framework CoreServices

QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
LIBS += -mmacosx-version-min=10.7 -stdlib=libc++

ROOT_PATH = "../.."
SOURCE_PATH = $$ROOT_PATH/src

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/Scanners

VPATH       += $$SOURCE_PATH/Common
VPATH       += $$SOURCE_PATH/Scanners


HEADERS += BaseScanner.h
HEADERS += AUScanner/AUPluginScanner.h

SOURCES += AUScanner/main.cpp
SOURCES += audio/core/PluginDescriptor.cpp
SOURCES += log/logging.cpp
SOURCES += BaseScanner.cpp
SOURCES += AUScanner/AUPluginScanner.cpp

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
LIBS+= -dead_strip

#mac osx doc icon
ICON = Jamtaba.icns
CONFIG += console

