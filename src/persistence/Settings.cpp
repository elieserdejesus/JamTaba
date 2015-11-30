#include "Settings.h"
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDir>
#include <QList>
#include <QSettings>
#include "../log/logging.h"


//extern Configurator *JTBConfig;

using namespace Persistence;

#if defined(Q_OS_WIN64) || defined(Q_OS_MAC64)
    QString Settings::fileName = "Jamtaba64.json";
#else
    QString Settings::fileName = "Jamtaba.json";
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SettingsObject::SettingsObject(QString name)
    :name(name){

}

int SettingsObject::getValueFromJson(const QJsonObject &json, QString propertyName, int fallBackValue){
    if(json.contains(propertyName)){
        return json[propertyName].toInt();
    }
    return fallBackValue;
}

bool SettingsObject::getValueFromJson(const QJsonObject &json, QString propertyName, bool fallBackValue){
    if(json.contains(propertyName)){
        return json[propertyName].toBool();
    }
    return fallBackValue;
}

QString SettingsObject::getValueFromJson(const QJsonObject &json, QString propertyName, QString fallBackValue){
    if(json.contains(propertyName)){
        return json[propertyName].toString();
    }
    return fallBackValue;
}


float SettingsObject::getValueFromJson(const QJsonObject &json, QString propertyName, float fallBackValue){
    if(json.contains(propertyName)){
        return (float)(json[propertyName].toDouble());
    }
    return fallBackValue;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PrivateServerSettings::PrivateServerSettings()
    :SettingsObject("PrivateServer"), server("localhost"), serverPort(2049), password(""){

}

void PrivateServerSettings::write(QJsonObject &out){
    out["server"] = server;
    out["password"] = password;
    out["port"] = serverPort;
}

void PrivateServerSettings::read(QJsonObject in){
    server = getValueFromJson(in, "server", QString("localhost"));
    password = getValueFromJson(in, "password", QString(""));
    serverPort = getValueFromJson(in, "port", 2049);
}

void Settings::setPrivateServerData(QString server, int serverPort, QString password){
    privateServerSettings.server = server;
    privateServerSettings.serverPort = serverPort;
    privateServerSettings.password = password;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioSettings::AudioSettings()
    : SettingsObject("audio"), sampleRate(44100), bufferSize(128)
{

}

void AudioSettings::read(QJsonObject in){
    sampleRate =    getValueFromJson(in, "sampleRate", 44100);
    bufferSize =    getValueFromJson(in, "bufferSize", 128);
    firstIn =       getValueFromJson(in, "firstIn", 0);
    firstOut =      getValueFromJson(in, "firstOut", 0);
    lastIn =        getValueFromJson(in, "lastIn", 0);
    lastOut =       getValueFromJson(in, "lastOut", 0);
    audioDevice =   getValueFromJson(in, "audioDevice", -1);

}

void AudioSettings::write(QJsonObject &out){
    out["sampleRate"]   = sampleRate;
    out["bufferSize"]   = bufferSize;
    out["firstIn"]      = firstIn;
    out["firstOut"]     = firstOut;
    out["lastIn"]       = lastIn;
    out["lastOut"]      = lastOut;
    out["audioDevice"]  = audioDevice;
}
//+++++++++++++++++++++++++++++
MidiSettings::MidiSettings()
    :SettingsObject("midi"){

}

void MidiSettings::write(QJsonObject &out){
    QJsonArray midiArray;
    foreach (bool state, inputDevicesStatus) {
        midiArray.append(state);
    }
    out["inputsState"] = midiArray;
}

void MidiSettings::read(QJsonObject in){
    inputDevicesStatus.clear();
    if(in.contains("inputsState")){
        QJsonArray inputsArray = in["inputsState"].toArray();
        for (int i = 0; i < inputsArray.size(); ++i) {
            inputDevicesStatus.append(inputsArray.at(i).toBool(true));
        }
    }
}
//++++++++++++++++++++++++++++++
RecordingSettings::RecordingSettings()
    : SettingsObject("recording"), saveMultiTracksActivated(false), recordingPath(""){

}

void RecordingSettings::write(QJsonObject &out){
    out["recordingPath"] = recordingPath;
    out["recordActivated"] = saveMultiTracksActivated;
}

void RecordingSettings::read(QJsonObject in){
    bool useDefaultRecordingPath = false;
    if(in.contains("recordingPath")){
        recordingPath = in["recordingPath"].toString();
        QDir dir(recordingPath);
        if(recordingPath.isEmpty() || !dir.exists()){
            //qWarning() << "Dir " << dir << " not exists, using the application directory to save multitracks!";
            useDefaultRecordingPath = true;
        }
    }
    else{
        useDefaultRecordingPath = true;
    }
    if(useDefaultRecordingPath){
        QString userDocuments = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
        QDir pathDir(QDir::homePath());
        QDir documentsDir(pathDir.absoluteFilePath(userDocuments));
        recordingPath = QDir(documentsDir).absoluteFilePath("Jamtaba");
    }
    saveMultiTracksActivated = getValueFromJson(in, "recordActivated", false);
}


//+++++++++++++++++++++++++++++
MetronomeSettings::MetronomeSettings()
    : SettingsObject("metronome"), pan(0), gain(1), muted(false)
{

}

void MetronomeSettings::read(QJsonObject in){
    pan =    getValueFromJson(in, "pan", (float)0);
    gain=    getValueFromJson(in, "gain", (float)1);
    muted=   getValueFromJson(in, "muted", false);
}

void MetronomeSettings::write(QJsonObject &out){
    out["pan"]   = pan;
    out["gain"]  = gain;
    out["muted"] = muted;
}
//+++++++++++++++++++++++++++
WindowSettings::WindowSettings()
    : SettingsObject("window"), maximized(false), fullViewMode(true){

}

void WindowSettings::read(QJsonObject in){
    maximized = getValueFromJson(in, "maximized", false);//not maximized as default
    fullViewMode = getValueFromJson(in, "fullView", true);//use full view mode as default
    fullScreenViewMode = getValueFromJson(in, "fullScreenView", false);//use normal mode as default;
    if(in.contains("location")){
        QJsonObject locationObj = in["location"].toObject();
        location.setX( getValueFromJson( locationObj, "x", (float)0));
        location.setY( getValueFromJson( locationObj, "y", (float)0));
    }
}

void WindowSettings::write(QJsonObject &out){
    out["maximized"] = maximized;
    out["fullView"] = fullViewMode;
    out["fullScreenView"] = fullScreenViewMode;
    QJsonObject locationObject;
    locationObject["x"] = this->location.x();
    locationObject["y"] = this->location.y();
    out["location"] = locationObject;
}

//+++++++++++++++++++++++++++++++++++++++
VstSettings::VstSettings()
    :SettingsObject("VST"){

}

//VST JSON WRITER
void VstSettings::write(QJsonObject &out){
    QJsonArray scanPathsArray;
    foreach (QString scanPath, foldersToScan) {
        scanPathsArray.append(scanPath);
    }
    out["scanPaths"] = scanPathsArray;

    QJsonArray cacheArray;
    foreach (QString pluginPath, cachedPlugins) {
        cacheArray.append(pluginPath);
    }
    out["cachedPlugins"] = cacheArray;

    QJsonArray BlackedArray;
    foreach (QString blackVst, blackedPlugins) {
        BlackedArray.append(blackVst);
    }
    out["BlackListPlugins"] = BlackedArray;
}

void VstSettings::read(QJsonObject in){
    foldersToScan.clear();
    if(in.contains("scanPaths")){
        QJsonArray scanPathsArray = in["scanPaths"].toArray();
        for (int i = 0; i < scanPathsArray.size(); ++i) {
           foldersToScan.append(scanPathsArray.at(i).toString());
        }
    }
    cachedPlugins.clear();
    if(in.contains("cachedPlugins")){
        QJsonArray cacheArray = in["cachedPlugins"].toArray();
        for (int x = 0; x < cacheArray.size(); ++x) {
            cachedPlugins.append(cacheArray.at(x).toString());
        }
    }
    blackedPlugins.clear();
    if(in.contains("BlackListPlugins")){
        QJsonArray cacheArray = in["BlackListPlugins"].toArray();
        for (int x = 0; x < cacheArray.size(); ++x) {
            blackedPlugins.append(cacheArray.at(x).toString());
        }
    }
}

//+++++++++++++++++++++++++++++++++++++++
PresetsSettings::PresetsSettings()
    :SettingsObject("PRESETS")
{

}

//PRESET JSON WRITER
void PresetsSettings::write(QJsonObject &out)
{
    QJsonArray channelsArray;
    foreach (const Channel& track, channels) {
        QJsonObject channelObject;
        channelObject["name"] = track.name;
        QJsonArray subchannelsArrays;
        foreach (const Subchannel& sub, track.subChannels) {
            QJsonObject subChannelObject;
            subChannelObject["firstInput"] = sub.firstInput;
            subChannelObject["channelsCount"] = sub.channelsCount;
            subChannelObject["midiDevice"] = sub.midiDevice;
            subChannelObject["midiChannel"] = sub.midiChannel;
            subChannelObject["gain"] = sub.gain;
            subChannelObject["boost"] = sub.boost;
            subChannelObject["pan"] = sub.pan;
            subChannelObject["muted"] = sub.muted;

            QJsonArray pluginsArray;
            foreach (Persistence::Plugin plugin, sub.plugins) {
                QJsonObject pluginObject;
                pluginObject["path"] = plugin.path;
                pluginObject["bypassed"] = plugin.bypassed;
                pluginObject["data"] =  QString(plugin.data.toBase64());
                pluginsArray.append(pluginObject);
            }
            subChannelObject["plugins"] = pluginsArray;

            subchannelsArrays.append(subChannelObject);
        }
        channelObject["subchannels"] = subchannelsArrays;
        channelsArray.append(channelObject);
    }
    out["channels"] = channelsArray;
}



void PresetsSettings::read(QJsonObject in)
{
   //todooooo ....
    if(in.contains("channels")){
        QJsonArray channelsArray = in["channels"].toArray();
        for (int i = 0; i < channelsArray.size(); ++i) {
            QJsonObject channelObject = channelsArray.at(i).toObject();
            Persistence::Channel channel(getValueFromJson(channelObject, "name", QString("")));
            if(channelObject.contains("subchannels")){
                QJsonArray subChannelsArray = channelObject["subchannels"].toArray();
                for (int k = 0; k < subChannelsArray.size(); ++k) {
                    QJsonObject subChannelObject = subChannelsArray.at(k).toObject();
                    int firstInput = getValueFromJson(subChannelObject, "firstInput", 0);
                    int channelsCount = getValueFromJson(subChannelObject, "channelsCount", 0);
                    int midiDevice = getValueFromJson(subChannelObject, "midiDevice", -1);
                    int midiChannel = getValueFromJson(subChannelObject, "midiChannel", -1);
                    float gain = getValueFromJson(subChannelObject, "gain", (float)1);
                    int boost = getValueFromJson(subChannelObject, "boost", (int)0);
                    float pan = getValueFromJson(subChannelObject, "pan", (float)0);
                    bool muted = getValueFromJson(subChannelObject, "muted", false);

                    QList<Plugin> plugins;
                    if(subChannelObject.contains("plugins")){
                        QJsonArray pluginsArray = subChannelObject["plugins"].toArray();
                        for (int p = 0; p < pluginsArray.size(); ++p) {
                            QJsonObject pluginObject = pluginsArray.at(p).toObject();
                            QString pluginPath = getValueFromJson(pluginObject, "path", QString(""));
                            bool bypassed = getValueFromJson(pluginObject, "bypassed", false);
                            QString dataString = getValueFromJson(pluginObject, "data", QString(""));
                            if( !pluginPath.isEmpty() && QFile(pluginPath).exists() ){
                                QByteArray rawByteArray(dataString.toStdString().c_str());
                                plugins.append(Persistence::Plugin(pluginPath, bypassed, QByteArray::fromBase64(rawByteArray)));
                            }
                        }
                    }
                    Persistence::Subchannel subChannel(firstInput, channelsCount, midiDevice, midiChannel, gain, boost, pan, muted, plugins);
                    channel.subChannels.append(subChannel);
                }
                this->channels.append(channel);
            }
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++
Channel::Channel(QString name)
    :name(name){

}

Plugin::Plugin(QString path, bool bypassed, QByteArray data)
    :path(path), bypassed(bypassed), data(data){

}

Subchannel::Subchannel(int firstInput, int channelsCount, int midiDevice, int midiChannel, float gain, int boost, float pan, bool muted, QList<Plugin> plugins)
    :firstInput(firstInput), channelsCount(channelsCount),
      midiDevice(midiDevice), midiChannel(midiChannel),
      gain(gain), boost(boost),
      pan(pan), muted(muted), plugins(plugins){

}

InputsSettings::InputsSettings()
    :SettingsObject("inputs"){

}

void InputsSettings::write(QJsonObject &out){
    QJsonArray channelsArray;
    foreach (const Channel& channel, channels) {
        QJsonObject channelObject;
        channelObject["name"] = channel.name;
        QJsonArray subchannelsArrays;
        foreach (const Subchannel& sub, channel.subChannels) {
            QJsonObject subChannelObject;
            subChannelObject["firstInput"] = sub.firstInput;
            subChannelObject["channelsCount"] = sub.channelsCount;
            subChannelObject["midiDevice"] = sub.midiDevice;
            subChannelObject["midiChannel"] = sub.midiChannel;
            subChannelObject["gain"] = sub.gain;
            subChannelObject["boost"] = sub.boost;
            subChannelObject["pan"] = sub.pan;
            subChannelObject["muted"] = sub.muted;

            QJsonArray pluginsArray;
            foreach (Persistence::Plugin plugin, sub.plugins) {
                QJsonObject pluginObject;
                pluginObject["path"] = plugin.path;
                pluginObject["bypassed"] = plugin.bypassed;
                pluginObject["data"] =  QString(plugin.data.toBase64());
                pluginsArray.append(pluginObject);
            }
            subChannelObject["plugins"] = pluginsArray;

            subchannelsArrays.append(subChannelObject);
        }
        channelObject["subchannels"] = subchannelsArrays;
        channelsArray.append(channelObject);
    }
    out["channels"] = channelsArray;
}

void InputsSettings::read(QJsonObject in){
    if(in.contains("channels")){
        QJsonArray channelsArray = in["channels"].toArray();
        for (int i = 0; i < channelsArray.size(); ++i) {
            QJsonObject channelObject = channelsArray.at(i).toObject();
            Persistence::Channel channel(getValueFromJson(channelObject, "name", QString("")));
            if(channelObject.contains("subchannels")){
                QJsonArray subChannelsArray = channelObject["subchannels"].toArray();
                for (int k = 0; k < subChannelsArray.size(); ++k) {
                    QJsonObject subChannelObject = subChannelsArray.at(k).toObject();
                    int firstInput = getValueFromJson(subChannelObject, "firstInput", 0);
                    int channelsCount = getValueFromJson(subChannelObject, "channelsCount", 0);
                    int midiDevice = getValueFromJson(subChannelObject, "midiDevice", -1);
                    int midiChannel = getValueFromJson(subChannelObject, "midiChannel", -1);
                    float gain = getValueFromJson(subChannelObject, "gain", (float)1);
                    int boost = getValueFromJson(subChannelObject, "boost", (int)0);
                    float pan = getValueFromJson(subChannelObject, "pan", (float)0);
                    bool muted = getValueFromJson(subChannelObject, "muted", false);

                    QList<Plugin> plugins;
                    if(subChannelObject.contains("plugins")){
                        QJsonArray pluginsArray = subChannelObject["plugins"].toArray();
                        for (int p = 0; p < pluginsArray.size(); ++p) {
                            QJsonObject pluginObject = pluginsArray.at(p).toObject();
                            QString pluginPath = getValueFromJson(pluginObject, "path", QString(""));
                            bool bypassed = getValueFromJson(pluginObject, "bypassed", false);
                            QString dataString = getValueFromJson(pluginObject, "data", QString(""));
                            if( !pluginPath.isEmpty() && QFile(pluginPath).exists() ){
                                QByteArray rawByteArray(dataString.toStdString().c_str());
                                plugins.append(Persistence::Plugin(pluginPath, bypassed, QByteArray::fromBase64(rawByteArray)));
                            }
                        }
                    }
                    Persistence::Subchannel subChannel(firstInput, channelsCount, midiDevice, midiChannel, gain, boost, pan, muted, plugins);
                    channel.subChannels.append(subChannel);
                }
                this->channels.append(channel);
            }
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++
void Settings::setUserName(QString newUserName){
    this->lastUserName = newUserName;
}

void Settings::setTranslation(QString translate){
    this->translation = translate;
}


void Settings::addVstPlugin(QString pluginPath){
    if(!vstSettings.cachedPlugins.contains(pluginPath)){
        vstSettings.cachedPlugins.append(pluginPath);
    }
}
void Settings::addVstToBlackList(QString pluginPath){
    if(!vstSettings.blackedPlugins.contains(pluginPath)){
        vstSettings.blackedPlugins.append(pluginPath);
    }
}
void Settings::RemVstFromBlackList(int index){
   vstSettings.blackedPlugins.removeAt(index);
}
QStringList Settings::getVstPluginsPaths() const{
    return vstSettings.cachedPlugins;
}



void Settings::clearVstCache(){
    vstSettings.cachedPlugins.clear();
}
//CLEAR VST BLACKBOX
void Settings::clearBlackBox(){
    vstSettings.blackedPlugins.clear();
}


//VST paths to scan
void Settings::addVstScanPath(QString path){
    vstSettings.foldersToScan.append(path);
}

void Settings::removeVstScanPath(int index){
    vstSettings.foldersToScan.removeAt(index);
}

QStringList Settings::getVstScanFolders() const {

    return vstSettings.foldersToScan;
}

QStringList Settings::getBlackBox() const {

    return vstSettings.blackedPlugins;
}


//++++++++++++++++++

//+++++++++++++++++++++++++++++
void Settings::setMetronomeSettings(float gain, float pan, bool muted){
    metronomeSettings.pan = pan;
    metronomeSettings.gain = gain;
    metronomeSettings.muted = muted;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Settings::setFullScreenView(bool v){ windowSettings.fullScreenViewMode=v;}
//+++++++++   Window Location  +++++++++++++++++++++++
void Settings::setWindowSettings(bool windowIsMaximized, bool usingFullView, QPointF location){
    double x = (location.x() >= 0) ? location.x() : 0;
    double y = (location.x() >= 0) ? location.y() : 0;
    if (x > 1) {
        location.setX( 0);
    }
    if (y > 1) {
        location.setY(0);
    }
    windowSettings.location = location;
    windowSettings.maximized = windowIsMaximized;
    windowSettings.fullViewMode = usingFullView;

}

//++++++++++++++++++++++++++++++++++++++++
void Settings::setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice, int sampleRate, int bufferSize){
    audioSettings.bufferSize = bufferSize;
    audioSettings.sampleRate = sampleRate;
    audioSettings.firstIn = firstIn;
    audioSettings.firstOut = firstOut;
    audioSettings.lastIn = lastIn;
    audioSettings.lastOut = lastOut;
    audioSettings.audioDevice = audioDevice;
}

// io ops ...
bool Settings::readFile(APPTYPE type, QList<Persistence::SettingsObject*> sections)
{
    if(type==plugin)
        fileDir = Configurator::getInstance()->getPluginDirPath();
    else
       fileDir = Configurator::getInstance()->getHomeDirPath();

    QDir dir(fileDir);//homepath

    QString absolutePath = dir.absoluteFilePath(fileName);
    //QFile file(absolutePath);
    QFile f(absolutePath);
    //qInfo(jtConfigurator) << "JSON Location :"<<absolutePath;
    if(f.open(QIODevice::ReadOnly))
    {
        qInfo(jtConfigurator) << "Reading settings from " << f.fileName() ;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonObject root = doc.object();

        //read user name
        if(root.contains("userName")){
            this->lastUserName = root["userName"].toString();
        }
        //read Translation
        if(root.contains("translation")){
            this->translation = root["translation"].toString();
        }
        if(this->translation.isEmpty()){
            QLocale userLocale;
            this->translation = userLocale.bcp47Name().left(2);
        }


        //read intervall progress shape
        if(root.contains("intervalProgressShape")){
            this->ninjamIntervalProgressShape = root["intervalProgressShape"].toInt();
        }
        else{
            this->ninjamIntervalProgressShape = 0;
        }

        //read settings sections (Audio settings, Midi settings, ninjam settings, etc...)
        foreach (SettingsObject* so, sections) {
            so->read(root[so->getName()].toObject());
        }
        return true;
    }
    else
    {
        qWarning(jtConfigurator) << "Settings : Can't load Jamtaba 2 config file:" << f.errorString();
    }

    return false;
}

bool Settings::writeFile(APPTYPE type, QList<SettingsObject *> sections)// io ops ...
{
   //Works with JTBConfig
   if(type==plugin)
        fileDir = Configurator::getInstance()->getPluginDirPath();
    QDir dir(fileDir);//homepath
    dir.mkpath(fileDir);
    //qWarning(jtConfigurator) << "SETTINGS WRITE JSON IN:" <<fileDir;
    QString absolutePath = dir.absoluteFilePath(fileName);
    QFile file(absolutePath);
    if(file.open(QIODevice::WriteOnly)){
        QJsonObject root;
        //write user name
        root["userName"] = this->lastUserName;
        //write translate locale
        root["translation"] = this->translation;

        root["intervalProgressShape"] = this->ninjamIntervalProgressShape;

        //write sections
        foreach (SettingsObject* so, sections) {
            QJsonObject sectionObject;
            so->write(sectionObject);
            root[so->getName()] = sectionObject;
        }
        QJsonDocument doc(root);
        file.write(doc.toJson());
        return true;
    }
    else{
        qCritical() << file.errorString();
    }
    return false;
}

//PRESETS
bool Settings::writePresetFile(QList<SettingsObject *> sections,QString name)
{
    //Works with Configurator

     QString absolutePath = Configurator::getInstance()->getPresetPath(name);
     QFile file(absolutePath);
     if(file.open(QIODevice::WriteOnly)){
         QJsonObject root;
         root["presetName"] = name;

         //write sections
         foreach (SettingsObject* so, sections) {
             QJsonObject sectionObject;
             so->write(sectionObject);
             root[so->getName()] = sectionObject;
         }
         QJsonDocument doc(root);
         file.write(doc.toJson());
         return true;
     }
     else{
         qCritical() << file.errorString();
     }
     return false;
}
bool Settings::readPresetFile(QList<Persistence::SettingsObject*> sections,QString name)
{

    QString absolutePath = Configurator::getInstance()->getPresetPath(name);
    //QFile file(absolutePath);
    QFile f(absolutePath);
    qInfo(jtConfigurator) << "JSON Location :"<<absolutePath;
    if(f.open(QIODevice::ReadOnly))
    {
        qInfo(jtConfigurator) << "Reading PRESET from " << f.fileName() ;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonObject root = doc.object();

        //read preset name , duno yet what it will serve ..
        if(root.contains("presetName"))
        {
            QString presetName = root["presetName"].toString();
        }


        //read settings sections (Audio settings, Midi settings, ninjam settings, etc...)
        foreach (SettingsObject* so, sections) {
            so->read(root[so->getName()].toObject());
            presetSettings.names.append(name);
        }
        return true;
    }
    else
    {
        qWarning(jtConfigurator) << "Settings : Can't load PRESET file:" << f.errorString();
    }

    return false;
}
//Using the new class presetSettings
void Settings::loadPreset(QString name)
{

    presetSettings.channels.clear();//very important to avoid a stack...
    QList<Persistence::SettingsObject*> sections;
    sections.append(&presetSettings);


    //NEW COOL CONFIGURATOR STUFF INSIDE
    if(readPresetFile(sections,name))
        qInfo(jtConfigurator) << "Settings : Presets loaded :" << name;
        else
        qInfo(jtConfigurator) << "Settings : Presets couldn't be loaded :" << name;

}

QStringList Settings::getPresetList()
{

  return Configurator::getInstance()->getPresetFilesNames(false);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Settings::load()
{
    QList<Persistence::SettingsObject*> sections;
    sections.append(&audioSettings);
    sections.append(&midiSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
    sections.append(&inputsSettings);
    sections.append(&recordingSettings);
    sections.append(&privateServerSettings);


    //NEW COOL CONFIGURATOR STUFF
    readFile(Configurator::getInstance()->getAppType(), sections);
}

Settings::Settings()
    :fileDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))

{
    //qDebug() << "Settings in " << fileDir;
}

void Settings::save(Persistence::InputsSettings inputsSettings){
    this->inputsSettings = inputsSettings;
    QList<Persistence::SettingsObject*> sections;
    sections.append(&audioSettings);
    sections.append(&midiSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
    sections.append(&this->inputsSettings);
    sections.append(&recordingSettings);
    sections.append(&privateServerSettings);

    //NEW COOL CONFIGURATOR STUFF
    writeFile(Configurator::getInstance()->getAppType(), sections);

}

//PRESETS TEST
void Settings::savePresets(InputsSettings inputsSettings,QString name)
{

    //this->inputsSettings = inputsSettings;
    this->presetSettings.channels= inputsSettings.channels;
    this->presetSettings.names.append(name);
    QList<Persistence::SettingsObject*> sections;
     //sections.append(&audioSettings);
     //sections.append(&midiSettings);
     //sections.append(&windowSettings);
     //sections.append(&metronomeSettings);
     //sections.append(&vstSettings);
    sections.append(&this->presetSettings);
     //sections.append(&recordingSettings);
     //sections.append(&privateServerSettings);

    //NEW COOL CONFIGURATOR STUFF
    if(writePresetFile(sections,name))
    qInfo(jtConfigurator) << "Settings : Presets written :" << name;
    else
    qInfo(jtConfigurator) << "Settings : Presets couldn't be written :" << name;



}
void Settings::DeletePreset(QString name)
{
   Configurator::getInstance()->deletePreset(name);
}

Settings::~Settings(){

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString Settings::getLastUserName() const{
//    QJsonObject audioObject = instance->rootObject["User"].toObject();
//    if(audioObject.contains("name")){
//        return audioObject["name"].toString();
//    }
    return "no saved yet";
}

QString Settings::getTranslation() const
{
 return translation;
}


void Settings::storeLasUserName(QString userName) {
    Q_UNUSED(userName)
    //    QJsonObject object = instance->rootObject["User"].toObject();
//    object["name"] = userName;
//    save();
}
