QT += core

TARGET = "PluginTester"
TEMPLATE = app

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

SOURCES += \
    src/mainPluginTester.cpp

INCLUDEPATH += "E:/Jamtaba2/VST3 SDK/"
INCLUDEPATH += "E:/Jamtaba2/VST3 SDK/pluginterfaces/vst2.x/"
INCLUDEPATH += "E:/Jamtaba2/VST3 SDK/public.sdk/source/vst2.x"
