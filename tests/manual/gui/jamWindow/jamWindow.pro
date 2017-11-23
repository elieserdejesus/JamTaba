QT += core gui widgets multimedia multimediawidgets

CONFIG += testcase
TEMPLATE = app
TARGET = jamWindow

INCLUDEPATH += .
INCLUDEPATH += ../../../../src
INCLUDEPATH += ../../../../src/Common
INCLUDEPATH += ../../../../src/Common/gui
INCLUDEPATH += ../../../../src/Common/gui/widgets
INCLUDEPATH += ../../../../src/Common/gui/chords
INCLUDEPATH += ../../../../src/Common/gui/screensaver
INCLUDEPATH += ../../../../src/Standalone
INCLUDEPATH += ../../../../src/Standalone/audio
INCLUDEPATH += ../../../../src/Standalone/vst

INCLUDEPATH += ../../../../libs/includes/ffmpeg
INCLUDEPATH += ../../../../libs/includes/vorbis
INCLUDEPATH += ../../../../libs/includes/ogg
INCLUDEPATH += ../../../../libs/includes/portaudio
INCLUDEPATH += ../../../../libs/includes/rtmidi
INCLUDEPATH += ../../../../libs/includes/minimp3
INCLUDEPATH += ../../../../libs/includes/stackwalker

INCLUDEPATH += ../../../../VST_SDK/pluginterfaces/vst2.x

VPATH += ../../../../src/

FORMS += Common/gui/PreferencesDialog.ui
FORMS += Common/gui/LooperWindow.ui
FORMS += Common/gui/PluginScanDialog.ui
FORMS += Common/gui/NinjamRoomWindow.ui
FORMS += Common/gui/NinjamPanel.ui
FORMS += Standalone/gui/MidiToolsDialog.ui
FORMS += Common/gui/BusyDialog.ui
FORMS += Common/gui/chat/ChatPanel.ui
FORMS += Common/gui/chat/ChatMessagePanel.ui
FORMS += Common/gui/JamRoomViewPanel.ui
FORMS += Common/gui/PrivateServerDialog.ui
FORMS += Common/gui/UserNameDialog.ui
FORMS += Common/gui/MainWindow.ui
FORMS += Common/gui/chords/ChordsPanel.ui
FORMS += Common/gui/CrashReportDialog.ui

RESOURCES += resources/jamtaba.qrc

SOURCES += jamWindow.cpp

HEADERS += Common/audio/RoomStreamerNode.h

HEADERS += Common/persistence/Settings.h
HEADERS += Common/persistence/UsersDataCache.h

HEADERS += Common/audio/vorbis/VorbisEncoder.h
HEADERS += Common/gui/BaseTrackView.h
HEADERS += Common/gui/BusyDialog.h
HEADERS += Common/gui/NinjamPanel.h
HEADERS += Common/gui/PreferencesDialog.h
HEADERS += Common/gui/PrivateServerDialog.h
HEADERS += Common/gui/MainWindow.h
HEADERS += Common/gui/NinjamRoomWindow.h
HEADERS += Common/gui/NinjamTrackView.h
HEADERS += Common/gui/JamRoomViewPanel.h
HEADERS += Common/gui/LocalTrackView.h
HEADERS += Common/gui/LocalTrackGroupView.h
HEADERS += Common/gui/TrackGroupView.h
HEADERS += Common/gui/NinjamTrackGroupView.h
HEADERS += Common/gui/LooperWindow.h
HEADERS += Common/gui/UserNameDialog.h
HEADERS += Common/gui/PluginScanDialog.h
HEADERS += Common/gui/CrashReportDialog.h

