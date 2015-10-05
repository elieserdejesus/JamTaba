#esconder o painel de server privado quando exibir o painel de user name

#change log
#1 - little change in chat translate button
#2 - Using dial in pan and improvements in local controls ui
#3 - fix minor issues in ui

#stringer disse que o programa não abre. Instalar o JamKazam para ver.

#-------------------------------- PRIMEIRO RELEASE ----------------------------------------
# Deu problema no MIDI no windows em 64 bits. Precilei compilar a portmidi  como MT ao invés de MD, port isso está dando
    #o warning sobre a mistura de libs MT e MD. Será que eu poderia usar tudo MT entao?
    #isso me deu várias horas de dor de cabeça, achei essa solução praticamente na sorte.

#--------------
#importante para VST plugin no MAC com Qt. ou seja é possível fazer o plugin VST:
    #https://forum.qt.io/topic/13731/solved-qt-gui-in-vst-plug-in-clicking-menu-bar-crashes-the-host

#---------------------------------------------------------
#separar o carregamento do plugin VST da instância. No momento uma instância é criada e depois é que o plugin é carregado. Pra mim
#isso é um bad design
#---------------------------------------------------------
#3 - Preciso melhorar o resampling aplicando o low pass.
    #o low pass mais simples: https://ccrma.stanford.edu/~jos/fp/Definition_Simplest_Low_Pass.html

    #gera código para low pass: http://www-users.cs.york.ac.uk/~fisher/cgi-bin/mkfscript

    #That’s it—to double the sample rate, we insert a zero between each sample, and low-pass filter to clear the extended
    #part of the audio band. Any low-pass filter will do, as long as you pick one steep enough to get the job done,
    #removing the aliased copy without removing much of the existing signal band. Most often, a linear phase FIR filter is
    #used—performance is good at the relatively high cut-off frequency, phase is maintained, and we have good control over
    #its characteristics.

    #Downsampling

        #The process of reducing the sample rate—downsampling—is similar, except we low-pass filter first, to reduce the bandwidth,
        #then discard samples. The filter stage is essential, since the signal will alias if we try to fit it into a narrower
        #band without removing the portion that can’t be encoded at the lower rate. So, we set the filter cut-off to half the
        #new, lower sample rate, then simply discard every other sample for a 2:1 downsample ratio.
        #(Yes, the result will be slightly different depending on whether you discard the odd samples or even ones.
        #And no, it doesn’t matter, just as the exact sampling phase didn’t matter when you converted from analog to digital
        #in the first place.)

        #In general, the algorithm for resampling to a higher frequency is:
        #* maintain a 'cursor': a floating-point sample index, into the source sample
        #* for each sample, advance the cursor by (sourceSampleRate/targetSampleRate), which will be < 1.0
        #* interpolate data from the source sample according to the cursor position using the interpolation method of your choice; this will generally involve a polynomial using a small number of source samples around the cursor
        #For resampling to a lower frequency, the process is similar but the source sample should be lowpass filtered to attenuate everything above half the target sample rate before interpolation.

#++++++++++++++++++++++++++++++++++++++++++++++

#quando solo uma das inputs as outras também são enviadas. Ou seja, o solo está atuando apenas localmente. Faz sentido mudar isso?

#++++++++++++++++++++++++++++++++++++++++++++++

#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:05:00
#
#-------------------------------------------------
VERSION = 2.0.1

# Define the preprocessor macro to get the application version in Jamtaba application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       +=  gui  network

macx{
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.6 -stdlib=libc++
    LIBS += -mmacosx-version-min=10.6 -stdlib=libc++
}

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Jamtaba2
TEMPLATE = app

MAIN = src/jamtaba/main.cpp
#MAIN = src/jamtaba/ninjam/_main.cpp


