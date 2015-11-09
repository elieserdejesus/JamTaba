#include "StandAloneMainController.h"

//#include "midi/portmididriver.h"
#include "midi/RtMidiDriver.h"
#include "audio/core/PortAudioDriver.h"

#include "audio/vst/VstPlugin.h"
#include "audio/vst/vsthost.h"
#include "../audio/vst/PluginFinder.h"
#include "../NinjamController.h"
#include <QDialog>

#if _WIN32
    #include "windows.h"
#endif


#include <QDataStream>
#include <QFile>
#include <QDirIterator>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include "../log/logging.h"

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
            bool isJamtabaVstPlugin = f.fileName().contains("Jamtaba");
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

void StandalonePluginFinder::run(QStringList blackList){
    emit scanStarted();

    for(QString scanFolder : scanFolders){
        qCDebug(jtStandalonePluginFinder) << "Scanning the folder " << scanFolder;
        QDirIterator folderIterator(scanFolder, QDirIterator::Subdirectories);
        while (folderIterator.hasNext()) {
            folderIterator.next();//point to next file inside current folder
            QFileInfo pluginFileInfo (folderIterator.filePath());
            if(pluginFileInfo.isFile()){
                qCDebug(jtStandalonePluginFinder) << "Scanning " << pluginFileInfo.absoluteFilePath();
                if(!blackList.contains(pluginFileInfo.absoluteFilePath())){
                    emit pluginScanStarted(pluginFileInfo.absoluteFilePath());

                    const Audio::PluginDescriptor& descriptor = getPluginDescriptor(pluginFileInfo, host);
                    if(descriptor.isValid()){
                        emit pluginScanFinished(descriptor.getName(), descriptor.getGroup(), descriptor.getPath());
                    }
                }
                else{
                    qCDebug(jtStandalonePluginFinder) << "\nFiltering black listed plugin:" <<pluginFileInfo.fileName() << endl;
                }
            }
            QApplication::processEvents();
        }
    }
    emit scanFinished();

}

void StandalonePluginFinder::scan(QStringList blackList){
    //run the VST plugins scanning in another tread to void block Qt thread
    //If Qt main thread is block the GUI will be unresponsive, can't send or receive network data
    QtConcurrent::run(this, &StandalonePluginFinder::run, blackList);
}

//++++++++++++++++++++++++++++++++++

void StandaloneMainController::on_ninjamBpmChanged(int newBpm){
    MainController::on_ninjamBpmChanged(newBpm);
    vstHost->setTempo(newBpm);
}

void StandaloneMainController::on_connectedInNinjamServer(Ninjam::Server server){
    MainController::on_connectedInNinjamServer(server);
    vstHost->setTempo(server.getBpm());
}

void StandaloneMainController::on_audioDriverSampleRateChanged(int newSampleRate){
    MainController::on_audioDriverSampleRateChanged(newSampleRate);
    vstHost->setSampleRate(newSampleRate);
    foreach (Audio::LocalInputAudioNode* inputNode, inputTracks) {
        inputNode->setProcessorsSampleRate(newSampleRate);
    }
}

void StandaloneMainController::on_audioDriverStarted(){
    MainController::on_audioDriverStarted();

    vstHost->setSampleRate(audioDriver->getSampleRate());
    vstHost->setBlockSize(audioDriver->getBufferSize());

    foreach (Audio::LocalInputAudioNode* inputTrack, inputTracks) {
        inputTrack->resumeProcessors();
    }
}

void StandaloneMainController::on_audioDriverStopped(){
    MainController::on_audioDriverStopped();
    foreach (Audio::LocalInputAudioNode* inputTrack, inputTracks) {
        inputTrack->suspendProcessors();//suspend plugins
    }
}

void StandaloneMainController::on_newNinjamInterval(){
    MainController::on_newNinjamInterval();
    vstHost->setPlayingFlag(true);
}

void StandaloneMainController::on_ninjamStartProcessing(int intervalPosition){
    MainController::on_ninjamStartProcessing(intervalPosition);
    vstHost->update(intervalPosition);//update the vst host time line.
}


void StandaloneMainController::on_VSTPluginFounded(QString name, QString group, QString path){
    pluginsDescriptors.append(Audio::PluginDescriptor(name, group, path));
    settings.addVstPlugin(path);

}

//++++++++++++++++++++++++++++++++++++++++++

bool StandaloneMainController::isRunningAsVstPlugin() const{
    return false;
}

Vst::PluginFinder* StandaloneMainController::createPluginFinder(){
    return new StandalonePluginFinder(vstHost);
}

