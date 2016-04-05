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
#include "log/Logging.h"

using namespace Persistence;

#if defined(Q_OS_WIN64) || defined(Q_OS_MAC64)
QString Settings::fileName = "Jamtaba64.json";
#else
QString Settings::fileName = "Jamtaba.json";
#endif

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SettingsObject::SettingsObject(const QString &name) :
    name(name)
{
}

SettingsObject::~SettingsObject()
{
}

int SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                     int fallBackValue)
{
    if (json.contains(propertyName))
        return json[propertyName].toInt();
    return fallBackValue;
}

bool SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                      bool fallBackValue)
{
    if (json.contains(propertyName))
        return json[propertyName].toBool();
    return fallBackValue;
}

QString SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                         QString fallBackValue)
{
    if (json.contains(propertyName))
        return json[propertyName].toString();
    return fallBackValue;
}

float SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                       float fallBackValue)
{
    if (json.contains(propertyName))
        return (float)(json[propertyName].toDouble());
    return fallBackValue;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PrivateServerSettings::PrivateServerSettings() :
    SettingsObject("PrivateServer"),
    server("localhost"),
    serverPort(2049),
    password("")
{
}

void PrivateServerSettings::write(QJsonObject &out) const
{
    out["server"] = server;
    out["password"] = password;
    out["port"] = serverPort;
}

void PrivateServerSettings::read(const QJsonObject &in)
{
    server = getValueFromJson(in, "server", QString("localhost"));
    password = getValueFromJson(in, "password", QString(""));
    serverPort = getValueFromJson(in, "port", 2049);
}

void Settings::setPrivateServerData(const QString &server, int serverPort, const QString &password)
{
    privateServerSettings.server = server;
    privateServerSettings.serverPort = serverPort;
    privateServerSettings.password = password;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioSettings::AudioSettings() :
    SettingsObject("audio"),
    sampleRate(44100),
    bufferSize(128)
{
}

void AudioSettings::read(const QJsonObject &in)
{
    sampleRate = getValueFromJson(in, "sampleRate", 44100);
    bufferSize = getValueFromJson(in, "bufferSize", 128);
    firstIn = getValueFromJson(in, "firstIn", 0);
    firstOut = getValueFromJson(in, "firstOut", 0);
    lastIn = getValueFromJson(in, "lastIn", 0);
    lastOut = getValueFromJson(in, "lastOut", 0);
    audioDevice = getValueFromJson(in, "audioDevice", -1);
}

void AudioSettings::write(QJsonObject &out) const
{
    out["sampleRate"] = sampleRate;
    out["bufferSize"] = bufferSize;
    out["firstIn"] = firstIn;
    out["firstOut"] = firstOut;
    out["lastIn"] = lastIn;
    out["lastOut"] = lastOut;
    out["audioDevice"] = audioDevice;
}

// +++++++++++++++++++++++++++++
MidiSettings::MidiSettings() :
    SettingsObject("midi")
{
}

void MidiSettings::write(QJsonObject &out) const
{
    QJsonArray midiArray;
    foreach (bool state, inputDevicesStatus)
        midiArray.append(state);
    out["inputsState"] = midiArray;
}

void MidiSettings::read(const QJsonObject &in)
{
    inputDevicesStatus.clear();
    if (in.contains("inputsState")) {
        QJsonArray inputsArray = in["inputsState"].toArray();
        for (int i = 0; i < inputsArray.size(); ++i)
            inputDevicesStatus.append(inputsArray.at(i).toBool(true));
    }
}

// ++++++++++++++++++++++++++++++
RecordingSettings::RecordingSettings() :
    SettingsObject("recording"),
    saveMultiTracksActivated(false),
    recordingPath("")
{
}

void RecordingSettings::write(QJsonObject &out) const
{
    out["recordingPath"] = recordingPath;
    out["recordActivated"] = saveMultiTracksActivated;
}

void RecordingSettings::read(const QJsonObject &in)
{
    bool useDefaultRecordingPath = false;
    if (in.contains("recordingPath")) {
        recordingPath = in["recordingPath"].toString();
        QDir dir(recordingPath);
        if (recordingPath.isEmpty() || !dir.exists()) {
            // qWarning() << "Dir " << dir << " not exists, using the application directory to save multitracks!";
            useDefaultRecordingPath = true;
        }
    } else {
        useDefaultRecordingPath = true;
    }
    if (useDefaultRecordingPath) {
        QString userDocuments = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
        QDir pathDir(QDir::homePath());
        QDir documentsDir(pathDir.absoluteFilePath(userDocuments));
        recordingPath = QDir(documentsDir).absoluteFilePath("Jamtaba");
    }
    saveMultiTracksActivated = getValueFromJson(in, "recordActivated", false);
}

// +++++++++++++++++++++++++++++
MetronomeSettings::MetronomeSettings() :
    SettingsObject("metronome"),
    pan(0),
    gain(1),
    muted(false),
    usingCustomSounds(false),
    customPrimaryBeatAudioFile(""),
    customSecondaryBeatAudioFile("")
{
}

void MetronomeSettings::read(const QJsonObject &in)
{
    pan = getValueFromJson(in, "pan", (float)0);
    gain = getValueFromJson(in, "gain", (float)1);
    muted = getValueFromJson(in, "muted", false);
    usingCustomSounds = getValueFromJson(in, "usingCustomSounds", false);
    customPrimaryBeatAudioFile = getValueFromJson(in, "customPrimaryBeatAudioFile", QString(""));
    customSecondaryBeatAudioFile = getValueFromJson(in, "customSecondaryBeatAudioFile", QString(""));
    builtInMetronomeAlias = getValueFromJson(in, "builtInMetronome", QString("Default"));
}

void MetronomeSettings::write(QJsonObject &out) const
{
    out["pan"] = pan;
    out["gain"] = gain;
    out["muted"] = muted;
    out["usingCustomSounds"] = usingCustomSounds;
    out["customPrimaryBeatAudioFile"] = customPrimaryBeatAudioFile;
    out["customSecondaryBeatAudioFile"] = customSecondaryBeatAudioFile;
    out["builtInMetronome"] = builtInMetronomeAlias;
}

// +++++++++++++++++++++++++++
WindowSettings::WindowSettings() :
    SettingsObject("window"),
    maximized(false),
    fullViewMode(true),
    fullScreenViewMode(false)
{
}

void WindowSettings::read(const QJsonObject &in)
{
    maximized = getValueFromJson(in, "maximized", false);// not maximized as default
    fullViewMode = getValueFromJson(in, "fullView", true);// use full view mode as default
    fullScreenViewMode = getValueFromJson(in, "fullScreenView", false);// use normal mode as default;
    if (in.contains("location")) {
        QJsonObject locationObj = in["location"].toObject();
        location.setX(getValueFromJson(locationObj, "x", (float)0));
        location.setY(getValueFromJson(locationObj, "y", (float)0));
    }
}

void WindowSettings::write(QJsonObject &out) const
{
    out["maximized"] = maximized;
    out["fullView"] = fullViewMode;
    out["fullScreenView"] = fullScreenViewMode;
    QJsonObject locationObject;
    locationObject["x"] = this->location.x();
    locationObject["y"] = this->location.y();
    out["location"] = locationObject;
}

// +++++++++++++++++++++++++++++++++++++++
VstSettings::VstSettings() :
    SettingsObject("VST")
{
}

// VST JSON WRITER
void VstSettings::write(QJsonObject &out) const
{
    QJsonArray scanPathsArray;
    foreach (const QString &scanPath, foldersToScan)
        scanPathsArray.append(scanPath);
    out["scanPaths"] = scanPathsArray;

    QJsonArray cacheArray;
    foreach (const QString &pluginPath, cachedPlugins)
        cacheArray.append(pluginPath);
    out["cachedPlugins"] = cacheArray;

    QJsonArray BlackedArray;
    foreach (const QString &blackVst, blackedPlugins)
        BlackedArray.append(blackVst);
    out["BlackListPlugins"] = BlackedArray;
}

void VstSettings::read(const QJsonObject &in)
{
    foldersToScan.clear();
    if (in.contains("scanPaths")) {
        QJsonArray scanPathsArray = in["scanPaths"].toArray();
        for (int i = 0; i < scanPathsArray.size(); ++i)
            foldersToScan.append(scanPathsArray.at(i).toString());
    }
    cachedPlugins.clear();
    if (in.contains("cachedPlugins")) {
        QJsonArray cacheArray = in["cachedPlugins"].toArray();
        for (int x = 0; x < cacheArray.size(); ++x) {
            QString pluginFile = cacheArray.at(x).toString();
            if (QFile(pluginFile).exists()) // if a cached plugin is removed from the disk we need skip this file

                cachedPlugins.append(pluginFile);
        }
    }
    blackedPlugins.clear();
    if (in.contains("BlackListPlugins")) {
        QJsonArray cacheArray = in["BlackListPlugins"].toArray();
        for (int x = 0; x < cacheArray.size(); ++x)
            blackedPlugins.append(cacheArray.at(x).toString());
    }
}

// +++++++++++++++++++++++++++++++++++++++
Channel::Channel(const QString &name) :
    name(name)
{
}

Plugin::Plugin(const QString &path, bool bypassed, const QByteArray &data) :
    path(path),
    bypassed(bypassed),
    data(data)
{
}

Subchannel::Subchannel(int firstInput, int channelsCount, int midiDevice, int midiChannel,
                       float gain, int boost, float pan, bool muted, qint8 transpose, quint8 lowerMidiNote, quint8 higherMidiNote) :
    firstInput(firstInput),
    channelsCount(channelsCount),
    midiDevice(midiDevice),
    midiChannel(midiChannel),
    gain(gain),
    boost(boost),
    pan(pan),
    muted(muted),
    transpose(transpose),
    lowerMidiNote(lowerMidiNote),
    higherMidiNote(higherMidiNote)
{

}

LocalInputTrackSettings::LocalInputTrackSettings(bool createOneTrack) :
    SettingsObject("inputs")
{
    if (createOneTrack) {
        // create a default channel
        Channel channel("my channel");
        qint8 transpose = 0;
        quint8 lowerNote = 0;
        quint8 higherNote = 127;
        Subchannel subchannel(0, 2, -1, -1, 1.0f, 1.0f, 0.0f, false, transpose, lowerNote, higherNote);
        channel.subChannels.append(subchannel);
        this->channels.append(channel);
    }
}

void LocalInputTrackSettings::write(QJsonObject &out) const
{
    QJsonArray channelsArray;
    foreach (const Channel &channel, channels) {
        QJsonObject channelObject;
        channelObject["name"] = channel.name;
        QJsonArray subchannelsArrays;
        foreach (const Subchannel &sub, channel.subChannels) {
            QJsonObject subChannelObject;
            subChannelObject["firstInput"] = sub.firstInput;
            subChannelObject["channelsCount"] = sub.channelsCount;
            subChannelObject["midiDevice"] = sub.midiDevice;
            subChannelObject["midiChannel"] = sub.midiChannel;
            subChannelObject["gain"] = sub.gain;
            subChannelObject["boost"] = sub.boost;
            subChannelObject["pan"] = sub.pan;
            subChannelObject["muted"] = sub.muted;
            subChannelObject["transpose"] = sub.transpose;
            subChannelObject["lowerNote"] = sub.lowerMidiNote;
            subChannelObject["higherNote"] = sub.higherMidiNote;

            QJsonArray pluginsArray;
            foreach (const Persistence::Plugin &plugin, sub.getPlugins()) {
                QJsonObject pluginObject;
                pluginObject["path"] = plugin.path;
                pluginObject["bypassed"] = plugin.bypassed;
                pluginObject["data"] = QString(plugin.data.toBase64());
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

void LocalInputTrackSettings::read(const QJsonObject &in, bool allowMultiSubchannels)
{
    if (in.contains("channels")) {
        QJsonArray channelsArray = in["channels"].toArray();
        for (int i = 0; i < channelsArray.size(); ++i) {
            QJsonObject channelObject = channelsArray.at(i).toObject();
            Persistence::Channel channel(getValueFromJson(channelObject, "name", QString("")));
            if (channelObject.contains("subchannels")) {
                QJsonArray subChannelsArray = channelObject["subchannels"].toArray();
                int subChannelsLimit = allowMultiSubchannels ? subChannelsArray.size() : 1;
                for (int k = 0; k < subChannelsLimit; ++k) {
                    QJsonObject subChannelObject = subChannelsArray.at(k).toObject();
                    int firstInput = getValueFromJson(subChannelObject, "firstInput", 0);
                    int channelsCount = getValueFromJson(subChannelObject, "channelsCount", 0);
                    int midiDevice = getValueFromJson(subChannelObject, "midiDevice", -1);
                    int midiChannel = getValueFromJson(subChannelObject, "midiChannel", -1);
                    float gain = getValueFromJson(subChannelObject, "gain", (float)1);
                    int boost = getValueFromJson(subChannelObject, "boost", (int)0);
                    float pan = getValueFromJson(subChannelObject, "pan", (float)0);
                    bool muted = getValueFromJson(subChannelObject, "muted", false);
                    qint8 transpose = getValueFromJson(subChannelObject, "transpose", (qint8)0);
                    quint8 lowerNote = getValueFromJson(subChannelObject, "lowerNote", (quint8)0);
                    quint8 higherNote = getValueFromJson(subChannelObject, "higherNote", (quint8)127);

                    QList<Plugin> plugins;
                    if (subChannelObject.contains("plugins")) {
                        QJsonArray pluginsArray = subChannelObject["plugins"].toArray();
                        for (int p = 0; p < pluginsArray.size(); ++p) {
                            QJsonObject pluginObject = pluginsArray.at(p).toObject();
                            QString pluginPath
                                = getValueFromJson(pluginObject, "path", QString(""));
                            bool bypassed = getValueFromJson(pluginObject, "bypassed", false);
                            QString dataString
                                = getValueFromJson(pluginObject, "data", QString(""));
                            if (!pluginPath.isEmpty() && QFile(pluginPath).exists()) {
                                QByteArray rawByteArray(dataString.toStdString().c_str());
                                plugins.append(Persistence::Plugin(pluginPath, bypassed,
                                                                   QByteArray::fromBase64(
                                                                       rawByteArray)));
                            }
                        }
                    }
                    Persistence::Subchannel subChannel(firstInput, channelsCount, midiDevice,
                                                       midiChannel, gain, boost, pan, muted, transpose, lowerNote, higherNote);
                    subChannel.setPlugins(plugins);
                    channel.subChannels.append(subChannel);
                }
                this->channels.append(channel);
            }
        }
    }
}

void LocalInputTrackSettings::read(const QJsonObject &in)
{
    read(in, true);// allowing multi subchannel by default
}

// ++++++++++++++++++++++++++++++++++++++++++
void Settings::setUserName(const QString &newUserName)
{
    this->lastUserName = newUserName;
}

void Settings::setTranslation(const QString &translate)
{
    this->translation = translate;
}

void Settings::addVstPlugin(const QString &pluginPath)
{
    if (!vstSettings.cachedPlugins.contains(pluginPath))
        vstSettings.cachedPlugins.append(pluginPath);
}

void Settings::addVstToBlackList(const QString &pluginPath)
{
    if (!vstSettings.blackedPlugins.contains(pluginPath))
        vstSettings.blackedPlugins.append(pluginPath);
}

void Settings::removeVstFromBlackList(const QString &pluginPath)
{
    vstSettings.blackedPlugins.removeOne(pluginPath);
}

QStringList Settings::getVstPluginsPaths() const
{
    return vstSettings.cachedPlugins;
}

void Settings::clearVstCache()
{
    vstSettings.cachedPlugins.clear();
}

// CLEAR VST BLACKBOX
void Settings::clearBlackBox()
{
    vstSettings.blackedPlugins.clear();
}

// VST paths to scan
void Settings::addVstScanPath(const QString &path)
{
    vstSettings.foldersToScan.append(path);
}

void Settings::removeVstScanPath(const QString &path)
{
    vstSettings.foldersToScan.removeOne(path);
}

QStringList Settings::getVstScanFolders() const
{
    return vstSettings.foldersToScan;
}

QStringList Settings::getBlackListedPlugins() const
{
    return vstSettings.blackedPlugins;
}

// ++++++++++++++++++

// +++++++++++++++++++++++++++++
void Settings::setMetronomeSettings(float gain, float pan, bool muted)
{
    metronomeSettings.pan = pan;
    metronomeSettings.gain = gain;
    metronomeSettings.muted = muted;
}

void Settings::setBuiltInMetronome(const QString &metronomeAlias)
{
    metronomeSettings.builtInMetronomeAlias = metronomeAlias;
    metronomeSettings.usingCustomSounds = false;
}


void Settings::setMetronomeFirstBeatAudioFile(const QString &filePath)
{
    if (QFileInfo(filePath).exists()){
        metronomeSettings.customPrimaryBeatAudioFile = filePath;
        metronomeSettings.usingCustomSounds = true;
    }
    else{
        metronomeSettings.customPrimaryBeatAudioFile = "";
        metronomeSettings.usingCustomSounds = false;
    }
}

void Settings::setMetronomeSecondaryBeatAudioFile(const QString &filePath)
{
    if (QFileInfo(filePath).exists()){
        metronomeSettings.customSecondaryBeatAudioFile = filePath;
        metronomeSettings.usingCustomSounds = true;
    }
    else{
        metronomeSettings.customSecondaryBeatAudioFile = "";
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Settings::setFullScreenView(bool v)
{
    windowSettings.fullScreenViewMode = v;
}

// +++++++++   Window Location  +++++++++++++++++++++++
void Settings::setWindowSettings(bool windowIsMaximized, bool usingFullView, QPointF location)
{
    double x = (location.x() >= 0) ? location.x() : 0;
    double y = (location.x() >= 0) ? location.y() : 0;
    if (x > 1)
        location.setX(0);
    if (y > 1)
        location.setY(0);
    windowSettings.location = location;
    windowSettings.maximized = windowIsMaximized;
    windowSettings.fullViewMode = usingFullView;
}

// ++++++++++++++++++++++++++++++++++++++++
void Settings::setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice)
{
    audioSettings.firstIn = firstIn;
    audioSettings.firstOut = firstOut;
    audioSettings.lastIn = lastIn;
    audioSettings.lastOut = lastOut;
    audioSettings.audioDevice = audioDevice;
}

void Settings::setSampleRate(int newSampleRate)
{
    audioSettings.sampleRate = newSampleRate;
}

void Settings::setBufferSize(int bufferSize)
{
    audioSettings.bufferSize = bufferSize;
}

// io ops ...
bool Settings::readFile(APPTYPE type, const QList<SettingsObject *> &sections)
{
    if (type == plugin)
        fileDir = Configurator::getInstance()->getPluginDirPath();
    else
        fileDir = Configurator::getInstance()->getHomeDirPath();

    QDir dir(fileDir);// homepath

    QString absolutePath = dir.absoluteFilePath(fileName);
    QFile f(absolutePath);
    if (f.open(QIODevice::ReadOnly)) {
        qInfo(jtConfigurator) << "Reading settings from " << f.fileName();
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonObject root = doc.object();

        if (root.contains("masterGain"))// read last master gain
            this->masterFaderGain = root["masterGain"].toDouble();
        else
            this->masterFaderGain = 1;// unit gain as default

        if (root.contains("userName"))// read user name
            this->lastUserName = root["userName"].toString();

        if (root.contains("translation"))// read Translation
            this->translation = root["translation"].toString();
        if (this->translation.isEmpty())
            this->translation = QLocale().bcp47Name().left(2);

        if (root.contains("intervalProgressShape"))// read intervall progress shape
            this->ninjamIntervalProgressShape = root["intervalProgressShape"].toInt(0);// zero as default value
        else
            this->ninjamIntervalProgressShape = 0;

        if (root.contains("tracksLayoutOrientation")) {
            int value = root["tracksLayoutOrientation"].toInt(2);// 2 is the Qt::Vertical value
            this->tracksLayoutOrientation = value == 2 ? Qt::Vertical : Qt::Horizontal;
        } else {
            this->tracksLayoutOrientation = Qt::Vertical;
        }

        if (root.contains("usingNarrowTracks"))
            this->usingNarrowedTracks = root["usingNarrowTracks"].toBool(false);
        else
            this->usingNarrowedTracks = false;

        // read settings sections (Audio settings, Midi settings, ninjam settings, etc...)
        foreach (SettingsObject *so, sections)
            so->read(root[so->getName()].toObject());
        return true;
    } else {
        qWarning(jtConfigurator) << "Settings : Can't load Jamtaba 2 config file:"
                                 << f.errorString();
    }

    return false;
}

bool Settings::writeFile(APPTYPE type, const QList<SettingsObject *> &sections)// io ops ...
{
    // Works with JTBConfig
    if (type == plugin)
        fileDir = Configurator::getInstance()->getPluginDirPath();
    QDir dir(fileDir);// homepath
    dir.mkpath(fileDir);
    // qWarning(jtConfigurator) << "SETTINGS WRITE JSON IN:" <<fileDir;
    QString absolutePath = dir.absoluteFilePath(fileName);
    QFile file(absolutePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject root;

        // writing global settings
        root["userName"] = lastUserName;// write user name
        root["translation"] = translation;// write translate locale
        root["intervalProgressShape"] = ninjamIntervalProgressShape;
        root["tracksLayoutOrientation"] = tracksLayoutOrientation;
        root["usingNarrowTracks"] = usingNarrowedTracks;
        root["masterGain"] = masterFaderGain;

        // write settings sections
        foreach (SettingsObject *so, sections) {
            QJsonObject sectionObject;
            so->write(sectionObject);
            root[so->getName()] = sectionObject;
        }
        QJsonDocument doc(root);
        file.write(doc.toJson());
        return true;
    } else {
        qCritical() << file.errorString();
    }
    return false;
}

// PRESETS
bool Settings::writePresetToFile(const Preset &preset)
{
    QString absolutePath = Configurator::getInstance()->getPresetPath(preset.name);
    QFile file(absolutePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject inputTracksJsonObject;
        preset.inputTrackSettings.write(inputTracksJsonObject);// write the channels and subchannels in the json object

        QJsonObject root;
        root[preset.name] = inputTracksJsonObject;

        QJsonDocument doc(root);
        file.write(doc.toJson());
        return true;
    } else {
        qCritical() << file.errorString();
    }
    return false;
}

// ++++++++++++++++++++++++++++++
Preset::Preset(const QString &name, const LocalInputTrackSettings &inputSettings) :
    name(name),
    inputTrackSettings(inputSettings)
{
}

// ++++++++++++++
Preset Settings::readPresetFromFile(const QString &presetFileName, bool allowMultiSubchannels)
{
    QString absolutePath = Configurator::getInstance()->getPresetPath(presetFileName);
    QFile presetFile(absolutePath);
    if (presetFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(presetFile.readAll());
        QJsonObject root = doc.object();
        QString presetName = "default";
        if (!root.keys().isEmpty()) {
            presetName = root.keys().first();
            Preset preset(presetName);
            preset.inputTrackSettings.read(root[presetName].toObject(), allowMultiSubchannels);
            return preset;
        }
    } else {
        qWarning(jtConfigurator) << "Settings : Can't load PRESET file:"
                                 << presetFile.errorString();
    }
    return Preset();// returning an empty/invalid preset
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Settings::load()
{
    QList<Persistence::SettingsObject *> sections;
    sections.append(&audioSettings);
    sections.append(&midiSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
    sections.append(&inputsSettings);
    sections.append(&recordingSettings);
    sections.append(&privateServerSettings);

    // NEW COOL CONFIGURATOR STUFF
    readFile(Configurator::getInstance()->getAppType(), sections);
}

Settings::Settings() :
    fileDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)),
    tracksLayoutOrientation(Qt::Vertical), masterFaderGain(1.0)
{
    // qDebug() << "Settings in " << fileDir;
}

void Settings::save(const LocalInputTrackSettings &localInputsSettings)
{
    this->inputsSettings = localInputsSettings;
    QList<Persistence::SettingsObject *> sections;
    sections.append(&audioSettings);
    sections.append(&midiSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
    sections.append(&inputsSettings);
    sections.append(&recordingSettings);
    sections.append(&privateServerSettings);

    writeFile(Configurator::getInstance()->getAppType(), sections);
}

void Settings::deletePreset(const QString &name)
{
    Configurator::getInstance()->deletePreset(name);
}

Settings::~Settings()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString Settings::getLastUserName() const
{
// QJsonObject audioObject = instance->rootObject["User"].toObject();
// if(audioObject.contains("name")){
// return audioObject["name"].toString();
// }
    return "no saved yet";
}

QString Settings::getTranslation() const
{
    return translation;
}

void Settings::storeLasUserName(const QString &userName)
{
    Q_UNUSED(userName)
    // QJsonObject object = instance->rootObject["User"].toObject();
// object["name"] = userName;
// save();
}