HEADERS += Common/gui/widgets/MultiStateButton.h
HEADERS += Common/gui/widgets/LooperWavePanel.h
HEADERS += Common/gui/widgets/BaseMeter.h
HEADERS += Common/gui/widgets/IntervalChunksDisplay.h
HEADERS += Common/gui/widgets/PeakMeter.h
HEADERS += Common/gui/widgets/WavePeakPanel.h
HEADERS += Common/gui/widgets/MarqueeLabel.h
HEADERS += Common/gui/widgets/BlinkableButton.h
HEADERS += Common/gui/widgets/Slider.h
HEADERS += Common/gui/widgets/UserNameLineEdit.h
HEADERS += Common/gui/widgets/MapWidget.h
HEADERS += Common/gui/widgets/BoostSpinBox.h

HEADERS += Common/gui/chords/ChordProgression.h
HEADERS += Common/gui/chords/ChatChordProgressionParser.h
HEADERS += Common/gui/chords/ChordsPanel.h
HEADERS += Common/gui/chords/ChordLabel.h

HEADERS += Common/vst/VstHost.h
HEADERS += Standalone/audio/Host.h
HEADERS += Common/audio/core/AudioDriver.h
HEADERS += Common/audio/core/Plugins.h
HEADERS += Common/audio/core/LocalInputNode.h
HEADERS += Common/audio/core/AudioNode.h
HEADERS += Common/video/FFMpegMuxer.h
HEADERS += Common/video/FFMpegDemuxer.h
HEADERS += Common/video/VideoWidget.h
HEADERS += Common/video/VideoFrameGrabber.h

HEADERS += Common/MainController.h
HEADERS += Common/NinjamController.h
HEADERS += Common/loginserver/LoginService.h
HEADERS += Common/geo/IpToLocationResolver.h
HEADERS += Common/geo/WebIpToLocationResolver.h
HEADERS += Common/ninjam/Service.h
HEADERS += Common/looper/Looper.h

HEADERS += Standalone/MainControllerStandalone.h
HEADERS += Standalone/PluginFinder.h
HEADERS += Standalone/vst/VstPluginFinder.h
HEADERS += Standalone/vst/VstPlugin.h
HEADERS += Standalone/audio/PortAudioDriver.h
HEADERS += Standalone/gui/LocalTrackViewStandalone.h
HEADERS += Standalone/gui/LocalTrackGroupViewStandalone.h
HEADERS += Standalone/gui/MainWindowStandalone.h
HEADERS += Standalone/gui/PreferencesDialogStandalone.h
HEADERS += Standalone/gui/MidiToolsDialog.h
HEADERS += Standalone/gui/ScanFolderPanel.h
HEADERS += Standalone/gui/FxPanel.h
HEADERS += Standalone/gui/FxPanelItem.h
HEADERS += Common/gui/Highligther.h

HEADERS += Common/gui/intervalProgress/IntervalProgressDisplay.h
HEADERS += Common/gui/intervalProgress/IntervalProgressWindow.h

HEADERS += Common/gui/chat/ChatPanel.h
HEADERS += Common/gui/chat/ChatMessagePanel.h


SOURCES += Standalone/ConfiguratorStandalone.cpp
SOURCES += Standalone/PluginFinder.cpp
SOURCES += Standalone/vst/VstPluginFinder.cpp
SOURCES += Standalone/vst/VstPlugin.cpp
SOURCES += Standalone/vst/WindowsVstPluginChecker.cpp

SOURCES += Standalone/audio/PortAudioDriver.cpp
SOURCES += Standalone/audio/WindowsPortAudioDriver.cpp
SOURCES += Standalone/MainControllerStandalone.cpp

SOURCES += Standalone/gui/MainWindowStandalone.cpp
SOURCES += Standalone/gui/LocalTrackViewStandalone.cpp
SOURCES += Standalone/gui/LocalTrackGroupViewStandalone.cpp
SOURCES += Standalone/gui/PreferencesDialogStandalone.cpp
SOURCES += Standalone/gui/MidiToolsDialog.cpp
SOURCES += Standalone/gui/FxPanel.cpp
SOURCES += Standalone/gui/FxPanelItem.cpp
SOURCES += Standalone/gui/ScanFolderPanel.cpp

SOURCES += Common/Configurator.cpp
SOURCES += Common/UploadIntervalData.cpp
SOURCES += Common/MetronomeUtils.cpp