Midi::MidiDriver* StandaloneMainController::createMidiDriver(){
    //return new Midi::PortMidiDriver(settings.getMidiInputDevicesStatus());
    return new Midi::RtMidiDriver(settings.getMidiInputDevicesStatus());
    //return new Midi::NullMidiDriver();
}

Controller::NinjamController* StandaloneMainController::createNinjamController(MainController *c){
    return new NinjamController(c);
}

Audio::AudioDriver* StandaloneMainController::createAudioDriver(const Persistence::Settings &settings){

#ifdef Q_OS_WIN
    return new Audio::PortAudioDriver(
                this,
                settings.getLastAudioDevice(),
                settings.getFirstGlobalAudioInput(),
                settings.getLastGlobalAudioInput(),
                settings.getFirstGlobalAudioOutput(),
                settings.getLastGlobalAudioOutput(),
                settings.getLastSampleRate(),
                settings.getLastBufferSize()
                );
#else
    //MAC
    return new Audio::PortAudioDriver(this, settings.getLastSampleRate(), settings.getLastBufferSize());
#endif
}

//++++++++++++++++++++++++++++++++++++++++++
StandaloneMainController::StandaloneMainController(Persistence::Settings settings, QApplication* application)
    : MainController(settings), vstHost(Vst::Host::getInstance()), application(application){

    application->setQuitOnLastWindowClosed(true);

    QObject::connect(Vst::Host::getInstance(), SIGNAL(pluginRequestingWindowResize(QString,int,int)),
                     this, SLOT(on_vstPluginRequestedWindowResize(QString,int,int)));


}

void StandaloneMainController::on_vstPluginRequestedWindowResize(QString pluginName, int newWidht, int newHeight){
    QDialog* pluginEditorWindow = Vst::VstPlugin::getPluginEditorWindow(pluginName);
    if(pluginEditorWindow){
        pluginEditorWindow->setFixedSize(newWidht, newHeight);
        //pluginEditorWindow->updateGeometry();

    }
}

void StandaloneMainController::start(){
    MainController::start();
    QObject::connect(pluginFinder.data(), SIGNAL(pluginScanFinished(QString,QString,QString)), this, SLOT(on_VSTPluginFounded(QString,QString,QString)));

    if(audioDriver){
        vstHost->setSampleRate(audioDriver->getSampleRate());
        vstHost->setBlockSize(audioDriver->getBufferSize());
    }
}

void StandaloneMainController::setCSS(QString css){
    application->setStyleSheet(css);
}

StandaloneMainController::~StandaloneMainController(){
    qCDebug(jtCore) << "StandaloneMainController destructor!";
    //pluginsDescriptors.clear();
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

QStringList StandaloneMainController::getSteinbergRecommendedPaths(){
    /*
    On a 64-bit OS

    64-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\VST
    32-bit plugins path = HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\VST
    */
    QStringList vstPaths;
#ifdef Q_OS_WIN

    #ifdef _WIN64//64 bits
        QSettings wowSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\VST\\", QSettings::NativeFormat);
        vstPaths.append(wowSettings.value("VSTPluginsPath").toString());
    #else//32 bits
        QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\VST\\", QSettings::NativeFormat);
        vstPaths.append(settings.value("VSTPluginsPath").toString());
    #endif
#endif

#ifdef Q_OS_MACX
       vstPaths.append("/Library/Audio/Plug-Ins/VST");
       vstPaths.append( "~/Library/Audio/Plug-Ins/VST");
#endif
       return vstPaths;
}

void StandaloneMainController::addDefaultPluginsScanPath(){
    QStringList vstPaths;

    //first try read the path store in registry by Jamtaba installer.
    //If this value is not founded use the Steinberg recommended paths.
    QSettings jamtabaRegistryEntry("HKEY_CURRENT_USER\\SOFTWARE\\Jamtaba", QSettings::NativeFormat);
    QString vst2InstallDir = jamtabaRegistryEntry.value("VST2InstallDir").toString();
    if(!vst2InstallDir.isEmpty()){
        vstPaths.append(vst2InstallDir);
    }else{
        vstPaths.append(getSteinbergRecommendedPaths());
    }

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
        pluginFinder->clearScanFolders();
        QStringList foldersToScan = settings.getVstScanFolders();

        foreach (QString folder, foldersToScan) {
            pluginFinder->addFolderToScan(folder);
        }
        pluginFinder->scan(settings.getBlackBox());
    }
}


void StandaloneMainController::stopNinjamController(){
    MainController::stopNinjamController();
    vstHost->setPlayingFlag(false);
}