HEADERS += \
    src/jamtaba/audio/core/AudioDriver.h \
    src/jamtaba/audio/core/AudioNode.h \
    src/jamtaba/audio/core/AudioMixer.h \
    src/jamtaba/audio/core/PortAudioDriver.h \
    #--------------------------------
    src/jamtaba/gui/widgets/PeakMeter.h \
    src/jamtaba/gui/widgets/WavePeakPanel.h \
    #--------------------------------
    src/jamtaba/loginserver/LoginService.h \
    src/jamtaba/loginserver/JsonUtils.h \
    #--------------------------------
    src/jamtaba/MainController.h \
    src/jamtaba/JamtabaFactory.h \
    #--------------------------------
    src/jamtaba/recorder/JamRecorder.h \
    src/jamtaba/recorder/ReaperProjectGenerator.h \
    #--------------------------------
    src/jamtaba/ninjam/protocol/ServerMessageParser.h \
    src/jamtaba/ninjam/protocol/ServerMessages.h \
    src/jamtaba/ninjam/protocol/ClientMessages.h \
    src/jamtaba/ninjam/User.h \
    src/jamtaba/ninjam/Service.h \
    src/jamtaba/ninjam/Server.h \
    #--------------------------------
    src/jamtaba/loginserver/natmap.h \
    src/jamtaba/audio/RoomStreamerNode.h \
    src/jamtaba/audio/codec.h \
    src/jamtaba/gui/LocalTrackView.h \
    src/jamtaba/gui/jamroomviewpanel.h \
    src/jamtaba/gui/mainframe.h \
    src/jamtaba/gui/FxPanel.h \
    src/jamtaba/gui/FxPanelItem.h \
    src/jamtaba/audio/core/plugins.h \
    src/jamtaba/gui/plugins/guis.h \
    src/jamtaba/midi/MidiDriver.h \
    src/jamtaba/midi/portmididriver.h \
    src/jamtaba/gui/pluginscandialog.h \
    src/jamtaba/gui/PreferencesDialog.h \
    src/jamtaba/gui/NinjamRoomWindow.h \
    src/jamtaba/gui/BaseTrackView.h \
    src/jamtaba/audio/NinjamTrackNode.h \
    src/jamtaba/gui/NinjamTrackView.h \
    src/jamtaba/audio/MetronomeTrackNode.h \
    src/jamtaba/gui/NinjamPanel.h \
    src/jamtaba/gui/FancyProgressDisplay.h \
    src/jamtaba/audio/Resampler.h \
    src/jamtaba/audio/vorbis/VorbisDecoder.h \
    src/jamtaba/ninjam/UserChannel.h \
    src/jamtaba/audio/core/SamplesBuffer.h \
    src/jamtaba/gui/BusyDialog.h \
    src/jamtaba/audio/core/AudioPeak.h \
    src/jamtaba/geo/IpToLocationResolver.h \
    src/jamtaba/gui/ChatPanel.h \
    src/jamtaba/gui/ChatMessagePanel.h \
    src/jamtaba/audio/SamplesBufferResampler.h \
    src/jamtaba/audio/samplesbufferrecorder.h \
    src/jamtaba/audio/vorbis/VorbisEncoder.h \
    src/jamtaba/gui/Highligther.h \
    src/jamtaba/persistence/Settings.h \
    src/jamtaba/Utils.h \
    src/jamtaba/gui/TrackGroupView.h \
    src/jamtaba/gui/LocalTrackGroupView.h \
    src/jamtaba/NinjamController.h \
    src/jamtaba/gui/IntervalProgressDisplay.h \
    src/jamtaba/audio/vst/PluginFinder.h \
    src/jamtaba/audio/vst/VstPlugin.h \
    src/jamtaba/audio/vst/vsthost.h \
    #src/jamtaba/geo/IpToLocationLITEResolver.h
    src/jamtaba/geo/WebIpToLocationResolver.h \
    src/jamtaba/gui/PrivateServerDialog.h \
    src/jamtaba/gui/UserNameDialog.h
    #src/jamtaba/midi/RtMidiDriver.h

