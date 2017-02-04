#ifndef STANDALONEMAINCONTROLLER_H
#define STANDALONEMAINCONTROLLER_H

#include "MainController.h"
#include <QApplication>
#include "vst/VstPluginFinder.h"
#ifdef Q_OS_MAC
    #include "AU/AudioUnitPluginFinder.h"
#endif
#include "audio/Host.h"
#include "audio/core/Plugins.h"
#include "audio/core/PluginDescriptor.h"

class QCoreApplication;

class MainWindowStandalone;

namespace Midi {
class MidiDriver;
}

// ++++++++++++++++++++++++++++++++++++++++++

namespace Controller {
class MainControllerStandalone : public MainController
{
    Q_OBJECT

public:
    MainControllerStandalone(Persistence::Settings settings, QApplication *application);
    ~MainControllerStandalone();

    void initializeVstPluginsList(const QStringList &paths);

#ifdef Q_OS_MAC
    void initializeAudioUnitPluginsList(const QStringList &paths);
    void scanAudioUnitPlugins();
#endif

    void addDefaultPluginsScanPath();// add vst path from registry

    void clearPluginsList();

    void clearPluginsCache();
    QStringList getSteinbergRecommendedPaths();
    bool vstScanIsNeeded() const; // plugins cache is empty OR we have new plugins in scan folders?

    void quit();

    void start() override;
    void stop() override;

    void updateInputTracksRange();// called when input range or method (audio or midi) are changed in preferences

    inline virtual float getSampleRate() const override
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
    void setInputTrackToMIDI(int localChannelIndex, int midiDevice, int midiChannel, qint8 transpose=0, quint8 lowerNote=0, quint8 higherNote=127);// use -1 to select all channels
    void setInputTrackToNoInput(int localChannelIndex);

    bool isUsingNullAudioDriver() const;

    void useNullAudioDriver();// use when the audio driver fails

    void setMainWindow(MainWindow *mainWindow) override;

    void cancelPluginFinders();

    inline audio::VSTPluginFinder *getVstPluginFinder() const
    {
        return vstPluginFinder.data();
    }

    void removePlugin(int inputTrackIndex, Audio::Plugin *PLUGIN);
    QMap<QString, QList<Audio::PluginDescriptor> > getPluginsDescriptors(Audio::PluginDescriptor::Category category);
    Audio::Plugin *addPlugin(quint32 inputTrackIndex, quint32 pluginSlotIndex, const Audio::PluginDescriptor &descriptor);

    std::vector<Midi::MidiMessage> pullMidiMessagesFromPlugins() override;

public slots:
    void setSampleRate(int newSampleRate) override;
    void setBufferSize(int newBufferSize);

    void removePluginsScanPath(const QString &path);
    void addPluginsScanPath(const QString &path);

    void addBlackVstToSettings(const QString &path);
    void removeBlackVstFromSettings(const QString &pluginPath);

    void scanAllVstPlugins();
    void scanOnlyNewVstPlugins();

    void openExternalAudioControlPanel();

protected:
    Midi::MidiDriver *createMidiDriver();

    // TODO - Audio driver need just the audio settings to initialize, not the entire settings.
    Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);

    Controller::NinjamController *createNinjamController() override;

    void setCSS(const QString &css) override;

    void setupNinjamControllerSignals() override;

    std::vector<Midi::MidiMessage> pullMidiMessagesFromDevices() override;

protected slots:
    void updateBpm(int newBpm) override;
    void connectedNinjamServer(const Ninjam::Server &server) override;


    void on_newNinjamInterval() override;

    //TODO After the big refatoration these 3 slots can be private slots
    void on_audioDriverStopped();
    void on_audioDriverStarted();
    void on_ninjamStartProcessing(int intervalPosition) ;

    void addFoundedVstPlugin(const QString &name, const QString &path);
#ifdef Q_OS_MAC
    void addFoundedAudioUnitPlugin(const QString &name, const QString &path);
#endif

private slots:
    void setVstPluginWindowSize(QString pluginName, int newWidht, int newHeight);

private:
    // VST and AU hosts
    QList<Host *> hosts;
    QApplication *application;

    QScopedPointer<Audio::AudioDriver> audioDriver;
    QScopedPointer<Midi::MidiDriver> midiDriver;

    QList<Audio::PluginDescriptor> pluginsDescriptors;

    MainWindowStandalone *window;

    bool inputIndexIsValid(int inputIndex);

    QScopedPointer<audio::VSTPluginFinder> vstPluginFinder;
#ifdef Q_OS_MAC
    QScopedPointer<audio::AudioUnitPluginFinder> auPluginFinder;
 #endif

    // used to sort plugins list
    static bool pluginDescriptorLessThan(const Audio::PluginDescriptor &d1,
                                         const Audio::PluginDescriptor &d2);

    Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor);

    void scanVstPlugins(bool scanOnlyNewVstPlugins);


};
}

#endif // STANDALONEMAINCONTROLLER_H
