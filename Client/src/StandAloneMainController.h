#ifndef STANDALONEMAINCONTROLLER_H
#define STANDALONEMAINCONTROLLER_H

#include "MainController.h"
#include <QApplication>
#include "audio/vst/PluginFinder.h"

namespace JamtabaVstPlugin {
    class VstHost;
    class StandalonePluginFinder;
    class PluginFinder;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
class StandalonePluginFinder : public Vst::PluginFinder
{

public:
    StandalonePluginFinder(Vst::Host *host);
    ~StandalonePluginFinder();
    void scan();
private:
    void run();

private:
    Audio::PluginDescriptor getPluginDescriptor(QFileInfo f, Vst::Host *host);
    Vst::Host* host;
};
//++++++++++++++++++++++++++++++++++++++++++

namespace Controller {


class StandaloneMainController;

class StandaloneSignalsHandler : public MainControllerSignalsHandler{
    Q_OBJECT

public:
    StandaloneSignalsHandler(StandaloneMainController* mainController);
public slots:
    void on_ninjamBpmChanged(int newBpm);
    void on_newNinjamInterval();
    void on_ninjamStartProcessing(int intervalPosition);
    void on_connectedInNinjamServer(Ninjam::Server server);
    void on_audioDriverSampleRateChanged(int newSampleRate);
    void on_audioDriverStarted();
    void on_audioDriverStopped();
    void on_VSTPluginFounded(QString name, QString group, QString path);
private:
    StandaloneMainController* controller;
};

class StandaloneMainController : public QApplication, public MainController
{
    Q_OBJECT

    friend class StandaloneSignalsHandler;
public:
    StandaloneMainController(Persistence::Settings settings, int& argc, char** argv);
    ~StandaloneMainController();

    bool isRunningAsVstPlugin() const;

    void initializePluginsList(QStringList paths);
    void scanPlugins();

    Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor);

    virtual void addDefaultPluginsScanPath();


    inline Vst::Host* getVstHost() const{return vstHost;}

    void stopNinjamController();

    void start();

protected:
    virtual void exit();
    virtual MainControllerSignalsHandler* createSignalsHandler();
    virtual Midi::MidiDriver* createMidiDriver();
    virtual Audio::AudioDriver* buildAudioDriver(const Persistence::Settings &settings);
    virtual Vst::PluginFinder* createPluginFinder();

    void setCSS(QString css);
private:
    //VST
    Vst::Host* vstHost;


//private slots:
//    //VST
//    void on_VSTPluginFounded(QString name, QString group, QString path);
//protected slots:
//    void on_audioDriverSampleRateChanged(int newSampleRate);
//    void on_audioDriverStarted();
//    void on_audioDriverStopped();

//    void on_connectedInNinjamServer(Ninjam::Server server);
//    void on_ninjamBpmChanged(int newBpm);
//    void on_ninjamStartProcessing(int intervalPosition);
//    void on_newNinjamInterval();
};

}

#endif // STANDALONEMAINCONTROLLER_H