SOURCES += \
    $$MAIN \
    src/jamtaba/audio/core/AudioDriver.cpp \
    src/jamtaba/audio/core/AudioNode.cpp \
    src/jamtaba/audio/core/AudioMixer.cpp \
    src/jamtaba/audio/core/PortAudioDriver.cpp \
    src/jamtaba/audio/RoomStreamerNode.cpp \
#------------------------------------------------
    src/jamtaba/recorder/JamRecorder.cpp \
    src/jamtaba/recorder/ReaperProjectGenerator.cpp \
#------------------------------------------------
    src/jamtaba/gui/widgets/PeakMeter.cpp \
    src/jamtaba/gui/widgets/WavePeakPanel.cpp \
#------------------------------------------------
    src/jamtaba/JamtabaFactory.cpp \
    src/jamtaba/MainController.cpp \
#------------------------------------------------
    src/jamtaba/loginserver/LoginService.cpp \
    src/jamtaba/loginserver/JsonUtils.cpp \
#------------------------------------------------
    src/jamtaba/ninjam/protocol/ServerMessages.cpp \
    src/jamtaba/ninjam/protocol/ClientMessages.cpp \
    src/jamtaba/ninjam/protocol/ServerMessageParser.cpp \
    src/jamtaba/ninjam/Server.cpp \
    src/jamtaba/ninjam/Service.cpp \
    src/jamtaba/ninjam/User.cpp \
    src/jamtaba/gui/LocalTrackView.cpp \
    src/jamtaba/gui/FxPanel.cpp \
    src/jamtaba/gui/FxPanelItem.cpp \
    src/jamtaba/audio/core/plugins.cpp \
    src/jamtaba/audio/codec.cpp \
    src/jamtaba/gui/plugins/guis.cpp \
    src/jamtaba/gui/jamroomviewpanel.cpp \
    src/jamtaba/gui/mainframe.cpp \
    src/jamtaba/audio/vst/PluginFinder.cpp \
    src/jamtaba/midi/MidiDriver.cpp \
    src/jamtaba/gui/PreferencesDialog.cpp \
    src/jamtaba/gui/pluginscandialog.cpp \
    src/jamtaba/midi/portmididriver.cpp \
    src/jamtaba/gui/NinjamRoomWindow.cpp \
    src/jamtaba/gui/BaseTrackView.cpp \
    src/jamtaba/audio/NinjamTrackNode.cpp \
    src/jamtaba/gui/NinjamTrackView.cpp \
    src/jamtaba/audio/MetronomeTrackNode.cpp \
    src/jamtaba/gui/NinjamPanel.cpp \
    src/jamtaba/gui/FancyProgressDisplay.cpp \
    src/jamtaba/audio/Resampler.cpp \
    src/jamtaba/audio/vorbis/VorbisDecoder.cpp \
    src/jamtaba/ninjam/UserChannel.cpp \
    src/jamtaba/audio/core/SamplesBuffer.cpp \
    src/jamtaba/gui/BusyDialog.cpp \
    src/jamtaba/audio/core/AudioPeak.cpp \
    src/jamtaba/geo/IpToLocationResolver.cpp \
    src/jamtaba/gui/ChatPanel.cpp \
    src/jamtaba/gui/ChatMessagePanel.cpp \
    src/jamtaba/audio/SamplesBufferResampler.cpp \
    src/jamtaba/audio/samplesbufferrecorder.cpp \
    src/jamtaba/audio/vorbis/VorbisEncoder.cpp \
    src/jamtaba/gui/Highligther.cpp \
    src/jamtaba/persistence/Settings.cpp \
    src/jamtaba/gui/TrackGroupView.cpp \
    src/jamtaba/gui/LocalTrackGroupView.cpp \
    src/jamtaba/NinjamController.cpp \
    src/jamtaba/gui/IntervalProgressDisplay.cpp \
    #src/jamtaba/geo/MaxMindIpToLocationResolver.cpp \
    #src/jamtaba/geo/IpToLocationLITEResolver.cpp \
    src/jamtaba/audio/vst/VstPlugin.cpp \
    src/jamtaba/audio/vst/vsthost.cpp \
    src/jamtaba/geo/WebIpToLocationResolver.cpp \
    src/jamtaba/gui/PrivateServerDialog.cpp \
    src/jamtaba/gui/UserNameDialog.cpp
    #src/jamtaba/midi/RtMidiDriver.cpp


