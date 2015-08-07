#include "Settings.h"
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QList>
#include <QSettings>

using namespace Persistence;

QString Settings::fileName = "Jamtaba.json";

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


float SettingsObject::getValueFromJson(const QJsonObject &json, QString propertyName, float fallBackValue){
    if(json.contains(propertyName)){
        return (float)(json[propertyName].toDouble());
    }
    return fallBackValue;
}

QString SettingsObject::getValueFromJson(const QJsonObject &json, QString propertyName, QString fallBackValue){
    if(json.contains(propertyName)){
        return json[propertyName].toString();
    }
    return fallBackValue;
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
    inputDevice =   getValueFromJson(in, "inputDevice", -1);
    outputDevice =  getValueFromJson(in, "outputDevice", -1);
    midiDevice =    getValueFromJson(in, "midiDevice", -1);
}

void AudioSettings::write(QJsonObject &out){
    out["sampleRate"]   = sampleRate;
    out["bufferSize"]   = bufferSize;
    out["firstIn"]      = firstIn;
    out["firstOut"]     = firstOut;
    out["lastIn"]       = lastIn;
    out["lastOut"]      = lastOut;
    out["inputDevice"]  = inputDevice;
    out["outputDevice"] = outputDevice;
    out["midiDevice"]   = midiDevice;
}
//+++++++++++++++++++++++++++++
MetronomeSettings::MetronomeSettings()
    : SettingsObject("metronome"), pan(0), gain(0)
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
    : SettingsObject("window"), maximized(false){

}

void WindowSettings::read(QJsonObject in){
    maximized = getValueFromJson(in, "maximized", false);
    if(in.contains("location")){
        QJsonObject locationObj = in["location"].toObject();
        location.setX( getValueFromJson( locationObj, "x", (float)0));
        location.setY( getValueFromJson( locationObj, "y", (float)0));
    }
}

void WindowSettings::write(QJsonObject &out){
    out["maximized"] = maximized;
    QJsonObject locationObject;
    locationObject["x"] = this->location.x();
    locationObject["y"] = this->location.y();
    out["location"] = locationObject;
}

//+++++++++++++++++++++++++++++++++++++++
VstSettings::VstSettings()
    :SettingsObject("VST"){

}


void VstSettings::write(QJsonObject &out){
    QJsonArray scanPathsArray;
    foreach (QString scanPath, scanPaths) {
        scanPathsArray.append(scanPath);
    }
    out["scanPaths"] = scanPathsArray;

    QJsonArray cacheArray;
    foreach (QString pluginPath, cachedPlugins) {
        cacheArray.append(pluginPath);
    }
    out["cachedPlugins"] = cacheArray;
}

