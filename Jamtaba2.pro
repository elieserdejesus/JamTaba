# - mixer de streams de audio
# - Abrir plugins VST

#BUGS relacionados com o diálogo de audioIO:
#1 - quando seleciono as entradas sPDIF da fast track a aplicação encerra
#2 - quando seleciono saídas que não são 1 e 2 dá um crash também.
#3 - Com a fast track quando seleciono as entradas e simplesmente volto para a tela de audio IO o valor do segundo combo está bugado.
#4 - preciso testar com a fonte da fast track para ver se os outros canais estão realmente funcionando



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
    src/jamtaba/audio/AudioDriver.h \
    src/jamtaba/audio/AudioDriverListener.h \
    src/jamtaba/audio/PortAudioDriver.h \
    src/jamtaba/gui/mainframe.h \
    src/jamtaba/maincontroller.h \
    src/jamtaba/gui/audioiodialog.h \
    src/jamtaba/ConfigStore.h \
    src/jamtaba/audio/AudioNode.h \
    src/jamtaba/audio/AudioMixer.h \
    src/jamtaba/gui/widgets/PeakMeter.h \
    src/jamtaba/gui/trackview.h


win32:HEADERS += portaudio/include/pa_asio.h

SOURCES += \
    $$MAIN \
    src/jamtaba/audio/PortAudioDriver.cpp \
    src/jamtaba/gui/mainframe.cpp \
    src/jamtaba/maincontroller.cpp \
    src/jamtaba/gui/audioiodialog.cpp \
    src/jamtaba/audio/AudioDriver.cpp \
    src/jamtaba/ConfigStore.cpp \
    src/jamtaba/audio/AudioNode.cpp \
    src/jamtaba/audio/AudioMixer.cpp \
    src/jamtaba/gui/widgets/PeakMeter.cpp \
    src/jamtaba/gui/trackview.cpp




FORMS += src/jamtaba/gui/mainframe.ui \
    src/jamtaba/gui/audioiodialog.ui \
    src/jamtaba/gui/trackview.ui

LIBPATH += C:/Qt/Qt5.4.0/Tools/mingw491_32/i686-w64-mingw32/lib/ \
           $$PWD/portaudio/lib/


LIBS += -lportaudio \
        -lwinmm \
        -lole32 \


PRE_TARGETDEPS += $$PWD/portaudio/lib/libportaudio.a

INCLUDEPATH += $$PWD/portaudio/include \
              src/jamtaba/gui \
              src/jamtaba/gui/widgets

DEPENDPATH += $$PWD/portaudio/include

RESOURCES += \
    src/jamtaba/resources/jamtaba.qrc



