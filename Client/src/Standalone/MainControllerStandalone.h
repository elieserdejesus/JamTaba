#ifndef STANDALONEMAINCONTROLLER_H
#define STANDALONEMAINCONTROLLER_H

#include "MainController.h"
#include <QApplication>
#include "vst/PluginFinder.h"
#include "vst/vsthost.h"
#include "audio/core/Plugins.h"
#include "audio/core/PluginDescriptor.h"

class QCoreApplication;

class MainWindowStandalone;

namespace Midi {
class MidiDriver;
}

namespace JamtabaVstPlugin {
class VstHost;
class PluginFinder;
}

// ++++++++++++++++++++++++++++++++++++++++++

namespace Controller {
class MainControllerStandalone : public MainController
{
    Q_OBJECT

public:
    MainControllerStandalone(Persistence::Settings settings, QApplication *application);
    ~MainControllerStandalone();

    void initializePluginsList(QStringList paths);
    void scanPlugins(bool scanOnlyNewPlugins = false);
    void addPluginsScanPath(QString path);
    void addBlackVstToSettings(QString path);
    void addDefaultPluginsScanPath();// add vst path from registry
    void removePluginsScanPath(QString path);
    void removeBlackVst(int index);
    void clearPluginsCache();
    QStringList getSteinbergRecommendedPaths();
    bool pluginsScanIsNeeded() const; // plugins cache is empty OR we have new plugins in scan folders?

    void quit();

    void start() override;
    void stop() override;

    void updateInputTracksRange();// called when input range or method (audio or midi) are changed in preferences

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

    void cancelPluginFinder();

    inline Vst::PluginFinder *getPluginFinder() const
    {
        return pluginFinder.data();
    }

    void removePlugin(int inputTrackIndex, Audio::Plugin *plugin);
    QList<Audio::PluginDescriptor> getPluginsDescriptors();
    Audio::Plugin *addPlugin(int inputTrackIndex, const Audio::PluginDescriptor &descriptor);

public slots:
    void setSampleRate(int newSampleRate) override;

protected:
    Midi::MidiDriver *createMidiDriver();

    // TODO - Audio driver need just the audio settings to initialize, not the entire settings.
    Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);

    Controller::NinjamController *createNinjamController() override;

    void setCSS(const QString &css) override;

    Midi::MidiBuffer pullMidiBuffer() override;

    void setupNinjamControllerSignals() override;

protected slots:
    void updateBpm(int newBpm) override;
    void connectedNinjamServer(const Ninjam::Server &server) override;


    void on_newNinjamInterval() override;

    //TODO After the big refatoration these 3 slots can be private slots
    void on_audioDriverStopped();
    void on_audioDriverStarted();
    void on_ninjamStartProcessing(int intervalPosition) ;

    void on_VSTPluginFounded(QString name, QString group, QString path);

private slots:
    void on_vstPluginRequestedWindowResize(QString pluginName, int newWidht, int newHeight);

private:
    // VST
    Vst::Host *vstHost;// static instance released inside Vst::Host using QSCopedPointer
    QApplication *application;

    QScopedPointer<Audio::AudioDriver> audioDriver;
    QScopedPointer<Midi::MidiDriver> midiDriver;

    QList<Audio::PluginDescriptor> pluginsDescriptors;

    MainWindowStandalone *window;

    bool isVstPluginFile(QString file) const;

    bool inputIndexIsValid(int inputIndex);

    QScopedPointer<Vst::PluginFinder> pluginFinder;

    Vst::PluginFinder *createPluginFinder();

    // used to sort plugins list
    static bool pluginDescriptorLessThan(const Audio::PluginDescriptor &d1,
                                         const Audio::PluginDescriptor &d2);

    Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor);

};
}

#endif // STANDALONEMAINCONTROLLER_H
