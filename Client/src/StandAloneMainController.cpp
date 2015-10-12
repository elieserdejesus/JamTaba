#include "StandAloneMainController.h"


#include "audio/vst/VstPlugin.h"
#include "audio/vst/vsthost.h"
#include "../audio/vst/PluginFinder.h"

#include <QSettings>

using namespace Controller;
//++++++++++++++++++++++++++++++++++

StandaloneSignalsHandler::StandaloneSignalsHandler(StandaloneMainController *mainController)
    :MainControllerSignalsHandler(mainController){
    this->controller = mainController;
}

void StandaloneSignalsHandler::on_ninjamBpmChanged(int newBpm){
    MainControllerSignalsHandler::on_ninjamBpmChanged(newBpm);
    this->controller->vstHost->setTempo(newBpm);
}

void StandaloneSignalsHandler::on_connectedInNinjamServer(Ninjam::Server server){
    MainControllerSignalsHandler::on_connectedInNinjamServer(server);
    this->controller->vstHost->setTempo(server.getBpm());
}

void StandaloneSignalsHandler::on_audioDriverSampleRateChanged(int newSampleRate){
    MainControllerSignalsHandler::on_audioDriverSampleRateChanged(newSampleRate);
    this->controller->vstHost->setSampleRate(newSampleRate);
    foreach (Audio::LocalInputAudioNode* inputNode, controller->inputTracks) {
        inputNode->setProcessorsSampleRate(newSampleRate);
    }
}

void StandaloneSignalsHandler::on_audioDriverStarted(){
    MainControllerSignalsHandler::on_audioDriverStarted();

    controller->vstHost->setSampleRate(controller->audioDriver->getSampleRate());
    controller->vstHost->setBlockSize(controller->audioDriver->getBufferSize());

    foreach (Audio::LocalInputAudioNode* inputTrack, controller->inputTracks) {
        inputTrack->resumeProcessors();
    }
}

void StandaloneSignalsHandler::on_audioDriverStopped(){
    MainControllerSignalsHandler::on_audioDriverStopped();
    foreach (Audio::LocalInputAudioNode* inputTrack, controller->inputTracks) {
        inputTrack->suspendProcessors();//suspend plugins
    }
}

void StandaloneSignalsHandler::on_newNinjamInterval(){
    controller->vstHost->setPlayingFlag(true);
}

void StandaloneSignalsHandler::on_ninjamStartProcessing(int intervalPosition){
    controller->vstHost->update(intervalPosition);//update the vst host time line.
}


void StandaloneSignalsHandler::on_VSTPluginFounded(QString name, QString group, QString path){
    controller->pluginsDescriptors.append(Audio::PluginDescriptor(name, group, path));
    controller->settings.addVstPlugin(path);
}

//++++++++++++++++++++++++++++++++++++++++++
MainControllerSignalsHandler* StandaloneMainController::createSignalsHandler(){
    return new StandaloneSignalsHandler(this);
}

//++++++++++++++++++++++++++++++++++++++++++
StandaloneMainController::StandaloneMainController(Persistence::Settings settings, int &argc, char **argv)
    : QApplication(argc, argv), MainController(settings), vstHost(Jamtaba::VstHost::getInstance()){


    QObject::connect(&pluginFinder, SIGNAL(pluginScanFinished(QString,QString,QString)), this->signalsHandler, SLOT(on_VSTPluginFounded(QString,QString,QString)));

    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());



    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
}

void StandaloneMainController::exit(){
    QApplication::exit(0); //call exit in QApplication
}

void StandaloneMainController::setCSS(QString css){
    QApplication::setStyleSheet(css);
}

StandaloneMainController::~StandaloneMainController(){
    pluginsDescriptors.clear();
}

Audio::Plugin *StandaloneMainController::createPluginInstance(const Audio::PluginDescriptor& descriptor)
{
    if(descriptor.isNative()){
        if(descriptor.getName() == "Delay"){
            return new Audio::JamtabaDelay(audioDriver->getSampleRate());
        }
    }
    else if(descriptor.isVST()){
            Jamtaba::VstPlugin* vstPlugin = new Jamtaba::VstPlugin(this->vstHost);
            if(vstPlugin->load( descriptor.getPath())){
                return vstPlugin;
            }
    }
    return nullptr;
}


void StandaloneMainController::addVstScanPath(QString path){
    settings.addVstScanPath(path);
}

void StandaloneMainController::addDefaultVstScanPath(){
    /*
    On a 64-bit OS

    64-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\VST
    32-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\VST
    */
    QStringList vstPaths;
#ifdef Q_OS_WIN
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\VST\\", QSettings::NativeFormat);
    vstPaths.append(settings.value("VSTPluginsPath").toString());

    #ifdef _WIN64//64 bits?
        QSettings wowSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\VST\\", QSettings::NativeFormat);
        vstPaths.append(wowSettings.value("VSTPluginsPath").toString());
    #endif
#endif

#ifdef Q_OS_MACX
       vstPaths.append("/Library/Audio/Plug-Ins/VST");
       vstPaths.append( "~/Library/Audio/Plug-Ins/VST");
#endif
    foreach (QString vstPath, vstPaths) {
        if(!vstPath.isEmpty() && QDir(vstPath).exists()){
            addVstScanPath(vstPath);
        }
    }
}

void StandaloneMainController::removeVstScanPath(int index){
   settings.removeVstScanPath(index);
}

void StandaloneMainController::clearVstCache(){
    settings.clearVstCache();
}

void StandaloneMainController::initializePluginsList(QStringList paths){
    pluginsDescriptors.clear();
    foreach (QString path, paths) {
        QFile file(path);
        if(file.exists()){
            QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(path);
            pluginsDescriptors.append(Audio::PluginDescriptor(pluginName, "VST", path));
        }
    }
}

void StandaloneMainController::scanPlugins(){
    pluginsDescriptors.clear();
    //ConfigStore::clearVstCache();
    pluginFinder.clearScanPaths();
    QStringList scanPaths = settings.getVstScanPaths();

    foreach (QString path, scanPaths) {
        pluginFinder.addPathToScan(path);
    }
    pluginFinder.scan(vstHost);
}


void StandaloneMainController::stopNinjamController(){
    MainController::stopNinjamController();
    vstHost->setPlayingFlag(false);
}


