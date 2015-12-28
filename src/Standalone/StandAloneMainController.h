#ifndef STANDALONEMAINCONTROLLER_H
#define STANDALONEMAINCONTROLLER_H

#include "MainController.h"
#include <QApplication>
#include <QTcpSocket>
#include <QProcess>
#include "audio/vst/PluginFinder.h"
#include "audio/vst/vsthost.h"

class QCoreApplication;

class MainWindowStandalone;

namespace Midi {
class MidiDriver;
}

namespace JamtabaVstPlugin {
class VstHost;
class StandalonePluginFinder;
class PluginFinder;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++
class StandalonePluginFinder : public Vst::PluginFinder
{
    Q_OBJECT
public:
    StandalonePluginFinder();
    ~StandalonePluginFinder();
    void scan(QStringList skipList);
    void cancel();

private:

    Audio::PluginDescriptor getPluginDescriptor(QFileInfo f);
    QString buildCommaSeparetedString(QStringList list) const;
    QProcess scanProcess;
    QString getVstScannerExecutablePath() const;
    QString lastScannedPlugin;// used to recover the last plugin path when the scanner process crash
    void handleProcessError(QString lastScannedPlugin);
private slots:
    void on_processStandardOutputReady();
    void on_processFinished();
    void on_processError(QProcess::ProcessError);
};
// ++++++++++++++++++++++++++++++++++++++++++

namespace Controller {
class StandaloneMainController : public MainController
{
    Q_OBJECT
public:
    StandaloneMainController(Persistence::Settings settings, QApplication *application);
    ~StandaloneMainController();

    bool isRunningAsVstPlugin() const;

    void initializePluginsList(QStringList paths);
    void scanPlugins(bool scanOnlyNewPlugins = false);

    void quit();

    void start() override;
    void stop() override;

    void updateInputTracksRange();// called when input range or method (audio or midi) are changed in preferences

    Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor);

    virtual void addDefaultPluginsScanPath();
    QStringList getSteinbergRecommendedPaths();
    bool pluginsScanIsNeeded() const; // plugins cache is empty OR we have new plugins in scan folders?

    inline Vst::Host *getVstHost() const
    {
        return vstHost;
    }

    inline virtual int getSampleRate() const override
    {
        return audioDriver->getSampleRate();
    }

    inline Audio::AudioDriver *getAudioDriver() const
    {
        return audioDriver.data();
    }

    inline Midi::MidiDriver *getMidiDriver() const
    {
        return midiDriver.data();
    }

    void stopNinjamController();

    QString getJamtabaFlavor() const override;

    void setInputTrackToMono(int localChannelIndex, int inputIndexInAudioDevice);
    void setInputTrackToStereo(int localChannelIndex, int firstInputIndex);
    void setInputTrackToMIDI(int localChannelIndex, int midiDevice, int midiChannel);// use -1 to select all channels
    void setInputTrackToNoInput(int localChannelIndex);

    bool isUsingNullAudioDriver() const;

    void useNullAudioDriver();// use when the audio driver fails

    void setMainWindow(MainWindow *mainWindow) override;

protected:
    Midi::MidiDriver *createMidiDriver();

    // TODO - Audio driver need just the audio settings to initialize, not the entire settings.
    Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);

    Vst::PluginFinder *createPluginFinder() override; //TODO use pluginFinder just in Standalone

    Controller::NinjamController *createNinjamController(MainController *) override;

    void setCSS(QString css);

    Midi::MidiBuffer pullMidiBuffer() override;

protected slots:
    void on_ninjamBpmChanged(int newBpm);
    void on_connectedInNinjamServer(Ninjam::Server server);
    void on_audioDriverSampleRateChanged(int newSampleRate);
    void on_audioDriverStarted();
    void on_audioDriverStopped();
    void on_newNinjamInterval();
    void on_ninjamStartProcessing(int intervalPosition);
    void on_VSTPluginFounded(QString name, QString group, QString path);

private slots:
    void on_vstPluginRequestedWindowResize(QString pluginName, int newWidht, int newHeight);

private:
    // VST
    Vst::Host *vstHost;// static instance released inside Vst::Host using QSCopedPointer
    QApplication *application;

    QScopedPointer<Audio::AudioDriver> audioDriver;
    QScopedPointer<Midi::MidiDriver> midiDriver;

    bool isVstPluginFile(QString file) const;

    bool inputIndexIsValid(int inputIndex);

    MainWindowStandalone *window;
};
}

#endif // STANDALONEMAINCONTROLLER_H
