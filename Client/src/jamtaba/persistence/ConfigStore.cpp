#include "ConfigStore.h"
#include <QDebug>
#include <QApplication>

using namespace Persistence;

const ConfigStore* ConfigStore::instance = new ConfigStore();

//++++++++++++++++++

const QString ConfigStore::KEY_LAST_USER_NAME = "USER/userName";
const QString ConfigStore::KEY_LAST_CHANNEL_NAME = "USER/channelName";

//audio
const QString ConfigStore::KEY_LAST_BUFFER_SIZE = "GLOBAL_AUDIO_PREFERENCES/bufferSize";
const QString ConfigStore::KEY_LAST_SAMPLE_RATE = "GLOBAL_AUDIO_PREFERENCES/sampleRate";
const QString ConfigStore::KEY_LAST_INPUT_DEVICE = "GLOBAL_AUDIO_PREFERENCES/inputDevice";
const QString ConfigStore::KEY_LAST_OUTPUT_DEVICE = "GLOBAL_AUDIO_PREFERENCES/outputDevice";
const QString ConfigStore::KEY_FIRST_GLOBAL_AUDIO_INPUT = "GLOBAL_AUDIO_PREFERENCES/firstIn";
const QString ConfigStore::KEY_FIRST_GLOBAL_AUDIO_OUTPUT = "GLOBAL_AUDIO_PREFERENCES/firstOut";
const QString ConfigStore::KEY_LAST_GLOBAL_AUDIO_INPUT = "GLOBAL_AUDIO_PREFERENCES/lastIn";
const QString ConfigStore::KEY_LAST_GLOBAL_AUDIO_OUTPUT = "GLOBAL_AUDIO_PREFERENCES/lastOut";

//MIDI
const QString ConfigStore::KEY_MIDI_INPUT = "MIDI/inputDevice";

//window
const QString ConfigStore::KEY_WINDOW_MAXIMIZED = "GUI/windowMaximized";
const QString ConfigStore::KEY_WINDOW_LOCATION = "GUI/windowLocation";
// const QString ConfigStore::KEY_UI_DETAILS = "advancedUI";

//ninjam
const QString ConfigStore::KEY_LAST_PRIVATE_SERVER_NAME = "NINJAM PRIVATE SERVER/name";
const QString ConfigStore::KEY_LAST_PRIVATE_SERVER_PORT = "NINJAM PRIVATE SERVER/port";
const QString ConfigStore::KEY_LAST_PRIVATE_SERVER_PASS = "NINJAM PRIVATE SERVER/password";
const QString ConfigStore::KEY_METRONOME_GAIN = "METRONOME/gain";
const QString ConfigStore::KEY_METRONOME_PAN = "METRONOME/pan";

//record
const QString ConfigStore::KEY_RECORD_PATH = "RECORD/path";
const QString ConfigStore::KEY_RECORD_STATUS = "RECORD/status";

const QString DEFAULT_RECORD_PATH = "./Recorded Jams";

QString ConfigStore::getSettingsFilePath(){
    return instance->settings.fileName();
}

//++++++++++++++++++

void ConfigStore::addVstPlugin(QString pluginPath){
    static int addedVSTs = 0;
    instance->settings.beginWriteArray("VSTs");
    instance->settings.setArrayIndex(addedVSTs++);
    instance->settings.setValue("path", pluginPath);
    instance->settings.endArray();
}

QStringList ConfigStore::getVstPluginsPaths(){
    int lenght = instance->settings.beginReadArray("VSTs");
    QStringList list;
    for (int i = 0; i < lenght; ++i) {
        instance->settings.setArrayIndex(i);
        QString path = instance->settings.value("path").toString();
        list.append(path);
    }
    instance->settings.endArray();
    return list;
}


void ConfigStore::clearVstCache(){
    instance->settings.remove("VSTs");
    instance->settings.sync();
}

//VST paths to scan
void ConfigStore::addVstScanPath(QString path){
    int newPathIndex = instance->settings.beginReadArray("VST_Paths_to_Scan");
    instance->settings.endArray();

    instance->settings.beginWriteArray("VST_Paths_to_Scan");
    instance->settings.setArrayIndex(newPathIndex);
    instance->settings.setValue("pathToScan", path);
    instance->settings.endArray();
}

void ConfigStore::removeVstScanPath(int index){
    QStringList allPathsToScan = getVstScanPaths();
    if(index >= 0 && index < allPathsToScan.size()){
        allPathsToScan.removeAt(index);

        //write all list entries
        instance->settings.beginWriteArray("VST_Paths_to_Scan", allPathsToScan.count());
        for (int i = 0; i < allPathsToScan.size(); ++i) {
            instance->settings.setArrayIndex(i);
            instance->settings.setValue("pathToScan", allPathsToScan.at(i));
        }
        instance->settings.endArray();
    }
}

QStringList ConfigStore::getVstScanPaths(){
    int size = instance->settings.beginReadArray("VST_Paths_to_Scan");
    QStringList list;
    for (int i = 0; i < size; ++i) {
        instance->settings.setArrayIndex(i);
        QString path = instance->settings.value("pathToScan").toString();
        list.append(path);
    }
    instance->settings.endArray();
    return list;
}

//++++++++++++++++++


int ConfigStore::getLastSampleRate(){
    bool ok;
    int result = readProperty(ConfigStore::KEY_LAST_SAMPLE_RATE).toInt(&ok);
    return ok ? result : -1;//use system sample rate
}

