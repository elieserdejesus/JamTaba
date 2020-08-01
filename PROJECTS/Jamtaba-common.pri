QT += core gui network websockets widgets concurrent multimedia multimediawidgets

mac:LIBS += -dead_strip

ROOT_PATH = "$$PWD/.."
SOURCE_PATH = "$$ROOT_PATH/src"
VST_SDK_PATH = "$$ROOT_PATH/VST_SDK"

INCLUDEPATH += $$SOURCE_PATH/Common

INCLUDEPATH += $$ROOT_PATH/libs/includes/ogg
INCLUDEPATH += $$ROOT_PATH/libs/includes/vorbis
INCLUDEPATH += $$ROOT_PATH/libs/includes/minimp3
INCLUDEPATH += $$ROOT_PATH/libs/includes/ffmpeg
INCLUDEPATH += $$ROOT_PATH/libs/includes/miniupnp

DEFINES += MINIUPNP_STATICLIB

win32:INCLUDEPATH += $$ROOT_PATH/libs/includes/stackwalker

VPATH += $$SOURCE_PATH/Common

DEFINES += VST_FORCE_DEPRECATED=0#enable VST 2.3 features
DEFINES += OV_EXCLUDE_STATIC_CALLBACKS  #avoid ogg static callback warnings

linux{ #avoid errors in VST SDK when compiling in Linux
    DEFINES += __cdecl=""
    QMAKE_CXXFLAGS += -D__LINUX_ALSA__
    QMAKE_CFLAGS_APP += -fPIC
    QMAKE_LFLAGS = -no-pie
}

macx{
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++ -D__MACOSX_CORE__
    LIBS += -mmacosx-version-min=10.7 -stdlib=libc++
}

win32 {
    QMAKE_LFLAGS_RELEASE += /DEBUG # releasing with debug symbols
    QMAKE_CXXFLAGS += -D__WINDOWS_MM__
}

CONFIG += c++11


PRECOMPILED_HEADER += PreCompiledHeaders.h

