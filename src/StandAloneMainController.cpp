#include "StandAloneMainController.h"

#include "midi/RtMidiDriver.h"
#include "audio/core/PortAudioDriver.h"

#include "audio/vst/VstPlugin.h"
#include "audio/vst/VstHost.h"
#include "audio/vst/PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "NinjamController.h"
#include <QDialog>
#include <QHostAddress>

#if _WIN32
    #include "windows.h"
#endif


#include <QDataStream>
#include <QFile>
#include <QDirIterator>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include "log/logging.h"
#include "configurator.h"

using namespace Controller;

//+++++++++++++++++++++++++

StandalonePluginFinder::StandalonePluginFinder(){

}

StandalonePluginFinder::~StandalonePluginFinder(){

}

Audio::PluginDescriptor StandalonePluginFinder::getPluginDescriptor(QFileInfo f){
    QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
    return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
}


void StandalonePluginFinder::on_processFinished(){
    QProcess::ExitStatus exitStatus = scanProcess.exitStatus();
    scanProcess.close();
    bool exitingWithoutError = exitStatus == QProcess::NormalExit;
    emit scanFinished(exitingWithoutError);
    QString lastScanned(lastScannedPlugin);
    lastScannedPlugin.clear();
    if(!exitingWithoutError){
        handleProcessError(lastScanned);
    }
}

void StandalonePluginFinder::on_processError(QProcess::ProcessError error){

    qCritical(jtStandalonePluginFinder) << "ERROR:" << error << scanProcess.errorString();
    on_processFinished();
}

void StandalonePluginFinder::handleProcessError(QString lastScannedPlugin){
    if(!lastScannedPlugin.isEmpty()){
        emit badPluginDetected(lastScannedPlugin);
    }
}

QString StandalonePluginFinder::getVstScannerExecutablePath() const{
    //try the same jamtaba executable path first
    QString scannerExePath = QApplication::applicationDirPath() + "/VstScanner";//In the deployed version the VstScanner and Jamtaba2 executables are in the same folder.
#ifdef Q_OS_WIN
    scannerExePath += ".exe";
#endif
    if(QFile(scannerExePath).exists()){
        return scannerExePath;
    }
    else{
        qWarning(jtStandalonePluginFinder) << "Scanner exe not founded in" << scannerExePath;
    }

    //In dev time the executable (Jamtaba2 and VstScanner) are in different folders...
    //We need a more elegant way to solve this at dev time. At moment I'm a very dirst approach to return the executable path in MacOsx, and just a little less dirty solution in windows.
    QString appPath = QCoreApplication::applicationDirPath();
#ifdef Q_OS_MAC
    return "/Users/elieser/Desktop/build-Jamtaba-Desktop_Qt_5_5_0_clang_64bit-Debug/VstScanner/VstScanner";
#endif
    QString buildType = QLibraryInfo::isDebugBuild() ? "debug" : "release";
    scannerExePath = appPath + "/../../VstScanner/"+ buildType +"/VstScanner.exe";
    if(QFile(scannerExePath).exists()){
        return scannerExePath;
    }
    qCCritical(jtStandalonePluginFinder) << "Vst scanner exeutable not found in" << scannerExePath;
    return "";
}

void StandalonePluginFinder::on_processStandardOutputReady(){
    QByteArray readedData = scanProcess.readAll();
    QTextStream stream(readedData, QIODevice::ReadOnly);
    while(!stream.atEnd()){
        QString readedLine = stream.readLine();
        if(!readedLine.isNull() && !readedLine.isEmpty()){
            bool startScanning = readedLine.startsWith("JT-Scanner-Scanning:");
            bool finishedScanning = readedLine.startsWith("JT-Scanner-Scan-Finished");
            if(startScanning || finishedScanning){
                QString pluginPath = readedLine.split(": ").at(1);
                if(startScanning){
                    lastScannedPlugin = pluginPath;//store the plugin path, if the scanner process crash we can add this bad plugin in the black list
                    emit pluginScanStarted(pluginPath);
                }
                else{
                    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(pluginPath);
                    emit pluginScanFinished(pluginName, "VST", pluginPath);
                }
            }
        }
    }

}