SOURCES += Common/vst/VstLoader.cpp
SOURCES += Common/vst/Utils.cpp
SOURCES += Common/vst/VstHost.cpp

SOURCES += Common/video/FFMpegMuxer.cpp
SOURCES += Common/video/FFMpegDemuxer.cpp
SOURCES += Common/video/VideoWidget.cpp
SOURCES += Common/video/VideoFrameGrabber.cpp

SOURCES += Common/looper/Looper.cpp
SOURCES += Common/looper/LooperStates.cpp
SOURCES += Common/looper/LooperLayer.cpp
SOURCES += Common/looper/LooperPersistence.cpp

SOURCES += Common/file/FileReaderFactory.cpp
SOURCES += Common/file/WaveFileReader.cpp
SOURCES += Common/file/WaveFileWriter.cpp
SOURCES += Common/file/OggFileReader.cpp
SOURCES += Common/file/Mp3FileReader.cpp
SOURCES += Common/file/FileUtils.cpp

SOURCES += Common/recorder/JamRecorder.cpp
SOURCES += Common/recorder/ReaperProjectGenerator.cpp
SOURCES += Common/recorder/ClipSortLogGenerator.cpp

SOURCES += Common/geo/IpToLocationResolver.cpp
SOURCES += Common/geo/WebIpToLocationResolver.cpp

SOURCES += Common/persistence/Settings.cpp
SOURCES += Common/persistence/UsersDataCache.cpp
SOURCES += Common/persistence/CacheHeader.cpp

SOURCES += Common/loginserver/LoginService.cpp

SOURCES += Common/MainController.cpp
SOURCES += Common/NinjamController.cpp

SOURCES += Common/performance/WindowsPerformanceMonitor.cpp

SOURCES += Common/gui/screensaver/WindowsScreensaverBlocker.cpp
SOURCES += Common/gui/BusyDialog.cpp
SOURCES += Common/gui/GuiUtils.cpp
SOURCES += Common/gui/LooperWindow.cpp
SOURCES += Common/gui/NinjamRoomWindow.cpp
SOURCES += Common/gui/PrivateServerDialog.cpp
SOURCES += Common/gui/LocalTrackGroupView.cpp
SOURCES += Common/gui/UserNameDialog.cpp
SOURCES += Common/gui/TrackGroupView.cpp
SOURCES += Common/gui/JamRoomViewPanel.cpp
SOURCES += Common/gui/NinjamTrackGroupView.cpp
SOURCES += Common/gui/MainWindow.cpp
SOURCES += Common/gui/PluginScanDialog.cpp
SOURCES += Common/gui/BaseTrackView.cpp
SOURCES += Common/gui/LocalTrackView.cpp
SOURCES += Common/gui/NinjamTrackView.cpp
SOURCES += Common/gui/widgets/MultiStateButton.cpp
SOURCES += Common/gui/widgets/IntervalChunksDisplay.cpp
SOURCES += Common/gui/widgets/PeakMeter.cpp
SOURCES += Common/gui/widgets/MapMarker.cpp
SOURCES += Common/gui/widgets/WavePeakPanel.cpp
SOURCES += Common/gui/widgets/MarqueeLabel.cpp
SOURCES += Common/gui/widgets/CustomTabWidget.cpp
SOURCES += Common/gui/widgets/Slider.cpp
SOURCES += Common/gui/widgets/UserNameLineEdit.cpp
SOURCES += Common/gui/widgets/BlinkableButton.cpp
SOURCES += Common/gui/widgets/LooperWavePanel.cpp
SOURCES += Common/gui/widgets/MapWidget.cpp
SOURCES += Common/gui/widgets/BoostSpinBox.cpp
SOURCES += Common/gui/PreferencesDialog.cpp
SOURCES += Common/gui/ThemeLoader.cpp
SOURCES += Common/gui/Highligther.cpp
SOURCES += Common/gui/chords/ChordProgression.cpp
SOURCES += Common/gui/chords/ChordProgressionMeasure.cpp
SOURCES += Common/gui/chords/ChatChordsProgressionParser.cpp
SOURCES += Common/gui/chords/ChordsPanel.cpp
SOURCES += Common/gui/chords/Chord.cpp
SOURCES += Common/gui/chords/ChordLabel.cpp
SOURCES += Common/gui/UsersColorsPool.cpp
SOURCES += Common/gui/BpiUtils.cpp