FORMS += \
    src/jamtaba/gui/PreferencesDialog.ui \
    src/jamtaba/gui/pluginscandialog.ui \
    src/jamtaba/gui/mainframe.ui \
    src/jamtaba/gui/NinjamRoomWindow.ui \
    src/jamtaba/gui/BaseTrackView.ui \
    src/jamtaba/gui/NinjamPanel.ui \
    src/jamtaba/gui/BusyDialog.ui \
    src/jamtaba/gui/ChatPanel.ui \
    src/jamtaba/gui/ChatMessagePanel.ui \
    src/jamtaba/gui/TrackGroupView.ui \
    src/jamtaba/gui/jamroomviewpanel.ui \
    src/jamtaba/gui/PrivateServerDialog.ui \
    src/jamtaba/gui/UserNameDialog.ui

INCLUDEPATH += src/jamtaba/gui                  \
               src/jamtaba/gui/widgets          \
               $$PWD/libs/includes/portaudio    \
               $$PWD/libs/includes/portmidi       \
               $$PWD/libs/includes/ogg          \
               $$PWD/libs/includes/vorbis       \
               $$PWD/libs/includes/minimp3      \
               $$PWD/libs/includes/libip2location   \

DEPENDPATH += $$PWD/libs/includes/portaudio     \
               $$PWD/libs/includes/portmidi       \
               $$PWD/libs/includes/ogg          \
               $$PWD/libs/includes/vorbis       \
               $$PWD/libs/includes/minimp3      \
               $$PWD/libs/includes/libip2location   \


win32 {

    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32  \

    VST_SDK_PATH = "E:/Jamtaba2\VST3 SDK/pluginterfaces/vst2.x/"

    !contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build") ## Windows x86 (32bit) specific build here
        LIBS_PATH = "win32-msvc"
    } else {
        message("x86_64 build") ## Windows x64 (64bit) specific build here
        LIBS_PATH = "win64-msvc"
    }

    CONFIG(release, debug|release): LIBS += -L$$PWD/libs/$$LIBS_PATH/ -lportaudio -lminimp3 -lportmidi -lvorbisfile -lvorbis -logg
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lportmidid -lvorbisfiled -lvorbisd -loggd

    RC_FILE = Jamtaba2.rc #windows icon

}

macx{
    message("Mac build")

    VST_SDK_PATH = "/Users/elieser/Desktop/VST3 SDK/pluginterfaces/vst2.x"

    macx-clang-32 {
        message("i386 build") ## mac 32bit specific build here
        LIBS_PATH = "mac32"
    } else {
        message("x86_64 build") ## mac 64bit specific build here
        LIBS_PATH = "mac64"
    }

    LIBS += -framework CoreAudio
    LIBS += -framework CoreMidi
    LIBS += -framework AudioToolbox
    LIBS += -framework AudioUnit
    LIBS += -framework CoreServices
    LIBS += -framework Carbon

    LIBS += -L$$PWD/libs/$$LIBS_PATH/ -lportaudio -lminimp3  -lportmidi -lvorbisfile -lvorbisenc -lvorbis -logg

    #mac osx doc icon
    ICON = Jamtaba.icns


}

INCLUDEPATH += $$VST_SDK_PATH  \


RESOURCES += src/jamtaba/resources/jamtaba.qrc

