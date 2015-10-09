#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:05:00
#
#-------------------------------------------------
VERSION = 1.0.0

# Define the preprocessor macro to get the application version in Jamtaba application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       +=  gui widgets network

#macx{
#    QMAKE_CXXFLAGS += -mmacosx-version-min=10.6 -stdlib=libc++
#    LIBS += -mmacosx-version-min=10.6 -stdlib=libc++
#}

CONFIG += c++11

TARGET = Jamtaba2-ThemeEditor
TEMPLATE = app

MAIN = src/jamtaba/ThemeEditorMain.cpp


HEADERS += \
    src/jamtaba/audio/core/AudioDriver.h \
    src/jamtaba/audio/core/AudioNode.h \
    src/jamtaba/audio/core/AudioMixer.h \
    #src/jamtaba/audio/core/PortAudioDriver.h \
    #--------------------------------
    src/jamtaba/gui/widgets/PeakMeter.h \
    src/jamtaba/gui/widgets/WavePeakPanel.h \
    #--------------------------------
    src/jamtaba/loginserver/LoginService.h \
    src/jamtaba/loginserver/JsonUtils.h \
    #--------------------------------
    #src/jamtaba/MainController.h \
    src/jamtaba/JamtabaFactory.h \
    #--------------------------------
    #src/jamtaba/recorder/JamRecorder.h \
    #src/jamtaba/recorder/ReaperProjectGenerator.h \
    #--------------------------------
    #src/jamtaba/ninjam/protocol/ServerMessageParser.h \
    #src/jamtaba/ninjam/protocol/ServerMessages.h \
    #src/jamtaba/ninjam/protocol/ClientMessages.h \
    #src/jamtaba/ninjam/User.h \
    #src/jamtaba/ninjam/Service.h \
    #src/jamtaba/ninjam/Server.h \
    #--------------------------------
    #src/jamtaba/loginserver/natmap.h \
    #src/jamtaba/audio/RoomStreamerNode.h \
    #src/jamtaba/audio/codec.h \
    src/jamtaba/gui/LocalTrackView.h \
    src/jamtaba/gui/jamroomviewpanel.h \
    src/jamtaba/gui/mainframe.h \
    src/jamtaba/gui/FxPanel.h \
    src/jamtaba/gui/FxPanelItem.h \
    #src/jamtaba/audio/core/plugins.h \
    src/jamtaba/gui/plugins/guis.h \
    #src/jamtaba/midi/MidiDriver.h \
    #src/jamtaba/midi/portmididriver.h \
    src/jamtaba/gui/pluginscandialog.h \
    src/jamtaba/gui/PreferencesDialog.h \
    src/jamtaba/gui/NinjamRoomWindow.h \
    src/jamtaba/gui/BaseTrackView.h \
    #src/jamtaba/audio/NinjamTrackNode.h \
    src/jamtaba/gui/NinjamTrackView.h \
    #src/jamtaba/audio/MetronomeTrackNode.h \
    src/jamtaba/gui/NinjamPanel.h \
    src/jamtaba/gui/FancyProgressDisplay.h \
    #src/jamtaba/audio/Resampler.h \
    src/jamtaba/ninjam/UserChannel.h \
    src/jamtaba/audio/core/SamplesBuffer.h \
    src/jamtaba/gui/BusyDialog.h \
    src/jamtaba/audio/core/AudioPeak.h \
    src/jamtaba/geo/IpToLocationResolver.h \
    src/jamtaba/gui/ChatPanel.h \
    src/jamtaba/gui/ChatMessagePanel.h \
    #src/jamtaba/audio/SamplesBufferResampler.h \
    #src/jamtaba/audio/samplesbufferrecorder.h \
    src/jamtaba/gui/Highligther.h \
    #src/jamtaba/persistence/Settings.h \
    src/jamtaba/Utils.h \
    src/jamtaba/gui/TrackGroupView.h \
    src/jamtaba/gui/LocalTrackGroupView.h \
    src/jamtaba/NinjamController.h \
    src/jamtaba/gui/IntervalProgressDisplay.h \
    #src/jamtaba/audio/vst/PluginFinder.h \
    #src/jamtaba/audio/vst/VstPlugin.h \
    #src/jamtaba/audio/vst/vsthost.h \
    #src/jamtaba/geo/IpToLocationLITEResolver.h
    #src/jamtaba/geo/WebIpToLocationResolver.h \
    src/jamtaba/gui/PrivateServerDialog.h \
    src/jamtaba/gui/UserNameDialog.h

SOURCES += \
    $$MAIN \
    src/jamtaba/audio/core/AudioDriver.cpp \
    #src/jamtaba/audio/core/AudioNode.cpp \
    #src/jamtaba/audio/core/AudioMixer.cpp \
    #src/jamtaba/audio/core/PortAudioDriver.cpp \
    #src/jamtaba/audio/RoomStreamerNode.cpp \
