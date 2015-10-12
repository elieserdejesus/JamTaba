TEMPLATE = lib

VERSION = 2.0.2

# Define the preprocessor macro to get the application version in Jamtaba application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += VST_FORCE_DEPRECATED=0

macx{
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

CONFIG += c++11

HEADERS += \
    ../src/audio/core/AudioDriver.h \
    ../src/audio/core/AudioNode.h \
    ../src/audio/core/AudioMixer.h \
    ../src/audio/core/PortAudioDriver.h \
    #--------------------------------
    ../src/gui/widgets/PeakMeter.h \
    ../src/gui/widgets/WavePeakPanel.h \
    #--------------------------------
    ../src/loginserver/LoginService.h \
    ../src/loginserver/JsonUtils.h \
    #--------------------------------
    ../src/MainController.h \
    ../src/JamtabaFactory.h \
    #--------------------------------
    ../src/recorder/JamRecorder.h \
    ../src/recorder/ReaperProjectGenerator.h \
    #--------------------------------
    ../src/ninjam/protocol/ServerMessageParser.h \
    ../src/ninjam/protocol/ServerMessages.h \
    ../src/ninjam/protocol/ClientMessages.h \
    ../src/ninjam/User.h \
    ../src/ninjam/Service.h \
    ../src/ninjam/Server.h \
    #--------------------------------
    ../src/loginserver/natmap.h \
    ../src/audio/RoomStreamerNode.h \
    ../src/audio/codec.h \
    ../src/gui/LocalTrackView.h \
    ../src/gui/jamroomviewpanel.h \
    ../src/gui/mainframe.h \
    ../src/gui/FxPanel.h \
    ../src/gui/FxPanelItem.h \
    ../src/audio/core/plugins.h \
    ../src/gui/plugins/guis.h \
    ../src/midi/MidiDriver.h \
    ../src/midi/portmididriver.h \
    ../src/gui/pluginscandialog.h \
    ../src/gui/PreferencesDialog.h \
    ../src/gui/NinjamRoomWindow.h \
    ../src/gui/BaseTrackView.h \
    ../src/audio/NinjamTrackNode.h \
    ../src/gui/NinjamTrackView.h \
    ../src/audio/MetronomeTrackNode.h \
    ../src/gui/NinjamPanel.h \
    ../src/gui/FancyProgressDisplay.h \
    ../src/audio/Resampler.h \
    ../src/audio/vorbis/VorbisDecoder.h \
    ../src/ninjam/UserChannel.h \
    ../src/audio/core/SamplesBuffer.h \
    ../src/gui/BusyDialog.h \
    ../src/audio/core/AudioPeak.h \
    ../src/geo/IpToLocationResolver.h \
    ../src/gui/ChatPanel.h \
    ../src/gui/ChatMessagePanel.h \
    ../src/audio/SamplesBufferResampler.h \
    ../src/audio/samplesbufferrecorder.h \
    ../src/audio/vorbis/VorbisEncoder.h \
    ../src/gui/Highligther.h \
    ../src/persistence/Settings.h \
    ../src/Utils.h \
    ../src/gui/TrackGroupView.h \
    ../src/gui/LocalTrackGroupView.h \
    ../src/NinjamController.h \
    ../src/gui/IntervalProgressDisplay.h \
    ../src/audio/vst/PluginFinder.h \
    ../src/audio/vst/VstPlugin.h \
    ../src/audio/vst/vsthost.h \
    #../src/geo/IpToLocationLITEResolver.h
    ../src/geo/WebIpToLocationResolver.h \
    ../src/gui/PrivateServerDialog.h \
    ../src/gui/UserNameDialog.h
    #../src/midi/RtMidiDriver.h

SOURCES += \
    #$$MAIN \
    ../src/audio/core/AudioDriver.cpp \
    ../src/audio/core/AudioNode.cpp \
    ../src/audio/core/AudioMixer.cpp \
    ../src/audio/core/PortAudioDriver.cpp \
    ../src/audio/RoomStreamerNode.cpp \
#------------------------------------------------
    ../src/recorder/JamRecorder.cpp \
    ../src/recorder/ReaperProjectGenerator.cpp \
#------------------------------------------------
    ../src/gui/widgets/PeakMeter.cpp \
    ../src/gui/widgets/WavePeakPanel.cpp \
#------------------------------------------------
    ../src/JamtabaFactory.cpp \
    ../src/MainController.cpp \
#------------------------------------------------
    ../src/loginserver/LoginService.cpp \
    ../src/loginserver/JsonUtils.cpp \
#------------------------------------------------
    ../src/ninjam/protocol/ServerMessages.cpp \
    ../src/ninjam/protocol/ClientMessages.cpp \
    ../src/ninjam/protocol/ServerMessageParser.cpp \
    ../src/ninjam/Server.cpp \
    ../src/ninjam/Service.cpp \
    ../src/ninjam/User.cpp \
    ../src/gui/LocalTrackView.cpp \
    ../src/gui/FxPanel.cpp \
    ../src/gui/FxPanelItem.cpp \
    ../src/audio/core/plugins.cpp \
    ../src/audio/codec.cpp \
    ../src/gui/plugins/guis.cpp \
    ../src/gui/jamroomviewpanel.cpp \
    ../src/gui/mainframe.cpp \
    ../src/audio/vst/PluginFinder.cpp \
    ../src/midi/MidiDriver.cpp \
    ../src/gui/PreferencesDialog.cpp \
    ../src/gui/pluginscandialog.cpp \
    ../src/midi/portmididriver.cpp \
    ../src/gui/NinjamRoomWindow.cpp \
    ../src/gui/BaseTrackView.cpp \
    ../src/audio/NinjamTrackNode.cpp \
    ../src/gui/NinjamTrackView.cpp \
    ../src/audio/MetronomeTrackNode.cpp \
    ../src/gui/NinjamPanel.cpp \
    ../src/gui/FancyProgressDisplay.cpp \
    ../src/audio/Resampler.cpp \
    ../src/audio/vorbis/VorbisDecoder.cpp \
    ../src/ninjam/UserChannel.cpp \
    ../src/audio/core/SamplesBuffer.cpp \
    ../src/gui/BusyDialog.cpp \
    ../src/audio/core/AudioPeak.cpp \
    ../src/geo/IpToLocationResolver.cpp \
    ../src/gui/ChatPanel.cpp \
    ../src/gui/ChatMessagePanel.cpp \
    ../src/audio/SamplesBufferResampler.cpp \
    ../src/audio/samplesbufferrecorder.cpp \
    ../src/audio/vorbis/VorbisEncoder.cpp \
    ../src/gui/Highligther.cpp \
    ../src/persistence/Settings.cpp \
    ../src/gui/TrackGroupView.cpp \
    ../src/gui/LocalTrackGroupView.cpp \
    ../src/NinjamController.cpp \
    ../src/gui/IntervalProgressDisplay.cpp \
    #../src/geo/MaxMindIpToLocationResolver.cpp \
    #../src/geo/IpToLocationLITEResolver.cpp \
    ../src/audio/vst/VstPlugin.cpp \
    ../src/audio/vst/vsthost.cpp \
    ../src/geo/WebIpToLocationResolver.cpp \
    ../src/gui/PrivateServerDialog.cpp \
    ../src/gui/UserNameDialog.cpp
    #../src/midi/RtMidiDriver.cpp


FORMS += \
    ../src/gui/PreferencesDialog.ui \
    ../src/gui/pluginscandialog.ui \
    ../src/gui/mainframe.ui \
    ../src/gui/NinjamRoomWindow.ui \
    ../src/gui/BaseTrackView.ui \
    ../src/gui/NinjamPanel.ui \
    ../src/gui/BusyDialog.ui \
    ../src/gui/ChatPanel.ui \
    ../src/gui/ChatMessagePanel.ui \
    ../src/gui/TrackGroupView.ui \
    ../src/gui/jamroomviewpanel.ui \
    ../src/gui/PrivateServerDialog.ui \
    ../src/gui/UserNameDialog.ui

INCLUDEPATH += ../src/gui                  \
               ../src/gui/widgets          \

RESOURCES += ../src/resources/jamtaba.qrc
