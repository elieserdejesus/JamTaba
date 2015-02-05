# - mostrar salas ninjam
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

QT       += network

QMAKE_CXXFLAGS += -std=c++11 #-Wno-unused-parameter

#MAC
macx:QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_MACOSX_DEPLOYMENT_TARGET=10.6

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Jamtaba2
TEMPLATE = app

MAIN = src/jamtaba/main.cpp
#MAIN = src/jamtaba/ninjam/main.cpp


HEADERS += \
    #nvwa/debug_new.h \
    src/jamtaba/persistence/ConfigStore.h \
    #--------------------------------
    src/jamtaba/audio/AudioDriver.h \
    src/jamtaba/audio/AudioNode.h \
    src/jamtaba/audio/AudioMixer.h \
    src/jamtaba/audio/PortAudioDriver.h \
    #--------------------------------
    src/jamtaba/gui/mainframe.h \
    src/jamtaba/gui/audioiodialog.h \
    src/jamtaba/gui/widgets/PeakMeter.h \
    src/jamtaba/gui/trackview.h \
    src/jamtaba/gui/jamroomviewpanel.h \
    #--------------------------------
    src/jamtaba/loginserver/LoginService.h \
    src/jamtaba/loginserver/JamRoom.h \
    src/jamtaba/loginserver/JsonUtils.h \
    #--------------------------------
    src/jamtaba/MainController.h \
    src/jamtaba/JamtabaFactory.h \
    #--------------------------------
    src/jamtaba/ninjam/protocol/ServerMessageParser.h \
    src/jamtaba/ninjam/protocol/ServerMessages.h \
    src/jamtaba/ninjam/protocol/ClientMessages.h \
    src/jamtaba/ninjam/User.h \
    src/jamtaba/ninjam/Service.h \
    src/jamtaba/ninjam/Server.h \
    #--------------------------------
    src/jamtaba/loginserver/natmap.h


win32:HEADERS += portaudio/include/pa_asio.h

SOURCES += \
    $$MAIN \
#    nvwa/debug_new.cpp \
    src/jamtaba/persistence/ConfigStore.cpp \
#------------------------------------------------
    src/jamtaba/audio/AudioDriver.cpp \
    src/jamtaba/audio/AudioNode.cpp \
    src/jamtaba/audio/AudioMixer.cpp \
    src/jamtaba/audio/PortAudioDriver.cpp \
#------------------------------------------------
    src/jamtaba/gui/mainframe.cpp \
    src/jamtaba/gui/audioiodialog.cpp \
    src/jamtaba/gui/widgets/PeakMeter.cpp \
    src/jamtaba/gui/trackview.cpp \
    src/jamtaba/gui/jamroomviewpanel.cpp \
#------------------------------------------------
    src/jamtaba/JamtabaFactory.cpp \
    src/jamtaba/MainController.cpp \
#------------------------------------------------
    src/jamtaba/loginserver/LoginService.cpp \
    src/jamtaba/loginserver/JsonUtils.cpp \
    src/jamtaba/loginserver/JamRoom.cpp \
#------------------------------------------------
    src/jamtaba/ninjam/protocol/ServerMessages.cpp \
    src/jamtaba/ninjam/protocol/ClientMessages.cpp \
    src/jamtaba/ninjam/protocol/ServerMessageParser.cpp \
    src/jamtaba/ninjam/Server.cpp \
    src/jamtaba/ninjam/Service.cpp \
    src/jamtaba/ninjam/User.cpp \

FORMS += src/jamtaba/gui/mainframe.ui \
    src/jamtaba/gui/audioiodialog.ui \
    src/jamtaba/gui/trackview.ui \
    src/jamtaba/gui/jamroomviewpanel.ui

#macx: LIBPATH += /Users/Eliesr/Qt5.4.0/5.4/clang_64/lib \
win32:LIBPATH += C:/Qt/Qt5.4.0/Tools/mingw491_32/i686-w64-mingw32/lib/ \
win32:LIBPATH += $$PWD/portaudio/lib/


win32:LIBS += -lportaudio \
              -lwinmm \
              -lole32 \


win32:PRE_TARGETDEPS += $$PWD/portaudio/lib/libportaudio.a

INCLUDEPATH += $$PWD/portaudio/include \
               src/jamtaba/gui \
               src/jamtaba/gui/widgets

DEPENDPATH += $$PWD/portaudio/include

RESOURCES += src/jamtaba/resources/jamtaba.qrc