QString StandalonePluginFinder::buildCommaSeparetedString(QStringList list) const{
    QString folderString;
    for (int c = 0; c < list.size(); ++c) {
        folderString += list.at(c);
        if(c < list.size() -1){
            folderString += ";";
        }
    }
    return folderString;
}


void StandalonePluginFinder::scan(QStringList skipList){
    if(scanProcess.isOpen()){
        qCWarning(jtStandalonePluginFinder) << "scan process is already open!";
        return;
    }

    QString scannerExePath = getVstScannerExecutablePath();
    if(scannerExePath.isEmpty()){
        return;//scanner executable not found!
    }

    emit scanStarted();
    //execute the scanner in another process to avoid crash Jamtaba process
    QStringList parameters;
    parameters.append(buildCommaSeparetedString(scanFolders));
    parameters.append(buildCommaSeparetedString(skipList));
    QObject::connect( &scanProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(on_processStandardOutputReady()));
    QObject::connect( &scanProcess, SIGNAL(finished(int)), this, SLOT(on_processFinished()));
    QObject::connect( &scanProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(on_processError(QProcess::ProcessError)));
    qCDebug(jtStandalonePluginFinder) << "Starting scan process...";
    scanProcess.start(scannerExePath, parameters);
    qCDebug(jtStandalonePluginFinder) << "Scan process started with " << scannerExePath;
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
    return new StandalonePluginFinder();
}

void StandalonePluginFinder::cancel(){
    if(scanProcess.isOpen()){
        scanProcess.terminate();
    }
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


//we need scan plugins when plugins cache is empty OR we have new plugins in scan folders
//This code is executed whem Jamtaba is started.
bool StandaloneMainController::pluginsScanIsNeeded() const
{
    if (settings.getVstPluginsPaths().isEmpty())//cache is empty?
    {
        return true;
    }

    //checking for new plugins in scan folders
    QStringList foldersToScan = settings.getVstScanFolders();

    QStringList skipList(settings.getBlackListedPlugins());
    skipList.append(settings.getVstPluginsPaths());

    foreach (QString scanFolder, foldersToScan)
    {
        QDirIterator folderIterator(scanFolder, QDirIterator::Subdirectories);
        while (folderIterator.hasNext())
        {
            folderIterator.next();//point to next file inside current folder
            QString filePath = folderIterator.filePath();
            if ( isVstPluginFile(filePath) && !skipList.contains(filePath) )
            {
                return true; //a new vst plugin was founded
            }
        }
    }
    return false;
}

bool StandaloneMainController::isVstPluginFile(QString filePath) const
{
#ifdef Q_OS_WIN
    return QLibrary::isLibrary(filePath);
#endif

#ifdef Q_OS_MAC
    QFileInfo file(filePath);
    return file.isBundle() && file.absoluteFilePath().endsWith(".vst")
#endif
    return false; //just in case
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



void StandaloneMainController::scanPlugins(bool scanOnlyNewPlugins){
    if(pluginFinder){
        if(!scanOnlyNewPlugins){
            pluginsDescriptors.clear();
        }

        pluginFinder->setFoldersToScan(settings.getVstScanFolders());

        //The skipList contains the paths for black listed plugins by default.
        //If the parameter 'scanOnlyNewPlugins' is 'true' the cached plugins are added in the skipList too.
        QStringList skipList(settings.getBlackListedPlugins());
        if(scanOnlyNewPlugins){
            skipList.append(settings.getVstPluginsPaths());
        }
        pluginFinder->scan(skipList);
    }
}


void StandaloneMainController::stopNinjamController(){
    MainController::stopNinjamController();
    vstHost->setPlayingFlag(false);
}


void StandaloneMainController::quit()
{
    //destroy the extern !
    //if(JTBConfig)delete JTBConfig;
    qDebug() << "Thank you for Jamming with Jamtaba !";
    application->quit();
}