HEADERS += midi/MidiDriver.h
HEADERS += midi/MidiMessage.h
HEADERS += looper/Looper.h
HEADERS += looper/LooperLayer.h
HEADERS += looper/LooperStates.h
HEADERS += looper/LooperPersistence.h
HEADERS += audio/core/AudioDriver.h
HEADERS += audio/core/AudioNode.h
HEADERS += audio/core/LocalInputNode.h
HEADERS += audio/core/LocalInputGroup.h
HEADERS += audio/core/AudioNodeProcessor.h
HEADERS += audio/core/AudioMixer.h
HEADERS += audio/core/SamplesBuffer.h
HEADERS += audio/core/AudioPeak.h
HEADERS += audio/core/Plugins.h
HEADERS += audio/core/Filters.h
HEADERS += audio/core/PluginDescriptor.h
HEADERS += audio/Encoder.h
HEADERS += audio/vorbis/VorbisDecoder.h
HEADERS += audio/vorbis/VorbisEncoder.h
HEADERS += audio/RoomStreamerNode.h
HEADERS += audio/NinjamTrackNode.h
HEADERS += audio/MetronomeTrackNode.h
HEADERS += audio/MidiSyncTrackNode.h
HEADERS += audio/SamplesBufferResampler.h
HEADERS += audio/SamplesBufferRecorder.h
HEADERS += audio/Mp3Decoder.h
HEADERS += audio/Resampler.h
HEADERS += video/FFMpegMuxer.h
HEADERS += video/FFMpegDemuxer.h
HEADERS += video/VideoFrameGrabber.h
HEADERS += video/VideoWidget.h
HEADERS += file/FileReader.h
HEADERS += file/FileReaderFactory.h
HEADERS += file/WaveFileReader.h
HEADERS += file/WaveFileWriter.h
HEADERS += file/OggFileReader.h
HEADERS += file/Mp3FileReader.h
HEADERS += file/FileUtils.h
HEADERS += recorder/JamRecorder.h
HEADERS += recorder/ReaperProjectGenerator.h
HEADERS += recorder/ClipSortLogGenerator.h
HEADERS += loginserver/LoginService.h
HEADERS += loginserver/Version.h
HEADERS += loginserver/MainChat.h
HEADERS += loginserver/natmap.h
HEADERS += MainController.h
HEADERS += NinjamController.h
HEADERS += MetronomeUtils.h
HEADERS += ninjam/client/User.h
HEADERS += ninjam/client/UserChannel.h
HEADERS += ninjam/client/Service.h
HEADERS += ninjam/client/ServerInfo.h
HEADERS += ninjam/client/ServerMessages.h
HEADERS += ninjam/client/ClientMessages.h
HEADERS += ninjam/client/ServerMessagesHandler.h
HEADERS += ninjam/server/Server.h
HEADERS += gui/plugins/Guis.h
HEADERS += gui/PluginScanDialog.h
HEADERS += gui/PreferencesDialog.h
HEADERS += gui/LooperWindow.h
HEADERS += gui/widgets/LooperWavePanel.h
HEADERS += gui/NinjamRoomWindow.h
HEADERS += gui/BaseTrackView.h
HEADERS += gui/NinjamTrackView.h
HEADERS += gui/NinjamTrackGroupView.h
HEADERS += gui/NinjamPanel.h
HEADERS += gui/MetronomePanel.h
HEADERS += gui/BusyDialog.h
HEADERS += gui/chat/ChatPanel.h
HEADERS += gui/chat/ChatMessagePanel.h
HEADERS += gui/chat/NinjamChatMessageParser.h
HEADERS += gui/chat/ChatTextEditor.h
HEADERS += gui/chat/EmojiWidget.h
HEADERS += gui/chat/EmojiManager.h
HEADERS += gui/screensaver/ScreensaverBlocker.h
HEADERS += gui/Highligther.h
HEADERS += gui/InactivityDetector.h
HEADERS += gui/TrackGroupView.h
HEADERS += gui/LocalTrackGroupView.h
HEADERS += gui/intervalProgress/IntervalProgressDisplay.h
HEADERS += gui/intervalProgress/IntervalProgressWindow.h
HEADERS += gui/PrivateServerDialog.h
HEADERS += gui/PrivateServerWindow.h
HEADERS += gui/UserNameDialog.h
HEADERS += gui/MainWindow.h
HEADERS += gui/IconFactory.h
HEADERS += gui/widgets/IntervalChunksDisplay.h
HEADERS += gui/widgets/MarqueeLabel.h
HEADERS += gui/widgets/PeakMeter.h
HEADERS += gui/widgets/WavePeakPanel.h
HEADERS += gui/widgets/UserNameLineEdit.h
HEADERS += gui/widgets/MapWidget.h
HEADERS += gui/widgets/MapMarker.h
HEADERS += gui/widgets/MultiStateButton.h
HEADERS += gui/widgets/BlinkableButton.h
HEADERS += gui/widgets/Slider.h
HEADERS += gui/widgets/BoostSpinBox.h
HEADERS += gui/widgets/ChatTabWidget.h
HEADERS += gui/widgets/InstrumentsMenu.h
HEADERS += gui/BpiUtils.h
HEADERS += gui/chords/ChordLabel.h
HEADERS += gui/chords/ChordsPanel.h
HEADERS += gui/chords/ChordProgression.h
HEADERS += gui/chords/ChordProgressionMeasure.h
HEADERS += gui/chords/ChordsProgressionParser.h
HEADERS += gui/chords/ChordProgressionCreationDialog.h
HEADERS += gui/chords/ChatChordsProgressionParser.h
HEADERS += gui/chords/Chord.h
HEADERS += gui/LocalTrackView.h
HEADERS += gui/JamRoomViewPanel.h
HEADERS += gui/UsersColorsPool.h
HEADERS += gui/GuiUtils.h
HEADERS += gui/ThemeLoader.h
HEADERS += Utils.h
HEADERS += Configurator.h
HEADERS += persistence/Settings.h
HEADERS += persistence/UsersDataCache.h
HEADERS += persistence/CacheHeader.h
HEADERS += log/Logging.h
HEADERS += UploadIntervalData.h
HEADERS += performance/PerformanceMonitor.h
HEADERS += upnp/UPnPManager.h

