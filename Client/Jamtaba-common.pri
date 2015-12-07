VST_SDK_PATH = "$$PWD/../VST_SDK"

TEMPLATE = lib

VERSION = 1.0.0

# Define the preprocessor macro to get the application version in Jamtaba application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += VST_FORCE_DEPRECATED=0 #enable VST 2.3 features

macx{
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

CONFIG += c++11

HEADERS += \
    ../src/audio/core/AudioDriver.h \
    ../src/audio/core/AudioNode.h \
    ../src/audio/core/AudioMixer.h \
    #--------------------------------
    ../src/gui/widgets/PeakMeter.h \
    ../src/gui/widgets/WavePeakPanel.h \
    #--------------------------------
    ../src/loginserver/LoginService.h \
    ../src/loginserver/JsonUtils.h \
    #--------------------------------
    ../src/MainController.h \
    #--------------------------------
    ../src/ninjam/User.h \
    ../src/ninjam/Service.h \
    ../src/ninjam/Server.h \
    #--------------------------------
    ../src/audio/RoomStreamerNode.h \
    ../src/gui/LocalTrackView.h \
    ../src/gui/jamroomviewpanel.h \
    ../src/gui/FxPanel.h \
    ../src/gui/FxPanelItem.h \
    ../src/audio/core/plugins.h \
    ../src/gui/plugins/guis.h \
    ../src/midi/MidiDriver.h \
    ../src/gui/pluginscandialog.h \
    ../src/gui/PreferencesDialog.h \
    ../src/gui/NinjamRoomWindow.h \
    ../src/gui/BaseTrackView.h \
    ../src/audio/NinjamTrackNode.h \
    ../src/gui/NinjamTrackView.h \
    ../src/audio/MetronomeTrackNode.h \
    ../src/gui/NinjamPanel.h \
    ../src/ninjam/UserChannel.h \
    ../src/audio/core/SamplesBuffer.h \
    ../src/gui/BusyDialog.h \
    ../src/audio/core/AudioPeak.h \
    ../src/geo/IpToLocationResolver.h \
    ../src/gui/ChatPanel.h \
    ../src/gui/ChatMessagePanel.h \
    ../src/audio/SamplesBufferResampler.h \
    ../src/audio/samplesbufferrecorder.h \
    ../src/gui/Highligther.h \
    ../src/Utils.h \
    ../src/gui/TrackGroupView.h \
    ../src/gui/LocalTrackGroupView.h \
    ../src/NinjamController.h \
    ../src/gui/IntervalProgressDisplay.h \
    ../src/audio/vst/PluginFinder.h \
    ../src/gui/PrivateServerDialog.h \
    ../src/gui/UserNameDialog.h \
    ../src/gui/MainWindow.h \
    $$PWD/src/log/logging.h \
    $$PWD/src/gui/widgets/CustomTabWidget.h \
#    $$PWD/src/performance/PerformanceMonitor.h \
    $$PWD/src/gui/chords/ChordLabel.h \
    $$PWD/src/gui/BpiUtils.h \
    $$PWD/src/gui/chords/ChordsWidget.h \
    $$PWD/src/gui/chords/ChordsPanel.h \
    $$PWD/src/gui/chords/ChordProgression.h \
    $$PWD/src/configurator.h \

SOURCES += \
    ../src/audio/core/AudioDriver.cpp \
    ../src/audio/core/AudioNode.cpp \
    ../src/audio/core/AudioMixer.cpp \
    ../src/audio/RoomStreamerNode.cpp \
#------------------------------------------------
    ../src/gui/widgets/PeakMeter.cpp \
    ../src/gui/widgets/WavePeakPanel.cpp \
#------------------------------------------------
    ../src/MainController.cpp \
##------------------------------------------------
    ../src/ninjam/Service.cpp \
    ../src/ninjam/User.cpp \
    ../src/gui/LocalTrackView.cpp \
    ../src/gui/FxPanel.cpp \
    ../src/gui/FxPanelItem.cpp \
    ../src/audio/core/plugins.cpp \
    ../src/audio/codec.cpp \
    ../src/gui/plugins/guis.cpp \
    ../src/gui/jamroomviewpanel.cpp \
    ../src/audio/vst/PluginFinder.cpp \
    ../src/midi/MidiDriver.cpp \
    ../src/gui/PreferencesDialog.cpp \
    ../src/gui/pluginscandialog.cpp \
    ../src/gui/NinjamRoomWindow.cpp \
    ../src/gui/BaseTrackView.cpp \
    ../src/audio/NinjamTrackNode.cpp \
    ../src/gui/NinjamTrackView.cpp \
    ../src/audio/MetronomeTrackNode.cpp \
    ../src/gui/NinjamPanel.cpp \
    ../src/ninjam/UserChannel.cpp \
    ../src/audio/core/SamplesBuffer.cpp \
    ../src/audio/SamplesBufferResampler.cpp \
    ../src/gui/BusyDialog.cpp \
    ../src/audio/core/AudioPeak.cpp \
    ../src/geo/IpToLocationResolver.cpp \
    ../src/gui/ChatPanel.cpp \
    ../src/gui/ChatMessagePanel.cpp \
    ../src/gui/Highligther.cpp \
    ../src/gui/TrackGroupView.cpp \
    ../src/gui/LocalTrackGroupView.cpp \
    ../src/NinjamController.cpp \
    ../src/gui/IntervalProgressDisplay.cpp \
    ../src/gui/PrivateServerDialog.cpp \
    ../src/gui/UserNameDialog.cpp \
    $$PWD/src/gui/MainWindow.cpp \
    $$PWD/src/log/logging.cpp \
    $$PWD/src/gui/widgets/CustomTabWidget.cpp \
    $$PWD/src/gui/chords/ChordLabel.cpp \
    $$PWD/src/gui/BpiUtils.cpp \
    $$PWD/src/gui/chords/ChordsWidget.cpp \
    $$PWD/src/gui/chords/ChordsPanel.cpp \
    $$PWD/src/gui/chords/ChordProgression.cpp \
    $$PWD/src/configurator.cpp \


#multiplatform implementations
#win32:SOURCES += $$PWD/src/performance/WindowsPerformanceMonitor.cpp
#macx:SOURCES += $$PWD/src/performance/MacPerformanceMonitor.cpp

FORMS += \
    ../src/gui/PreferencesDialog.ui \
    ../src/gui/pluginscandialog.ui \
    ../src/gui/NinjamRoomWindow.ui \
    ../src/gui/BaseTrackView.ui \
    ../src/gui/NinjamPanel.ui \
    ../src/gui/BusyDialog.ui \
    ../src/gui/ChatPanel.ui \
    ../src/gui/ChatMessagePanel.ui \
    ../src/gui/TrackGroupView.ui \
    ../src/gui/jamroomviewpanel.ui \
    ../src/gui/PrivateServerDialog.ui \
    ../src/gui/UserNameDialog.ui \
    $$PWD/src/gui/MainWindow.ui \
    $$PWD/src/gui/PreferencesDialog.ui \
    $$PWD/src/gui/chords/ChordsPanel.ui


INCLUDEPATH += ../src/ \
               ../src/gui \
               ../src/gui/widgets \

               ../src/gui/chords \

RESOURCES += ../src/resources/jamtaba.qrc
