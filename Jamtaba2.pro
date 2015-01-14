#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:05:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Jamtaba2
TEMPLATE = app

MAIN = src/jamtaba/main.cpp #src/jamtaba/gui/testAudioIoDialog.cpp

HEADERS += \
    src/jamtaba/audio/AbstractAudioDriver.h \
    src/jamtaba/audio/AudioDriver.h \
    src/jamtaba/audio/AudioDriverListener.h \
    src/jamtaba/audio/PortAudioDriver.h \
    src/jamtaba/gui/mainframe.h \
    src/jamtaba/maincontroller.h \
    src/jamtaba/gui/audioiodialog.h

win32:HEADERS += portaudio/include/pa_asio.h

SOURCES += \
    $$MAIN \
    src/jamtaba/audio/AbstractAudioDriver.cpp \
    src/jamtaba/audio/PortAudioDriver.cpp \
    src/jamtaba/gui/mainframe.cpp \
    src/jamtaba/maincontroller.cpp \
    src/jamtaba/gui/audioiodialog.cpp \


FORMS += src/jamtaba/gui/mainframe.ui \
    src/jamtaba/gui/audioiodialog.ui

LIBPATH += C:/Qt/Qt5.4.0/Tools/mingw491_32/i686-w64-mingw32/lib/ \
           $$PWD/portaudio/lib/


LIBS += -lportaudio \
        -lwinmm \
        -lole32 \


PRE_TARGETDEPS += $$PWD/portaudio/lib/libportaudio.a

INCLUDEPATH += $$PWD/portaudio/include

DEPENDPATH += $$PWD/portaudio/include


