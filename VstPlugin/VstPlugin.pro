#CONFIG += qtwinmigrate-uselib
include(../qtwinmigrate/src/qtwinmigrate.pri)

QT += core widgets network

TARGET = "JamtabaVST"
TEMPLATE = lib
DEFINES += VST_FORCE_DEPRECATED=0
CONFIG += shared

win32 {
    LIBS += -lwinmm
    LIBS += -ladvapi32
    LIBS += -lws2_32
    LIBS += -lQt5PlatformSupport
    LIBS += -L$(QTDIR)\plugins\platforms\ -lqwindows
    #LIBS += -limm32
}

win32-msvc* {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    #RC_FILE = vstdll.rc
}

SOURCES += \
    audioeffectx.cpp \
    audioeffect.cpp \
    main.cpp \
    vst.cpp \


HEADERS  += \
    vst.h \


win32 {
    message("Windows VST build")
    INCLUDEPATH += "E:/Jamtaba2\VST3 SDK/"
    INCLUDEPATH += "E:\Jamtaba2\VST3 SDK\public.sdk\source\vst2.x"

}



