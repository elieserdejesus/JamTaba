QT += core
QT -= gui

TARGET = AUScanner
CONFIG -= app_bundle #create just a binary, not a complete bundle
CONFIG += c++11


#when debugging the AUScanner executable is generated in the Standalone folder
CONFIG(debug, debug|release){
    DESTDIR = $$OUT_PWD/../Standalone/Jamtaba2.app/Contents/MacOS
    DESTDIR = $$OUT_PWD/../Standalone/debug
    message("Generating AUScanner executable in" $$DESTDIR)
}

TEMPLATE = app

ROOT_PATH = "../.."
SOURCE_PATH = $$ROOT_PATH/src

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/AUScanner
INCLUDEPATH += $$SOURCE_PATH/../AU_SDK/PublicUtility
INCLUDEPATH += $$SOURCE_PATH/../AU_SDK/AUPlublic/AUBase
INCLUDEPATH += $$SOURCE_PATH/../AU_SDK/AUPlublic/OtherBases
INCLUDEPATH += $$SOURCE_PATH/../AU_SDK/AUPlublic/Utility

VPATH       += $$SOURCE_PATH/Common
VPATH       += $$SOURCE_PATH/AUScanner

HEADERS += AUPluginScanner.h

SOURCES += main.cpp
SOURCES += audio/core/PluginDescriptor.cpp
SOURCES += log/logging.cpp
SOURCES += AUPluginScanner.cpp

QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

LIBS += -framework AudioToolBox

#mac osx doc icon
ICON = Jamtaba.icns
CONFIG += console
