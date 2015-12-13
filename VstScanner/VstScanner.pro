QT += core widgets
QT -= gui

TARGET = VstScanner
CONFIG += console
CONFIG -= app_bundle #in MAC create just a binary, not a complete bundle

DEFINES += VST_FORCE_DEPRECATED=0 #enable VST 2.3 features

TEMPLATE = app

HEADERS += ../src/audio/vst/VstHost.h \
    VstPluginScanner.h

SOURCES += \
    vstScannerMain.cpp \
    ../src/audio/vst/VstHost.cpp \
    ../src/audio/core/PluginDescriptor.cpp \
    ../src/audio/vst/VstLoader.cpp \
    ../src/log/logging.cpp \
    VstPluginScanner.cpp \

win32{
    SOURCES += WindowsVstPluginScanner.cpp
}
macx{
   SOURCES += MacVstPluginScanner.cpp
}

win32{

    win32-msvc*{#all msvc compilers
        #windows XP support
        QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

        CONFIG(release, debug|release) {
            #ltcg - http://blogs.msdn.com/b/vcblog/archive/2009/02/24/quick-tips-on-using-whole-program-optimization.aspx
            QMAKE_CXXFLAGS_RELEASE +=  -GL -Gy -Gw
            QMAKE_LFLAGS_RELEASE += /LTCG
        }
    }

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32
    RC_FILE = ../Jamtaba2.rc #windows icon
}


macx{
    message("VstScanner Mac build")
    #mac osx doc icon
    ICON = Jamtaba.icns
}

INCLUDEPATH += ../../VST_SDK/pluginterfaces/vst2.x/ \
               ../src/