SOURCES += MainController.cpp
SOURCES += NinjamController.cpp
SOURCES += MetronomeUtils.cpp
SOURCES += midi/MidiDriver.cpp
SOURCES += midi/MidiMessage.cpp
SOURCES += looper/Looper.cpp
SOURCES += looper/LooperLayer.cpp
SOURCES += looper/LooperStates.cpp
SOURCES += file/WaveFileWriter.cpp
SOURCES += looper/LooperPersistence.cpp
SOURCES += audio/core/AudioDriver.cpp
SOURCES += audio/core/AudioNode.cpp
SOURCES += audio/core/LocalInputNode.cpp
SOURCES += audio/core/LocalInputGroup.cpp
SOURCES += audio/core/AudioNodeProcessor.cpp
SOURCES += audio/core/AudioMixer.cpp
SOURCES += audio/core/Filters.cpp
SOURCES += audio/RoomStreamerNode.cpp
SOURCES += audio/core/Plugins.cpp
SOURCES += audio/Mp3Decoder.cpp
SOURCES += audio/NinjamTrackNode.cpp
SOURCES += audio/MetronomeTrackNode.cpp
SOURCES += audio/MidiSyncTrackNode.cpp
SOURCES += audio/core/SamplesBuffer.cpp
SOURCES += audio/core/PluginDescriptor.cpp
SOURCES += audio/SamplesBufferResampler.cpp
SOURCES += audio/vorbis/VorbisDecoder.cpp
SOURCES += audio/vorbis/VorbisEncoder.cpp
SOURCES += audio/core/AudioPeak.cpp
SOURCES += audio/Resampler.cpp
SOURCES += video/FFMpegMuxer.cpp
SOURCES += video/FFMpegDemuxer.cpp
SOURCES += video/VideoFrameGrabber.cpp
SOURCES += video/VideoWidget.cpp
SOURCES += file/FileReaderFactory.cpp
SOURCES += file/WaveFileReader.cpp
SOURCES += file/OggFileReader.cpp
SOURCES += file/Mp3FileReader.cpp
SOURCES += file/FileUtils.cpp
SOURCES += recorder/JamRecorder.cpp
SOURCES += recorder/ReaperProjectGenerator.cpp
SOURCES += recorder/ClipSortLogGenerator.cpp
SOURCES += ninjam/Ninjam.cpp
SOURCES += ninjam/client/ServerInfo.cpp
SOURCES += ninjam/client/Service.cpp
SOURCES += ninjam/client/User.cpp
SOURCES += ninjam/client/ServerMessages.cpp
SOURCES += ninjam/client/ClientMessages.cpp
SOURCES += ninjam/client/ServerMessagesHandler.cpp
SOURCES += ninjam/client/UserChannel.cpp
SOURCES += ninjam/server/Server.cpp
SOURCES += gui/widgets/PeakMeter.cpp
SOURCES += gui/widgets/WavePeakPanel.cpp
SOURCES += gui/widgets/ChatTabWidget.cpp
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
SOURCES += gui/MetronomePanel.cpp
SOURCES += gui/BusyDialog.cpp
SOURCES += gui/IconFactory.cpp
SOURCES += gui/InactivityDetector.cpp
SOURCES += gui/LooperWindow.cpp
SOURCES += gui/widgets/LooperWavePanel.cpp
SOURCES += gui/chat/ChatPanel.cpp
SOURCES += gui/chat/ChatMessagePanel.cpp
SOURCES += gui/chat/ChatTextEditor.cpp
SOURCES += gui/chat/EmojiWidget.cpp
SOURCES += gui/chat/EmojiManager.cpp
SOURCES += gui/chat/NinjamChatMessageParser.cpp
win32:SOURCES += gui/screensaver/WindowsScreensaverBlocker.cpp
linux:SOURCES += gui/screensaver/LinuxScreensaverBlocker.cpp
OBJECTIVE_SOURCES += gui/screensaver/MacScreensaverBlocker.mm
win32:SOURCES += log/stackwalker/WindowsStackWalker.cpp

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
SOURCES += gui/PrivateServerWindow.cpp
SOURCES += gui/UserNameDialog.cpp
SOURCES += gui/MainWindow.cpp
SOURCES += gui/widgets/UserNameLineEdit.cpp
SOURCES += gui/widgets/IntervalChunksDisplay.cpp
SOURCES += gui/widgets/MarqueeLabel.cpp
SOURCES += gui/widgets/MapMarker.cpp
SOURCES += gui/widgets/MapWidget.cpp
SOURCES += gui/widgets/MultiStateButton.cpp
SOURCES += gui/widgets/BlinkableButton.cpp
SOURCES += gui/widgets/BoostSpinBox.cpp
SOURCES += gui/widgets/Slider.cpp
SOURCES += gui/widgets/InstrumentsMenu.cpp
SOURCES += gui/BpiUtils.cpp
SOURCES += gui/chords/ChordsPanel.cpp
SOURCES += gui/chords/ChordProgression.cpp
SOURCES += gui/chords/ChordProgressionMeasure.cpp
SOURCES += gui/chords/ChordProgressionCreationDialog.cpp
SOURCES += gui/chords/Chord.cpp
SOURCES += gui/chords/ChordLabel.cpp
SOURCES += gui/chords/ChatChordsProgressionParser.cpp
SOURCES += gui/UsersColorsPool.cpp
SOURCES += gui/GuiUtils.cpp
SOURCES += gui/ThemeLoader.cpp
SOURCES += log/logging.cpp
SOURCES += loginserver/LoginService.cpp
SOURCES += loginserver/Version.cpp
SOURCES += loginserver/MainChat.cpp
SOURCES += Configurator.cpp
SOURCES += persistence/UsersDataCache.cpp
SOURCES += persistence/Settings.cpp
SOURCES += persistence/CacheHeader.cpp
SOURCES += UploadIntervalData.cpp
SOURCES += upnp/UPnPManager.cpp

#multiplatform implementations
win32:SOURCES += performance/WindowsPerformanceMonitor.cpp
macx:SOURCES += performance/MacPerformanceMonitor.cpp
linux:SOURCES += performance/LinuxPerformanceMonitor.cpp

FORMS += gui/PreferencesDialog.ui
FORMS += gui/LooperWindow.ui
FORMS += gui/PluginScanDialog.ui
FORMS += gui/NinjamRoomWindow.ui
FORMS += gui/NinjamPanel.ui
FORMS += gui/MetronomePanel.ui
FORMS += gui/BusyDialog.ui
FORMS += gui/chat/ChatPanel.ui
FORMS += gui/chat/ChatMessagePanel.ui
FORMS += gui/JamRoomViewPanel.ui
FORMS += gui/PrivateServerDialog.ui
FORMS += gui/PrivateServerWindow.ui
FORMS += gui/UserNameDialog.ui
FORMS += gui/MainWindow.ui
FORMS += gui/chords/ChordsPanel.ui
FORMS += gui/chords/ChordProgressionCreationDialog.ui

RESOURCES += ../resources/jamtaba.qrc

#this is the file used by the Beautifier QtCreator plugin (using the Uncrustify tool)
DISTFILES += $$PWD/uncrustify.cfg
