#ifndef STANDALONEMAINCONTROLLER_H
#define STANDALONEMAINCONTROLLER_H

#include "MainController.h"
#include <QApplication>

namespace Jamtaba {
    class VstHost;
    class PluginFinder;
}

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

    void scanPlugins();
    void initializePluginsList(QStringList paths);

    Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor);


    void addVstScanPath(QString path);
    void addDefaultVstScanPath();//add vst path from registry
    void removeVstScanPath(int index);
    void clearVstCache();

    inline Jamtaba::VstHost* getVstHost() const{return vstHost;}

    void stopNinjamController();

protected:
    virtual void exit();
    virtual MainControllerSignalsHandler* createSignalsHandler();

    void setCSS(QString css);
private:
    //VST
    Jamtaba::VstHost* vstHost;


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