void VstSettings::read(QJsonObject in){
    scanPaths.clear();
    if(in.contains("scanPaths")){
        QJsonArray scanPathsArray = in["scanPaths"].toArray();
        for (int i = 0; i < scanPathsArray.size(); ++i) {
           scanPaths.append(scanPathsArray.at(i).toString());
        }
    }
    cachedPlugins.clear();
    if(in.contains("cachedPlugins")){
        QJsonArray cacheArray = in["cachedPlugins"].toArray();
        for (int x = 0; x < cacheArray.size(); ++x) {
            cachedPlugins.append(cacheArray.at(x).toString());
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++
Channel::Channel(QString name)
    :name(name){

}

Plugin::Plugin(QString path, bool bypassed)
    :path(path), bypassed(bypassed){

}

Subchannel::Subchannel(int firstInput, int channelsCount, bool isMidi, float gain, float pan, bool muted, QList<Plugin> plugins)
    :firstInput(firstInput), channelsCount(channelsCount),
      usingMidi(isMidi), gain(gain), pan(pan), muted(muted), plugins(plugins){

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
            subChannelObject["usingMidi"] = sub.usingMidi;
            subChannelObject["gain"] = sub.gain;
            subChannelObject["pan"] = sub.pan;
            subChannelObject["muted"] = sub.muted;

            QJsonArray pluginsArray;
            foreach (Persistence::Plugin plugin, sub.plugins) {
                QJsonObject pluginObject;
                pluginObject["path"] = plugin.path;
                pluginObject["bypassed"] = plugin.bypassed;
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
                    bool isMidi = getValueFromJson(subChannelObject, "usingMidi", false);
                    float gain = getValueFromJson(subChannelObject, "gain", (float)1);
                    float pan = getValueFromJson(subChannelObject, "pan", (float)0);
                    bool muted = getValueFromJson(subChannelObject, "muted", false);

                    QList<Plugin> plugins;
                    if(subChannelObject.contains("plugins")){
                        QJsonArray pluginsArray = subChannelObject["plugins"].toArray();
                        for (int p = 0; p < pluginsArray.size(); ++p) {
                            QJsonObject pluginObject = pluginsArray.at(p).toObject();
                            QString pluginPath = getValueFromJson(pluginObject, "path", QString(""));
                            bool bypassed = getValueFromJson(pluginObject, "bypassed", false);
                            if( !pluginPath.isEmpty() && QFile(pluginPath).exists() ){
                                plugins.append(Persistence::Plugin(pluginPath, bypassed));
                            }
                        }
                    }
                    Persistence::Subchannel subChannel(firstInput, channelsCount, isMidi, gain, pan, muted, plugins);
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

void Settings::addVstPlugin(QString pluginPath){
    vstSettings.cachedPlugins.append(pluginPath);
}

QStringList Settings::getVstPluginsPaths() const{
    return vstSettings.cachedPlugins;
}



void Settings::clearVstCache(){
    vstSettings.cachedPlugins.clear();
}

//VST paths to scan
void Settings::addVstScanPath(QString path){
    vstSettings.scanPaths.append(path);
}

void Settings::removeVstScanPath(int index){
    vstSettings.scanPaths.removeAt(index);
}

QStringList Settings::getVstScanPaths() const {

    return vstSettings.scanPaths;
}

//++++++++++++++++++

//+++++++++++++++++++++++++++++
void Settings::setMetronomeSettings(float gain, float pan, bool muted){
    metronomeSettings.pan = pan;
    metronomeSettings.gain = gain;
    metronomeSettings.muted = muted;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++   Window Location  +++++++++++++++++++++++
void Settings::setWindowSettings(bool windowIsMaximized, QPointF location){
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
}

//++++++++++++++++++++++++++++++++++++++++
void Settings::setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, int midiDevice){
    audioSettings.bufferSize = bufferSize;
    audioSettings.sampleRate = sampleRate;
    audioSettings.firstIn = firstIn;
    audioSettings.firstOut = firstOut;
    audioSettings.lastIn = lastIn;
    audioSettings.lastOut = lastOut;
    audioSettings.inputDevice = inputDevice;
    audioSettings.outputDevice = outputDevice;
    audioSettings.midiDevice = midiDevice;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Settings::load(){
    QList<Persistence::SettingsObject*> sections;
    sections.append(&audioSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
    sections.append(&inputsSettings);

    QDir dir(fileDir);
    QString absolutePath = dir.absoluteFilePath(fileName);
    QFile file(absolutePath);
    if(file.open(QIODevice::ReadOnly)){
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = doc.object();

        //read user name
        if(root.contains("userName")){
            this->lastUserName = root["userName"].toString();
        }

        //read intervall progress shape
        if(root.contains("intervalProgressShape")){
            this->ninjamIntervalProgressShape = root["intervalProgressShape"].toInt();
        }
        else{
            this->ninjamIntervalProgressShape = 0;
        }

        //read sections
        foreach (SettingsObject* so, sections) {
            so->read(root[so->getName()].toObject());
        }
    }
    else{
        qCritical() << file.errorString();
    }

}

Settings::Settings()
    :fileDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation))

{
}

void Settings::save(Persistence::InputsSettings inputsSettings){
    this->inputsSettings = inputsSettings;
    QList<Persistence::SettingsObject*> sections;
    sections.append(&audioSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
    sections.append(&this->inputsSettings);
    //++++++++++++++++++++++++++
    QDir dir(fileDir);
    dir.mkpath(fileDir);
    QString absolutePath = dir.absoluteFilePath(fileName);
    QFile file(absolutePath);
    if(file.open(QIODevice::WriteOnly)){
        QJsonObject root;
        //write user name
        root["userName"] = this->lastUserName;
        root["intervalProgressShape"] = this->ninjamIntervalProgressShape;

        //write sections
        foreach (SettingsObject* so, sections) {
            QJsonObject sectionObject;
            so->write(sectionObject);
            root[so->getName()] = sectionObject;
        }
        QJsonDocument doc(root);
        file.write(doc.toJson());
    }
    else{
        qCritical() << file.errorString();
    }
}

Settings::~Settings(){

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString Settings::getLastUserName() const{
//    QJsonObject audioObject = instance->rootObject["User"].toObject();
//    if(audioObject.contains("name")){
//        return audioObject["name"].toString();
//    }
    return "no save yet";
}


void Settings::storeLasUserName(QString userName) {
//    QJsonObject object = instance->rootObject["User"].toObject();
//    object["name"] = userName;
//    save();
}