SOURCES += Common/gui/chat/ChatPanel.cpp
SOURCES += Common/gui/chat/ChatTextEditor.cpp
SOURCES += Common/gui/chat/ChatMessagePanel.cpp
SOURCES += Common/gui/chat/NinjamVotingMessageParser.cpp
SOURCES += Common/gui/CrashReportDialog.cpp


SOURCES += Common/log/logging.cpp

SOURCES += Common/audio/core/PluginDescriptor.cpp
SOURCES += Common/audio/core/AudioDriver.cpp
SOURCES += Common/audio/core/SamplesBuffer.cpp
SOURCES += Common/audio/core/Filters.cpp
SOURCES += Common/audio/SamplesBufferResampler.cpp
SOURCES += Common/audio/core/AudioNode.cpp
SOURCES += Common/audio/core/LocalInputGroup.cpp
SOURCES += Common/audio/core/AudioPeak.cpp
SOURCES += Common/audio/core/AudioMixer.cpp
SOURCES += Common/audio/core/LocalInputNode.cpp
SOURCES += Common/audio/core/AudioNodeProcessor.cpp
SOURCES += Common/audio/core/Plugins.cpp

SOURCES += Common/audio/MetronomeTrackNode.cpp
SOURCES += Common/audio/NinjamTrackNode.cpp
SOURCES += Common/audio/Resampler.cpp
SOURCES += Common/audio/Mp3Decoder.cpp
SOURCES += Common/audio/RoomStreamerNode.cpp

SOURCES += Common/midi/MidiDriver.cpp
SOURCES += Common/midi/RtMidiDriver.cpp
SOURCES += Common/midi/MidiMessage.cpp
SOURCES += Common/audio/vorbis/VorbisEncoder.cpp
SOURCES += Common/audio/vorbis/VorbisDecoder.cpp

SOURCES += Common/ninjam/Service.cpp
SOURCES += Common/ninjam/ServerMessages.cpp
SOURCES += Common/ninjam/ServerMessagesHandler.cpp
SOURCES += Common/ninjam/ClientMessages.cpp
SOURCES += Common/ninjam/UserChannel.cpp
SOURCES += Common/ninjam/User.cpp
SOURCES += Common/ninjam/Server.cpp
SOURCES += Common/gui/NinjamPanel.cpp

SOURCES += Common/gui/intervalProgress/IntervalProgressDisplay.cpp
SOURCES += Common/gui/intervalProgress/IntervalProgressWindow.cpp
SOURCES += Common/gui/intervalProgress/LinearPaintStrategy.cpp
SOURCES += Common/gui/intervalProgress/EllipticalPaintStrategy.cpp
SOURCES += Common/gui/intervalProgress/PiePaintStrategy.cpp
SOURCES += Common/gui/intervalProgress/CircularPaintStrategy.cpp

win32:SOURCES += Common/log/stackwalker/WindowsStackWalker.cpp

LIBS_PATH = "static/win64-msvc"

CONFIG(release, debug|release):    LIBS += -L$$LIBS_PATH -lportaudio -lminimp3 -lrtmidi -lvorbisfile -lvorbis -logg -lavcodec -lavutil -lavformat -lswscale -lswresample -lstackwalker
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../libs/$$LIBS_PATH/ -lportaudiod -lminimp3d -lrtmidid -lvorbisfiled -lvorbisd -loggd -lavcodecd -lavutild -lavformatd -lswscaled -lswresampled -lstackwalkerd

DEFINES += VST_FORCE_DEPRECATED=0 # enable VST 2.3 features

QMAKE_LFLAGS += "/NODEFAULTLIB:libcmt"
