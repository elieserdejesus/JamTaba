#ifndef STANDALONEMAINCONTROLLER_H
#define STANDALONEMAINCONTROLLER_H

#include "MainController.h"
#include <QApplication>
#include <QTcpSocket>
#include <QProcess>
#include "audio/vst/PluginFinder.h"
#include "audio/vst/vsthost.h"

class QCoreApplication;

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

    Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor);

    virtual void addDefaultPluginsScanPath();
    QStringList getSteinbergRecommendedPaths();
    bool pluginsScanIsNeeded() const; // plugins cache is empty OR we have new plugins in scan folders?

    inline Vst::Host *getVstHost() const
    {
        return vstHost;
    }

    void stopNinjamController();

    void start();

protected:
    virtual Midi::MidiDriver *createMidiDriver();
    virtual Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);
    virtual Vst::PluginFinder *createPluginFinder();
    virtual Controller::NinjamController *createNinjamController(MainController *);

    void setCSS(QString css);

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

    bool isVstPluginFile(QString file) const;
};
}

#endif // STANDALONEMAINCONTROLLER_H
