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

QMAKE_MACOSX_DEPLOYMENT_TARGET=10.9

QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-parameter
macx:QMAKE_CXXFLAGS += -stdlib=libc++

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Jamtaba2
TEMPLATE = app

MAIN = src/jamtaba/ninjam/main.cpp #src/jamtaba/main.cpp


HEADERS += \
    #nvwa/debug_new.h \
#    src/jamtaba/audio/AudioDriver.h \
#    src/jamtaba/audio/AudioDriverListener.h \
#    src/jamtaba/audio/AudioNode.h \
#    src/jamtaba/audio/AudioMixer.h \
#    src/jamtaba/audio/PortAudioDriver.h \
#    src/jamtaba/gui/mainframe.h \
#    src/jamtaba/gui/audioiodialog.h \
#    src/jamtaba/gui/widgets/PeakMeter.h \
#    src/jamtaba/gui/trackview.h \
#    src/jamtaba/ConfigStore.h \
#    src/jamtaba/network/loginserver/LoginService.h \
#    src/jamtaba/network/loginserver/DefaultLoginService.h \
#    src/jamtaba/JamtabaFactory.h \
#    src/jamtaba/network/loginserver/LoginServiceResponse.h \
#    src/jamtaba/model/JamRoomRepository.h \
#    src/jamtaba/model/Peer.h \
#    src/jamtaba/model/JamRoom.h \
#    src/jamtaba/network/loginserver/JsonUtils.h \
#    src/jamtaba/MainController.h \
    src/jamtaba/ninjam/NinjamService.h \
    src/jamtaba/ninjam/NinjamServer.h \
    src/jamtaba/ninjam/NinjamUser.h \
    src/jamtaba/ninjam/protocol/ServerMessageParser.h \
    src/jamtaba/ninjam/protocol/ServerMessages.h \
    src/jamtaba/ninjam/protocol/ClientMessages.h \
    src/jamtaba/ninjam/NinjamPublicServersParser.h \

#win32:HEADERS += portaudio/include/pa_asio.h

SOURCES += \
    $$MAIN \
#    nvwa/debug_new.cpp \
#    src/jamtaba/ConfigStore.cpp \
#    src/jamtaba/audio/AudioDriver.cpp \
#    src/jamtaba/audio/AudioNode.cpp \
#    src/jamtaba/audio/AudioMixer.cpp \
#    src/jamtaba/audio/PortAudioDriver.cpp \
#    src/jamtaba/gui/mainframe.cpp \
#    src/jamtaba/gui/audioiodialog.cpp \
#    src/jamtaba/gui/widgets/PeakMeter.cpp \
#    src/jamtaba/gui/trackview.cpp \
#    src/jamtaba/network/loginserver/LoginService.cpp \
#    src/jamtaba/network/loginserver/DefaultLoginService.cpp \
#    src/jamtaba/JamtabaFactory.cpp \
#    src/jamtaba/network/loginserver/LoginServiceResponse.cpp \
#    src/jamtaba/model/Peer.cpp \
#    src/jamtaba/model/JamRoomRepository.cpp \
#    src/jamtaba/network/loginserver/JsonUtils.cpp \
#    src/jamtaba/MainController.cpp \
#    src/jamtaba/model/JamRoom.cpp \
    src/jamtaba/ninjam/NinjamService.cpp \
    src/jamtaba/ninjam/NinjamServer.cpp \
    src/jamtaba/ninjam/NinjamUser.cpp \
    src/jamtaba/ninjam/protocol/ServerMessages.cpp \
    src/jamtaba/ninjam/protocol/ClientMessages.cpp \
    src/jamtaba/ninjam/protocol/ServerMessageParser.cpp \


#FORMS += src/jamtaba/gui/mainframe.ui \
#    src/jamtaba/gui/audioiodialog.ui \
#    src/jamtaba/gui/trackview.ui

#macx: LIBPATH += /Users/Eliesr/Qt5.4.0/5.4/clang_64/lib \
#win32:LIBPATH += C:/Qt/Qt5.4.0/Tools/mingw491_32/i686-w64-mingw32/lib/ \
#win32:LIBPATH += $$PWD/portaudio/lib/


#win32:LIBS += -lportaudio \
#        -lwinmm \
#        -lole32 \


#win32:PRE_TARGETDEPS += $$PWD/portaudio/lib/libportaudio.a

#INCLUDEPATH += $$PWD/portaudio/include \
#              src/jamtaba/gui \
#              src/jamtaba/gui/widgets

#DEPENDPATH += $$PWD/portaudio/include

#RESOURCES += \
#    src/jamtaba/resources/jamtaba.qrc