#------------------------------------------------
    #src/jamtaba/recorder/JamRecorder.cpp \
    #src/jamtaba/recorder/ReaperProjectGenerator.cpp \
#------------------------------------------------
    src/jamtaba/gui/widgets/PeakMeter.cpp \
    src/jamtaba/gui/widgets/WavePeakPanel.cpp \
#------------------------------------------------
    src/jamtaba/JamtabaFactory.cpp \
    #src/jamtaba/MainController.cpp \
#------------------------------------------------
    src/jamtaba/loginserver/LoginService.cpp \
    src/jamtaba/loginserver/JsonUtils.cpp \
#------------------------------------------------
#    src/jamtaba/ninjam/protocol/ServerMessages.cpp \
#    src/jamtaba/ninjam/protocol/ClientMessages.cpp \
#    src/jamtaba/ninjam/protocol/ServerMessageParser.cpp \
#    src/jamtaba/ninjam/Server.cpp \
#    src/jamtaba/ninjam/Service.cpp \
#    src/jamtaba/ninjam/User.cpp \
    src/jamtaba/gui/LocalTrackView.cpp \
    src/jamtaba/gui/FxPanel.cpp \
    src/jamtaba/gui/FxPanelItem.cpp \
#    src/jamtaba/audio/core/plugins.cpp \
#    src/jamtaba/audio/codec.cpp \
#    src/jamtaba/gui/plugins/guis.cpp \
    src/jamtaba/gui/jamroomviewpanel.cpp \
    src/jamtaba/gui/mainframe.cpp \
#    src/jamtaba/audio/vst/PluginFinder.cpp \
#    src/jamtaba/midi/MidiDriver.cpp \
    src/jamtaba/gui/PreferencesDialog.cpp \
    src/jamtaba/gui/pluginscandialog.cpp \
#    src/jamtaba/midi/portmididriver.cpp \
    src/jamtaba/gui/NinjamRoomWindow.cpp \
    src/jamtaba/gui/BaseTrackView.cpp \
#    src/jamtaba/audio/NinjamTrackNode.cpp \
    src/jamtaba/gui/NinjamTrackView.cpp \
#    src/jamtaba/audio/MetronomeTrackNode.cpp \
    src/jamtaba/gui/NinjamPanel.cpp \
    src/jamtaba/gui/FancyProgressDisplay.cpp \
#    src/jamtaba/audio/Resampler.cpp \
    src/jamtaba/ninjam/UserChannel.cpp \
#    src/jamtaba/audio/core/SamplesBuffer.cpp \
    src/jamtaba/gui/BusyDialog.cpp \
    src/jamtaba/audio/core/AudioPeak.cpp \
    src/jamtaba/geo/IpToLocationResolver.cpp \
    src/jamtaba/gui/ChatPanel.cpp \
    src/jamtaba/gui/ChatMessagePanel.cpp \
#    src/jamtaba/audio/SamplesBufferResampler.cpp \
#    src/jamtaba/audio/samplesbufferrecorder.cpp \
    src/jamtaba/gui/Highligther.cpp \
#    src/jamtaba/persistence/Settings.cpp \
    src/jamtaba/gui/TrackGroupView.cpp \
    src/jamtaba/gui/LocalTrackGroupView.cpp \
    src/jamtaba/NinjamController.cpp \
    src/jamtaba/gui/IntervalProgressDisplay.cpp \
#    src/jamtaba/audio/vst/VstPlugin.cpp \
#    src/jamtaba/audio/vst/vsthost.cpp \
#    src/jamtaba/geo/WebIpToLocationResolver.cpp \
    src/jamtaba/gui/PrivateServerDialog.cpp \
    src/jamtaba/gui/UserNameDialog.cpp \
    src/jamtaba/ThemeEditorMain.cpp

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


#               $$PWD/libs/includes/portaudio    \
#               $$PWD/libs/includes/portmidi       \
#               $$PWD/libs/includes/ogg          \
#               $$PWD/libs/includes/minimp3      \
#               $$PWD/libs/includes/libip2location   \

#DEPENDPATH += $$PWD/libs/includes/portaudio     \
#               $$PWD/libs/includes/portmidi       \
#               $$PWD/libs/includes/ogg          \
#               $$PWD/libs/includes/minimp3      \
#               $$PWD/libs/includes/libip2location   \


win32 {
    LIBS +=  -lwinmm -lole32 -lws2_32 -lAdvapi32 -lUser32  \
    RC_FILE = Jamtaba2.rc #windows icon
}

RESOURCES += src/jamtaba/resources/jamtaba.qrc
