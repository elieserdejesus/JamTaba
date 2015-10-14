#include "StandAloneMainController.h"

#include "midi/portmididriver.h"
#include "audio/core/PortAudioDriver.h"

#include "audio/vst/VstPlugin.h"
#include "audio/vst/vsthost.h"
#include "../audio/vst/PluginFinder.h"

#if _WIN32
    #include "windows.h"
#endif


#include <QDataStream>
#include <QFile>
#include <QDirIterator>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>

using namespace Controller;

//+++++++++++++++++++++++++
StandalonePluginFinder::StandalonePluginFinder(Vst::Host *host)
    :host(host)
{

}

StandalonePluginFinder::~StandalonePluginFinder()
{

}


//++++++++++++++++++++++++++++++++++++++++++++
#ifdef Q_OS_WIN
class WindowsDllArchChecker{
public:
    static bool is32Bits(QString dllPath){
        return getMachineHeader(dllPath) == 0x14c;//i386
    }

    static bool is64Bits(QString dllPath){
        return getMachineHeader(dllPath) == 0x8664;//AMD64
    }
private:
    static quint16 getMachineHeader(QString dllPath){
        // See http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
        // Offset to PE header is always at 0x3C.
        // The PE header starts with "PE\0\0" =  0x50 0x45 0x00 0x00,
        // followed by a 2-byte machine type field (see the document above for the enum).

        QFile dllFile(dllPath);
        if(!dllFile.open(QFile::ReadOnly)){
            return 0;
        }
        dllFile.seek(0x3c);
        QDataStream dataStream(&dllFile);
        dataStream.setByteOrder(QDataStream::LittleEndian);
        qint32 peOffset;
        dataStream >> peOffset;
        dllFile.seek(peOffset);
        quint32 peHead;
        dataStream >> peHead;
        if (peHead!=0x00004550) // "PE\0\0", little-endian
            return 0; //"Can't find PE header"

        quint16 machineType;
        dataStream >> machineType;
        dllFile.close();
        return machineType;
    }
};
#endif
//++++++++++++++++++++++++++++++++++++++++++++


//retorna nullptr se não for um plugin
Audio::PluginDescriptor StandalonePluginFinder::getPluginDescriptor(QFileInfo f, Vst::Host* host){

    try{
        bool archIsValid = true;
        #ifdef Q_OS_WIN
            //qCDebug(vst) << "Testing " << f.absoluteFilePath();
            bool isFile = f.isFile();
            bool isLibrary = QLibrary::isLibrary(f.fileName());
            bool isJamtabaVstPlugin = false;//f.fileName().contains("Jamtaba");
            if (!isFile || !isLibrary || isJamtabaVstPlugin){
                return Audio::PluginDescriptor();//invalid descriptor
            }
            #ifdef _WIN64
                archIsValid = WindowsDllArchChecker::is64Bits(f.absoluteFilePath());
            #else
                archIsValid = WindowsDllArchChecker::is32Bits(f.absoluteFilePath());
            #endif
        #else
            //MAC
            if(!f.isBundle() || f.completeSuffix() != "vst"){
                return Audio::PluginDescriptor();//invalid descriptor
            }
        #endif
            if(archIsValid){
                Vst::VstPlugin plugin(host);
                if(plugin.load(f.absoluteFilePath())){
                    QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
                    return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
                }
            }
    }
    catch(...){
        qCritical() << "não carregou " << f.absoluteFilePath();
    }
    return Audio::PluginDescriptor();//invalid descriptor
}

void StandalonePluginFinder::run(){
    emit scanStarted();
    for(QString scanPath : scanPaths){
        QDirIterator dirIt(scanPath, QDirIterator::Subdirectories);
        while (dirIt.hasNext()) {
            dirIt.next();
            QFileInfo fileInfo (dirIt.filePath());
            emit pluginScanStarted(fileInfo.absoluteFilePath());
            const Audio::PluginDescriptor& descriptor = getPluginDescriptor(fileInfo, host);
            if(descriptor.isValid()){
                emit pluginScanFinished(descriptor.getName(), descriptor.getGroup(), descriptor.getPath());
            }
            QApplication::processEvents();
        }
    }
    emit scanFinished();
}

void StandalonePluginFinder::scan(){
    QtConcurrent::run(this, &StandalonePluginFinder::run);
}

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

bool StandaloneMainController::isRunningAsVstPlugin() const{
    return false;
}

Vst::PluginFinder* StandaloneMainController::createPluginFinder(){
    return new StandalonePluginFinder(vstHost);
}

Midi::MidiDriver* StandaloneMainController::createMidiDriver(){
    return new Midi::PortMidiDriver(settings.getMidiInputDevicesStatus());
}

Audio::AudioDriver* StandaloneMainController::buildAudioDriver(const Persistence::Settings &settings){
#ifdef Q_OS_WIN
    return new Audio::PortAudioDriver(
                this, //the AudioDriverListener instance
                settings.getLastInputDevice(), settings.getLastOutputDevice(),
                settings.getFirstGlobalAudioInput(), settings.getLastGlobalAudioInput(),
                settings.getFirstGlobalAudioOutput(), settings.getLastGlobalAudioOutput(),
                settings.getLastSampleRate(), settings.getLastBufferSize()
                );
#else
    //MAC
    return new Audio::PortAudioDriver(this, settings.getLastSampleRate(), settings.getLastBufferSize());
#endif
}

//++++++++++++++++++++++++++++++++++++++++++
StandaloneMainController::StandaloneMainController(Persistence::Settings settings, int &argc, char **argv)
    : QApplication(argc, argv), MainController(settings), vstHost(Vst::Host::getInstance()){


    setQuitOnLastWindowClosed(false);//wait disconnect from server to close
}

void StandaloneMainController::start(){
    MainController::start();
    QObject::connect(pluginFinder, SIGNAL(pluginScanFinished(QString,QString,QString)), this->signalsHandler, SLOT(on_VSTPluginFounded(QString,QString,QString)));

    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());
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
            Vst::VstPlugin* vstPlugin = new Vst::VstPlugin(this->vstHost);
            if(vstPlugin->load( descriptor.getPath())){
                return vstPlugin;
            }
    }
    return nullptr;
}


void StandaloneMainController::addDefaultPluginsScanPath(){
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
            addPluginsScanPath(vstPath);
        }
    }
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
    if(pluginFinder){
        pluginFinder->clearScanPaths();
        QStringList scanPaths = settings.getVstScanPaths();

        foreach (QString path, scanPaths) {
            pluginFinder->addPathToScan(path);
        }
        pluginFinder->scan();
    }
}


void StandaloneMainController::stopNinjamController(){
    MainController::stopNinjamController();
    vstHost->setPlayingFlag(false);
}