int ConfigStore::getLastBufferSize() {
    bool ok;
    int result = readProperty(ConfigStore::KEY_LAST_BUFFER_SIZE).toInt(&ok);
    return ok ? result :  128;
}

//+++++++++++++++++++++++++++++
void ConfigStore::storeMetronomeSettings(float gain, float pan){
    saveProperty(ConfigStore::KEY_METRONOME_GAIN, gain);
    saveProperty(ConfigStore::KEY_METRONOME_PAN, pan);
}

float ConfigStore::getMetronomeGain(){
    bool ok;
    float result = readProperty(ConfigStore::KEY_METRONOME_GAIN).toFloat(&ok);
    return ok ? result : 1;
}

float ConfigStore::getMetronomePan(){
    bool ok;
    float result = readProperty(ConfigStore::KEY_METRONOME_PAN).toFloat(&ok);
    return ok ? result : 0;//center
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ConfigStore::storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, int midiDevice){
    saveProperty(KEY_FIRST_GLOBAL_AUDIO_INPUT,  firstIn);
    saveProperty(KEY_LAST_GLOBAL_AUDIO_INPUT ,  lastIn);
    saveProperty(KEY_FIRST_GLOBAL_AUDIO_OUTPUT, firstOut);
    saveProperty(KEY_LAST_GLOBAL_AUDIO_OUTPUT,  lastOut);
    saveProperty(KEY_LAST_INPUT_DEVICE,  inputDevice);
    saveProperty(KEY_LAST_OUTPUT_DEVICE, outputDevice);
    saveProperty(KEY_LAST_SAMPLE_RATE, sampleRate);
    saveProperty(KEY_LAST_BUFFER_SIZE, bufferSize);
    saveProperty(KEY_MIDI_INPUT, midiDevice);
}


int ConfigStore::getLastMidiDeviceIndex(){
    return readProperty(KEY_MIDI_INPUT).toInt();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++   Window Location  +++++++++++++++++++++++
void ConfigStore::storeWindowLocation(QPointF location) {
    double x = (location.x() >= 0) ? location.x() : 0;
    double y = (location.x() >= 0) ? location.y() : 0;
    if (x > 1) {
        location.setX( 0);
    }
    if (y > 1) {
        location.setY(0);
    }
    saveProperty(ConfigStore::KEY_WINDOW_LOCATION, location);
}

QPointF ConfigStore::getLastWindowLocation() {
    return readProperty(ConfigStore::KEY_WINDOW_LOCATION).toPointF();
}

//    //+++++++++   Window Size +++++++++++++++++++++++
void ConfigStore::storeWindowState(bool windowIsMaximized) {
    saveProperty(ConfigStore::KEY_WINDOW_MAXIMIZED, windowIsMaximized);
}

bool ConfigStore::windowWasMaximized() {
    return readProperty(ConfigStore::KEY_WINDOW_MAXIMIZED).toBool();
}
//++++++++++++++++++++++++++++++++++++++++
int ConfigStore::getFirstGlobalAudioInput() {
    return readProperty(ConfigStore::KEY_FIRST_GLOBAL_AUDIO_INPUT).toInt();
}

int ConfigStore::getLastGlobalAudioInput(){
    return readProperty(ConfigStore::KEY_LAST_GLOBAL_AUDIO_INPUT).toInt();
}

int ConfigStore::getFirstGlobalAudioOutput() {
    return readProperty(ConfigStore::KEY_FIRST_GLOBAL_AUDIO_OUTPUT).toInt();
}

int ConfigStore::getLastGlobalAudioOutput(){
    return readProperty(ConfigStore::KEY_LAST_GLOBAL_AUDIO_OUTPUT).toInt();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ConfigStore::ConfigStore()
    :
      settings(QSettings::IniFormat, QSettings::UserScope, "Jamtaba 2", "jamtaba")
{

}

ConfigStore::~ConfigStore(){
    //delete settings;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString ConfigStore::getLastUserName() {
    return readProperty(ConfigStore::KEY_LAST_USER_NAME).toString();
}

QString ConfigStore::getLastChannelName() {
    QString channelName = readProperty(ConfigStore::KEY_LAST_CHANNEL_NAME).toString();
    if (channelName.isNull() || channelName.isEmpty()) {
        return "channel name";
    }
    return channelName;
}

int ConfigStore::getLastInputDevice(){
    bool ok;
    int result = readProperty(ConfigStore::KEY_LAST_INPUT_DEVICE).toInt(&ok);
    return ok ? result : -1;//use default system device
}

int ConfigStore::getLastOutputDevice() {
    bool ok;
    int result = readProperty(ConfigStore::KEY_LAST_OUTPUT_DEVICE).toInt(&ok);
    return ok ? result : -1;//use default system input device
}

void ConfigStore::storeLasUserName(QString userName) {
    saveProperty(ConfigStore::KEY_LAST_USER_NAME, userName);
}

void ConfigStore::storeLastChannelName(QString channelName) {
    saveProperty(ConfigStore::KEY_LAST_CHANNEL_NAME, channelName);
}

void ConfigStore::saveProperty(QString key, QVariant value) {
    //qDebug() << "saving property"<< key << "=>" << value;
    instance->settings.setValue(key, value);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QVariant ConfigStore::readProperty(QString key) {
    return instance->settings.value(key);
}
