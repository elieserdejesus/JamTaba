VST_SDK_PATH = "$$PWD/../VST_SDK"

message("VST PATH: " $$VST_SDK_PATH)

TEMPLATE = lib

VERSION = 2.0.14

ROOT_PATH = "../.."
SOURCE_PATH = "$$ROOT_PATH/src"

INCLUDEPATH += $$SOURCE_PATH/Common
INCLUDEPATH += $$SOURCE_PATH/Common/gui
INCLUDEPATH += $$SOURCE_PATH/Common/gui/widgets
INCLUDEPATH += $$SOURCE_PATH/Common/gui/chords

VPATH       += $$SOURCE_PATH/Common
VPATH       += $$SOURCE_PATH

# Define the preprocessor macro to get the application version in Jamtaba application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += VST_FORCE_DEPRECATED=0#enable VST 2.3 features

linux{ #avoid erros in VST SDK when compiling in Linux
    DEFINES += __cdecl=""
}

macx{
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

CONFIG += c++11

PRECOMPILED_HEADER += PreCompiledHeaders.h

HEADERS += midi/MidiDriver.h
HEADERS += midi/MidiMessage.h
HEADERS += midi/MidiMessageBuffer.h
HEADERS += audio/core/AudioDriver.h
HEADERS += audio/core/AudioNode.h
HEADERS += audio/core/LocalInputNode.h
HEADERS += audio/core/LocalInputGroup.h
HEADERS += audio/core/AudioNodeProcessor.h
HEADERS += audio/core/AudioMixer.h
HEADERS += audio/core/SamplesBuffer.h
HEADERS += audio/core/AudioPeak.h
HEADERS += audio/core/Plugins.h
HEADERS += audio/vorbis/VorbisDecoder.h
HEADERS += audio/vorbis/VorbisEncoder.h
HEADERS += audio/RoomStreamerNode.h
HEADERS += audio/NinjamTrackNode.h
HEADERS += audio/MetronomeTrackNode.h
HEADERS += audio/SamplesBufferResampler.h
HEADERS += audio/SamplesBufferRecorder.h
HEADERS += audio/codec.h
HEADERS += audio/Resampler.h
HEADERS += audio/file/FileReader.h
HEADERS += audio/file/FileReaderFactory.h
HEADERS += audio/file/WaveFileReader.h
HEADERS += audio/file/OggFileReader.h
HEADERS += recorder/JamRecorder.h
HEADERS += recorder/ReaperProjectGenerator.h
HEADERS += loginserver/LoginService.h
HEADERS += loginserver/natmap.h
HEADERS += MainController.h
HEADERS += NinjamController.h
HEADERS += MetronomeUtils.h
HEADERS += ninjam/User.h
HEADERS += ninjam/Service.h
HEADERS += ninjam/Server.h
HEADERS += ninjam/ServerMessages.h
HEADERS += ninjam/ClientMessages.h
HEADERS += ninjam/ServerMessagesHandler.h
HEADERS += gui/plugins/Guis.h
HEADERS += gui/PluginScanDialog.h
HEADERS += gui/PreferencesDialog.h
HEADERS += gui/NinjamRoomWindow.h
HEADERS += gui/BaseTrackView.h
HEADERS += gui/NinjamTrackView.h
HEADERS += gui/NinjamTrackGroupView.h
HEADERS += gui/NinjamPanel.h
HEADERS += gui/BusyDialog.h
HEADERS += gui/ChatPanel.h
HEADERS += gui/ChatMessagePanel.h
HEADERS += gui/Highligther.h
HEADERS += gui/TrackGroupView.h
HEADERS += gui/LocalTrackGroupView.h
HEADERS += gui/intervalProgress/IntervalProgressDisplay.h
HEADERS += gui/intervalProgress/IntervalProgressWindow.h
HEADERS += gui/PrivateServerDialog.h
HEADERS += gui/UserNameDialog.h
HEADERS += gui/MainWindow.h
HEADERS += gui/widgets/CustomTabWidget.h
HEADERS += gui/widgets/IntervalChunksDisplay.h
HEADERS += gui/widgets/MarqueeLabel.h
HEADERS += gui/widgets/PeakMeter.h
HEADERS += gui/widgets/WavePeakPanel.h
HEADERS += gui/widgets/UserNameLineEdit.h
HEADERS += gui/BpiUtils.h
HEADERS += gui/chords/ChordLabel.h
HEADERS += gui/chords/ChordsWidget.h
HEADERS += gui/chords/ChordsPanel.h
HEADERS += gui/chords/ChordProgression.h
HEADERS += gui/chords/ChordProgressionMeasure.h
HEADERS += gui/chords/ChordsProgressionParser.h
HEADERS += gui/chords/ChatChordsProgressionParser.h
HEADERS += gui/chords/Chord.h
HEADERS += gui/LocalTrackView.h
HEADERS += gui/JamRoomViewPanel.h
HEADERS += gui/UsersColorsPool.h
HEADERS += ninjam/UserChannel.h
HEADERS += geo/IpToLocationResolver.h
HEADERS += geo/WebIpToLocationResolver.h
HEADERS += Utils.h
HEADERS += Configurator.h
HEADERS += persistence/Settings.h
HEADERS += persistence/UsersDataCache.h
HEADERS += log/Logging.h
HEADERS += UploadIntervalData.h
#HEADERS +=performance/PerformanceMonitor.h

SOURCES += MainController.cpp
SOURCES += NinjamController.cpp
SOURCES += MetronomeUtils.cpp
SOURCES += midi/MidiDriver.cpp
SOURCES += midi/MidiMessage.cpp
SOURCES += midi/MidiMessageBuffer.cpp
SOURCES += audio/core/AudioDriver.cpp
SOURCES += audio/core/AudioNode.cpp
SOURCES += audio/core/LocalInputNode.cpp
SOURCES += audio/core/LocalInputGroup.cpp
SOURCES += audio/core/AudioNodeProcessor.cpp
SOURCES += audio/core/AudioMixer.cpp
SOURCES += audio/RoomStreamerNode.cpp
SOURCES += audio/core/Plugins.cpp
SOURCES += audio/codec.cpp
SOURCES += audio/NinjamTrackNode.cpp
SOURCES += audio/MetronomeTrackNode.cpp
SOURCES += audio/core/SamplesBuffer.cpp
SOURCES += audio/SamplesBufferResampler.cpp
SOURCES += audio/vorbis/VorbisDecoder.cpp
SOURCES += audio/vorbis/VorbisEncoder.cpp
SOURCES += audio/core/AudioPeak.cpp
SOURCES += audio/Resampler.cpp
SOURCES += audio/file/FileReaderFactory.cpp
SOURCES += audio/file/WaveFileReader.cpp
SOURCES += audio/file/OggFileReader.cpp
SOURCES += recorder/JamRecorder.cpp
SOURCES += recorder/ReaperProjectGenerator.cpp
SOURCES += ninjam/Server.cpp
SOURCES += ninjam/Service.cpp
SOURCES += ninjam/User.cpp
SOURCES += ninjam/ServerMessages.cpp
SOURCES += ninjam/ClientMessages.cpp
SOURCES += ninjam/ServerMessagesHandler.cpp
SOURCES += ninjam/UserChannel.cpp
SOURCES += gui/widgets/PeakMeter.cpp
SOURCES += gui/widgets/WavePeakPanel.cpp
SOURCES += gui/LocalTrackView.cpp
SOURCES += gui/plugins/Guis.cpp
SOURCES += gui/JamRoomViewPanel.cpp
SOURCES += gui/PreferencesDialog.cpp
SOURCES += gui/PluginScanDialog.cpp
SOURCES += gui/NinjamRoomWindow.cpp
SOURCES += gui/BaseTrackView.cpp
SOURCES += gui/NinjamTrackView.cpp
SOURCES += gui/NinjamTrackGroupView.cpp
SOURCES += gui/NinjamPanel.cpp
SOURCES += gui/BusyDialog.cpp
SOURCES += gui/ChatPanel.cpp
SOURCES += gui/ChatMessagePanel.cpp
SOURCES += gui/Highligther.cpp
SOURCES += gui/TrackGroupView.cpp
SOURCES += gui/LocalTrackGroupView.cpp
SOURCES += gui/intervalProgress/IntervalProgressDisplay.cpp
SOURCES += gui/intervalProgress/IntervalProgressWindow.cpp
SOURCES += gui/intervalProgress/LinearPaintStrategy.cpp
SOURCES += gui/intervalProgress/EllipticalPaintStrategy.cpp
SOURCES += gui/intervalProgress/CircularPaintStrategy.cpp
SOURCES += gui/intervalProgress/PiePaintStrategy.cpp
SOURCES += gui/PrivateServerDialog.cpp
SOURCES += gui/UserNameDialog.cpp
SOURCES += gui/MainWindow.cpp
SOURCES += gui/widgets/CustomTabWidget.cpp
SOURCES += gui/widgets/UserNameLineEdit.cpp
SOURCES += gui/widgets/IntervalChunksDisplay.cpp
SOURCES += gui/widgets/MarqueeLabel.cpp
SOURCES += gui/BpiUtils.cpp
SOURCES += gui/chords/ChordsWidget.cpp
SOURCES += gui/chords/ChordsPanel.cpp
SOURCES += gui/chords/ChordProgression.cpp
SOURCES += gui/chords/ChordProgressionMeasure.cpp
SOURCES += gui/chords/Chord.cpp
SOURCES += gui/chords/ChordLabel.cpp
SOURCES += gui/chords/ChatChordsProgressionParser.cpp
SOURCES += gui/UsersColorsPool.cpp
SOURCES += geo/IpToLocationResolver.cpp
SOURCES += log/logging.cpp
SOURCES += geo/WebIpToLocationResolver.cpp
SOURCES += loginserver/LoginService.cpp
SOURCES += Configurator.cpp
SOURCES += persistence/UsersDataCache.cpp
SOURCES += persistence/Settings.cpp
SOURCES += UploadIntervalData.cpp

#multiplatform implementations
#win32:SOURCES += $$PWD/src/performance/WindowsPerformanceMonitor.cpp
#macx:SOURCES += $$PWD/src/performance/MacPerformanceMonitor.cpp

FORMS += gui/PreferencesDialog.ui
FORMS += gui/PluginScanDialog.ui
FORMS += gui/NinjamRoomWindow.ui
FORMS += gui/NinjamPanel.ui
FORMS += gui/BusyDialog.ui
FORMS += gui/ChatPanel.ui
FORMS += gui/ChatMessagePanel.ui
FORMS += gui/JamRoomViewPanel.ui
FORMS += gui/PrivateServerDialog.ui
FORMS += gui/UserNameDialog.ui
FORMS += gui/MainWindow.ui
FORMS += gui/chords/ChordsPanel.ui

RESOURCES += ../resources/jamtaba.qrc

#this is the file used by the Beautifier QtCreator plugin (using the Uncrustify tool)
DISTFILES += $$PWD/uncrustify.cfg
