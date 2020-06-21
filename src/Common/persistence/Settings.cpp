#include "Settings.h"
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDir>
#include <QList>
#include <QStringList>
#include <QSettings>
#include "log/Logging.h"
#include "audio/vorbis/Vorbis.h"

using namespace persistence;

#if defined(Q_OS_WIN64) || defined(Q_OS_MAC64)
QString Settings::fileName = "Jamtaba64.json";
#else
QString Settings::fileName = "Jamtaba.json";
#endif

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SettingsObject::SettingsObject(const QString &name) :
    name(name)
{
    qCDebug(jtSettings) << "SettingsObject ctor";
}

SettingsObject::~SettingsObject()
{
    // qCDebug(jtSettings) << "SettingsObject destroyed";
}

int SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                     int fallBackValue)
{
    auto val = json.contains(propertyName) ? json[propertyName].toInt() : fallBackValue;
    qCDebug(jtSettings) << "SettingsObject getValueFromJson: " << propertyName << " = " << val;

    return val;
}

bool SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                      bool fallBackValue)
{
    auto val = json.contains(propertyName) ? json[propertyName].toBool() : fallBackValue;
    qCDebug(jtSettings) << "SettingsObject getValueFromJson: " << propertyName << " = " << val;

    return val;
}

QString SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                         QString fallBackValue)
{
    auto val = json.contains(propertyName) ? json[propertyName].toString() : fallBackValue;
    qCDebug(jtSettings) << "SettingsObject getValueFromJson: " << propertyName << " = " << val;
    return val;
}

float SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                       float fallBackValue)
{
    auto val = json.contains(propertyName) ? static_cast<float>(json[propertyName].toDouble()) : fallBackValue;
    qCDebug(jtSettings) << "SettingsObject getValueFromJson: " << propertyName << " = " << val;
    return val;
}

QJsonArray SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                           QJsonArray fallBackValue)
{
    qCDebug(jtSettings) << "SettingsObject getValueFromJson: propertyName = " << propertyName;

    if (json.contains(propertyName))
        return json[propertyName].toArray();

    return fallBackValue;
}

QJsonObject SettingsObject::getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                           QJsonObject fallBackValue)
{
    qCDebug(jtSettings) << "SettingsObject getValueFromJson: propertyName = " << propertyName;

    if (json.contains(propertyName))
        return json[propertyName].toObject();

    return fallBackValue;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LooperSettings::LooperSettings() :
    SettingsObject("Looper"),
    preferredLayersCount(4),
    preferredMode(0),
    loopsFolder(""),
    encodingAudioWhenSaving(false),
    waveFilesBitDepth(16) // 16 bits
{
    qCDebug(jtSettings) << "LooperSettings ctor";
    setDefaultLooperFilesPath();
}

void LooperSettings::read(const QJsonObject &in)
{
    preferredLayersCount = getValueFromJson(in, "preferresLayersCount", (quint8)4); // 4 layers as default value
    preferredMode = getValueFromJson(in, "preferredMode", (quint8)0); // use the first mode as default value
    loopsFolder = getValueFromJson(in, "loopsFolder", QString());
    encodingAudioWhenSaving = getValueFromJson(in, "encodeAudio", false);
    waveFilesBitDepth = getValueFromJson(in, "bitDepth", quint8(16)); // 16 bit as default value

    if (!(waveFilesBitDepth == 16 || waveFilesBitDepth == 32)) {
        qWarning() << "Invalid bit depth " << waveFilesBitDepth << ", using 16 bits as default value";
        waveFilesBitDepth = 16;
    }

    bool useDefaultSavePath = false;
    if (!loopsFolder.isEmpty()) {
        QDir saveDir(QDir::fromNativeSeparators(loopsFolder));
        if (!saveDir.exists()) {
            qDebug() << "Creating looper save dir " << saveDir;
            saveDir.mkpath(".");
        }

        if (loopsFolder.isEmpty() || !saveDir.exists()) {
            qWarning() << "Dir " << saveDir << " not exists, using the application directory to save looper data!";
            useDefaultSavePath = true;
        }
    }
    else {
        useDefaultSavePath = true;
    }

    if (useDefaultSavePath) {
       setDefaultLooperFilesPath();
    }

    qCDebug(jtSettings) << "LooperSettings: preferredLayersCount " << preferredLayersCount
                    << "; preferredMode " << preferredMode
                    << "; loopsFolder " << loopsFolder
                    << " (useDefaultSavePath " << useDefaultSavePath << ")"
                    << "; encodingAudioWhenSaving " << encodingAudioWhenSaving
                    << "; waveFilesBitDepth " << waveFilesBitDepth;

}

void LooperSettings::setDefaultLooperFilesPath()
{
    qCDebug(jtSettings) << "LooperSettings setDefaultLooperFilesPath";

    QString userDocuments = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
    QDir pathDir(QDir::homePath());
    QDir documentsDir(pathDir.absoluteFilePath(userDocuments));
    loopsFolder = QDir(documentsDir).absoluteFilePath("JamTaba/Looper");
    QDir saveDir(loopsFolder);
    if (!saveDir.exists()) {
        saveDir.mkpath(".");
        qDebug() << "Creating looper data folder " << saveDir;
    }
}

void LooperSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "LooperSettings write";
    out["preferresLayersCount"] = preferredLayersCount;
    out["preferredMode"] = preferredMode;
    out["loopsFolder"] = loopsFolder;
    out["encodeAudio"] = encodingAudioWhenSaving;

    if (!encodingAudioWhenSaving)
        out["bitDepth"] = waveFilesBitDepth;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

PrivateServerSettings::PrivateServerSettings() :
    SettingsObject("PrivateServer")
{
    qCDebug(jtSettings) << "PrivateServerSettings ctor";
    addPrivateServerData("localhost", 2049);
}

void PrivateServerSettings::addPrivateServerData(const QString &serverName, int serverPort, const QString &password)
{
    qCDebug(jtSettings) << "PrivateServerSettings addPrivateServerData";
    if (lastServers.contains(serverName))
        lastServers.removeOne(serverName);

    lastServers.insert(0, serverName); // the last used server is the first element in the list

    lastPort = serverPort;
    lastPassword = password;
}

void PrivateServerSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "PrivateServerSettings write";
    QJsonArray serversArray;
    for (const QString &server : lastServers) {
        serversArray.append(server);
    }
    out["lastPort"] = lastPort;
    out["lastPassword"] = lastPassword;
    out["lastServers"] = serversArray;
}

void PrivateServerSettings::read(const QJsonObject &in)
{
    if (in.contains("lastServers")) {
        lastServers.clear();
        QJsonArray serversArray = in["lastServers"].toArray();
        for (int serverIndex = 0; serverIndex < serversArray.size(); ++serverIndex) {
            QString serverName = serversArray.at(serverIndex).toString();
            if (!serverName.isEmpty() && !lastServers.contains(serverName))
                lastServers.append(serverName);
        }
    }

    lastPort = getValueFromJson(in, "lastPort", (int)2049);
    lastPassword = getValueFromJson(in, "lastPassword", QString(""));

    qCDebug(jtSettings) << "PrivateServerSettings: lastServers " << lastServers
                        << "; lastPort " << lastPort
                        << "; lastPassword " << lastPassword;
}

void Settings::addPrivateServer(const QString &serverName, int serverPort, const QString &password)
{
    qCDebug(jtSettings) << "PrivateServerSettings addPrivateServer";
    privateServerSettings.addPrivateServerData(serverName, serverPort, password);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

AudioSettings::AudioSettings() :
    SettingsObject("audio"),
    sampleRate(44100),
    bufferSize(128),
    encodingQuality(vorbis::EncoderQualityNormal),
    firstIn(-1),
    firstOut(-1),
    lastIn(-1),
    lastOut(-1),
    audioInputDevice(""),
    audioOutputDevice("")
{
    qCDebug(jtSettings) << "AudioSettings ctor";
}

void AudioSettings::read(const QJsonObject &in)
{
    sampleRate = getValueFromJson(in, "sampleRate", 44100);
    bufferSize = getValueFromJson(in, "bufferSize", 128);
    firstIn = getValueFromJson(in, "firstIn", 0);
    firstOut = getValueFromJson(in, "firstOut", 0);
    lastIn = getValueFromJson(in, "lastIn", 0);
    lastOut = getValueFromJson(in, "lastOut", 0);
    audioInputDevice = getValueFromJson(in, "audioInputDevice", QString(""));
    audioOutputDevice = getValueFromJson(in, "audioOutputDevice", QString(""));

    encodingQuality = getValueFromJson(in, "encodingQuality", vorbis::EncoderQualityNormal); // using normal quality as fallback value.

    // ensure vorbis quality is in accepted range
    if (encodingQuality < vorbis::EncoderQualityLow)
        encodingQuality = vorbis::EncoderQualityLow;
    else if(encodingQuality > vorbis::EncoderQualityHigh)
        encodingQuality = vorbis::EncoderQualityHigh;

    qCDebug(jtSettings) << "AudioSettings: sampleRate " << sampleRate
                        << "; bufferSize " << bufferSize
                        << "; firstIn " << firstIn
                        << "; firstOut " << firstOut
                        << "; lastIn " << lastIn
                        << "; lastOut " << lastOut
                        << "; audioInputDevice " << audioInputDevice
                        << "; audioOutputDevice " << audioOutputDevice
                        << "; encodingQuality " << encodingQuality;
}

void AudioSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "AudioSettings write";
    out["sampleRate"] = sampleRate;
    out["bufferSize"] = bufferSize;
    out["firstIn"] = firstIn;
    out["firstOut"] = firstOut;
    out["lastIn"] = lastIn;
    out["lastOut"] = lastOut;

    out["audioInputDevice"] = audioInputDevice;
    out["audioOutputDevice"] = audioOutputDevice;

    out["encodingQuality"] = encodingQuality;
}

// +++++++++++++++++++++++++++++

MidiSettings::MidiSettings() :
    SettingsObject("midi")
{
    qCDebug(jtSettings) << "MidiSettings ctor";
}

void MidiSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "MidiSettings write";
    QJsonArray midiArray;

    for (bool state : inputDevicesStatus)
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

    qCDebug(jtSettings) << "MidiSettings: inputDevicesStatus " << inputDevicesStatus;
}

// ++++++++++++++++++++++++++++++

SyncSettings::SyncSettings() :
    SettingsObject("sync")
{
    qCDebug(jtSettings) << "SyncSettings ctor";
}

void SyncSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "SyncSettings write";
    QJsonArray midiArray;

    for (bool state : syncOutputDevicesStatus)
        midiArray.append(state);

    out["syncOutputsState"] = midiArray;
}

void SyncSettings::read(const QJsonObject &in)
{
    syncOutputDevicesStatus.clear();
    if (in.contains("syncOutputsState")) {
        QJsonArray inputsArray = in["syncOutputsState"].toArray();
        for (int i = 0; i < inputsArray.size(); ++i)
            syncOutputDevicesStatus.append(inputsArray.at(i).toBool(true));
    }

    qCDebug(jtSettings) << "SyncSettings: syncOutputDevicesStatus " << syncOutputDevicesStatus;
}

// ++++++++++++++++++++++++++++++


MultiTrackRecordingSettings::MultiTrackRecordingSettings() :
    SettingsObject("recording"),
    saveMultiTracksActivated(false),
    jamRecorderActivated(QMap<QString, bool>()),
    recordingPath(""),
    dirNameDateFormat("Qt::TextDate")
{
    qCDebug(jtSettings) << "MultiTrackRecordingSettings ctor";
    // TODO: populate jamRecorderActivated with {jamRecorderId, false} pairs for each known jamRecorder
}

void MultiTrackRecordingSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "MultiTrackRecordingSettings write";
    out["recordingPath"] = QDir::toNativeSeparators(recordingPath);
    out["dirNameDateFormat"] = dirNameDateFormat;
    out["recordActivated"] = saveMultiTracksActivated;
    QJsonObject jamRecorders = QJsonObject();
    for (const QString &key : jamRecorderActivated.keys()) {
        QJsonObject jamRecorder = QJsonObject();
        jamRecorder["activated"] = jamRecorderActivated[key];
        jamRecorders[key] = jamRecorder;
    }
    out["jamRecorders"] = jamRecorders;
}

void MultiTrackRecordingSettings::read(const QJsonObject &in)
{
    bool useDefaultRecordingPath = false;
    if (in.contains("recordingPath")) {
        recordingPath = in["recordingPath"].toString();
        QDir dir(QDir::fromNativeSeparators(recordingPath));
        if (!dir.exists())
            dir.mkpath(".");

        if (recordingPath.isEmpty() || !dir.exists()) {
            qWarning() << "Dir " << dir << " not exists, using the application directory to save multitracks!";
            useDefaultRecordingPath = true;
        }
    } else {
        useDefaultRecordingPath = true;
    }

    if (useDefaultRecordingPath)
        recordingPath = MultiTrackRecordingSettings::getDefaultRecordingPath();

    if (in.contains("dirNameDateFormat")) {
        dirNameDateFormat = in["dirNameDateFormat"].toString();
    } else {
        dirNameDateFormat = "Qt::TextDate";
    }

    saveMultiTracksActivated = getValueFromJson(in, "recordActivated", false);

    QJsonObject jamRecorders = getValueFromJson(in, "jamRecorders", QJsonObject());
    for(const QString &key : jamRecorders.keys()) {
        QJsonObject jamRecorder = jamRecorders[key].toObject();
        jamRecorderActivated[key] = getValueFromJson(jamRecorder, "activated", false);
    }

    qCDebug(jtSettings) << "MultiTrackRecordingSettings: recordingPath " << recordingPath
                        << " (useDefaultRecordingPath " << useDefaultRecordingPath << ")"
                        << "; dirNameDateFormat " << dirNameDateFormat
                        << "; saveMultiTracksActivated " << saveMultiTracksActivated
                        << "; jamRecorderActivated " << jamRecorderActivated;
}

QString MultiTrackRecordingSettings::getDefaultRecordingPath()
{
    qCDebug(jtSettings) << "MultiTrackRecordingSettings getDefaultRecordingPath";
    QString userDocuments = QStandardPaths::displayName(QStandardPaths::DocumentsLocation);
    QDir pathDir(QDir::homePath());
    QDir documentsDir(pathDir.absoluteFilePath(userDocuments));
    QDir jamTabaDir = QDir(documentsDir).absoluteFilePath("JamTaba");

    return QDir(jamTabaDir).absoluteFilePath("Jams"); // using 'Jams' as default recording folder (issue #891)
}

// +++++++++++++++++++++++++++++

MetronomeSettings::MetronomeSettings() :
    SettingsObject("metronome"),
    pan(0),
    gain(1),
    muted(false),
    usingCustomSounds(false),
    customPrimaryBeatAudioFile(""),
    customOffBeatAudioFile(""),
    customAccentBeatAudioFile("")
{
    qCDebug(jtSettings) << "MetronomeSettings ctor";
    //
}

void MetronomeSettings::read(const QJsonObject &in)
{
    pan = getValueFromJson(in, "pan", (float)0);
    gain = getValueFromJson(in, "gain", (float)1);
    muted = getValueFromJson(in, "muted", false);
    usingCustomSounds = getValueFromJson(in, "usingCustomSounds", false);
    customPrimaryBeatAudioFile = getValueFromJson(in, "customPrimaryBeatAudioFile", QString(""));
    customOffBeatAudioFile = getValueFromJson(in, "customOffBeatAudioFile", getValueFromJson(in, "customSecondaryBeatAudioFile", QString(""))); // backward compatible
    customAccentBeatAudioFile = getValueFromJson(in, "customAccentBeatAudioFile", QString(""));
    builtInMetronomeAlias = getValueFromJson(in, "builtInMetronome", QString("Default"));

    qCDebug(jtSettings) << "MetronomeSettings: pan " << pan
                        << "; gain " << gain
                        << "; muted " << muted
                        << "; usingCustomSounds " << usingCustomSounds
                        << "; customPrimaryBeatAudioFile " << customPrimaryBeatAudioFile
                        << "; customOffBeatAudioFile " << customOffBeatAudioFile
                        << "; customAccentBeatAudioFile " << customAccentBeatAudioFile
                        << "; builtInMetronomeAlias " << builtInMetronomeAlias;
}

void MetronomeSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "MetronomeSettings write";
    out["pan"] = pan;
    out["gain"] = gain;
    out["muted"] = muted;
    out["usingCustomSounds"] = usingCustomSounds;
    out["customPrimaryBeatAudioFile"] = customPrimaryBeatAudioFile;
    out["customOffBeatAudioFile"] = customOffBeatAudioFile;
    out["customAccentBeatAudioFile"] = customAccentBeatAudioFile;
    out["builtInMetronome"] = builtInMetronomeAlias;
}

// +++++++++++++++++++++++++++

CollapseSettings::CollapseSettings() :
    SettingsObject("Collapse"),
    localChannelsCollapsed(false),
    bottomSectionCollapsed(false),
    chatSectionCollapsed(false)
{
    qCDebug(jtSettings) << "CollapseSettings ctor";
}

void CollapseSettings::read(const QJsonObject &in)
{
    localChannelsCollapsed = getValueFromJson(in, "localChannels", false);
    bottomSectionCollapsed = getValueFromJson(in, "bottomSection", false);
    chatSectionCollapsed = getValueFromJson(in, "chatSection", false);

    qCDebug(jtSettings) << "CollapseSettings: localChannelsCollapsed " << localChannelsCollapsed
                        << "; bottomSectionCollapsed" << bottomSectionCollapsed
                        << "; chatSectionCollapsed " << chatSectionCollapsed;
}

void CollapseSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "CollapseSettings write";
    out["localChannels"] = localChannelsCollapsed;
    out["bottomSection"] = bottomSectionCollapsed;
    out["chatSection"] = chatSectionCollapsed;
}

// ++++++++++++++++++++++

WindowSettings::WindowSettings() :
    SettingsObject("window"),
    maximized(false),
    fullScreenMode(false)
{
    qCDebug(jtSettings) << "WindowSettings ctor";
}

void WindowSettings::read(const QJsonObject &in)
{
    maximized = getValueFromJson(in, "maximized", false); // not maximized as default
    fullScreenMode = getValueFromJson(in, "fullScreenView", false);// use normal mode as default;
    if (in.contains("location")) {
        QJsonObject locationObj = in["location"].toObject();
        location.setX(getValueFromJson(locationObj, "x", (float)0));
        location.setY(getValueFromJson(locationObj, "y", (float)0));
    }

    if (in.contains("size")) {
        QJsonObject sizeObject = in["size"].toObject();
        size.setWidth(getValueFromJson(sizeObject, "width", (int)800));
        size.setHeight(getValueFromJson(sizeObject, "height", (int)600));
    }

    qCDebug(jtSettings) << "WindowSettings: maximized " << maximized
                        << "; fullScreenMode " << fullScreenMode
                        << "; location " << location
                        << "; size " << size;
}

void WindowSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "WindowSettings write";
    out["maximized"] = maximized;
    out["fullScreenView"] = fullScreenMode;

    QJsonObject locationObject;
    locationObject["x"] = this->location.x();
    locationObject["y"] = this->location.y();
    out["location"] = locationObject;

    QJsonObject sizeObject;
    sizeObject["width"] = this->size.width();
    sizeObject["height"] = this->size.height();
    out["size"] = sizeObject;
}

// +++++++++++++++++++++++++++++++++++++++

VstSettings::VstSettings() :
    SettingsObject("VST")
{
    qCDebug(jtSettings) << "VstSettings ctor";
}

// VST JSON WRITER
void VstSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "VstSettings write";
    QJsonArray scanPathsArray;

    for (const QString &scanPath : foldersToScan)
        scanPathsArray.append(scanPath);

    out["scanPaths"] = scanPathsArray;

    QJsonArray cacheArray;
    for (const QString &pluginPath : cachedPlugins)
        cacheArray.append(pluginPath);

    out["cachedPlugins"] = cacheArray;

    QJsonArray BlackedArray;
    for (const QString &blackVst: blackedPlugins)
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

    qCDebug(jtSettings) << "VstSettings: foldersToScan " << foldersToScan
                        << "; cachedPlugins " << cachedPlugins
                        << "; blackedPlugins " << blackedPlugins;
}

// +++++++++++++++++++++++++++++++++++++++

AudioUnitSettings::AudioUnitSettings() :
    SettingsObject("AU")
{
    qCDebug(jtSettings) << "AudioUnitSettings ctor";
}

// AU JSON WRITER
void AudioUnitSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "AudioUnitSettings write";
    QJsonArray cacheArray;
    for (const QString &pluginPath : cachedPlugins)
        cacheArray.append(pluginPath);

    out["cachedPlugins"] = cacheArray;
}

void AudioUnitSettings::read(const QJsonObject &in)
{
    cachedPlugins.clear();
    if (in.contains("cachedPlugins")) {
        QJsonArray cacheArray = in["cachedPlugins"].toArray();
        for (int x = 0; x < cacheArray.size(); ++x) {
            QString pluginFile = cacheArray.at(x).toString();
        }
    }
    qCDebug(jtSettings) << "AudioUnitSettings: cachedPlugins " << cachedPlugins;
}

// +++++++++++++++++++++++++++++++++++++++

Channel::Channel(int instrumentIndex) :
    instrumentIndex(instrumentIndex)
{
    qCDebug(jtSettings) << "Channel ctor";
}

Plugin::Plugin(const audio::PluginDescriptor &descriptor, bool bypassed, const QByteArray &data) :
    name(descriptor.getName()),
    path(descriptor.getPath()),
    manufacturer(descriptor.getManufacturer()),
    bypassed(bypassed),
    data(data),
    category(descriptor.getCategory())
{
    qCDebug(jtSettings) << "Plugin ctor";
}

SubChannel::SubChannel(int firstInput, int channelsCount, int midiDevice, int midiChannel,
                       float gain, int boost, float pan, bool muted, bool stereoInverted,
                       qint8 transpose, quint8 lowerMidiNote, quint8 higherMidiNote, bool routingMidiToFirstSubchannel) :
    firstInput(firstInput),
    channelsCount(channelsCount),
    midiDevice(midiDevice),
    midiChannel(midiChannel),
    gain(gain),
    boost(boost),
    pan(pan),
    muted(muted),
    stereoInverted(stereoInverted),
    transpose(transpose),
    lowerMidiNote(lowerMidiNote),
    higherMidiNote(higherMidiNote),
    routingMidiToFirstSubchannel(routingMidiToFirstSubchannel)
{
    qCDebug(jtSettings) << "SubChannel ctor";
}

LocalInputTrackSettings::LocalInputTrackSettings(bool createOneTrack) :
    SettingsObject("inputs")
{
    qCDebug(jtSettings) << "LocalInputTrackSettings ctor";
    if (createOneTrack) {
        // create a default channel
        Channel channel(-1); // default instrument icon
        qint8 transpose = 0;
        quint8 lowerNote = 0;
        quint8 higherNote = 127;
        int firstInput = 0;
        int channelsCount = 2;
        int midiDevice = -1;
        int midiChannel = -1;
        float gain = 1.0f;
        float boost = 1.0f;
        float pan = 0.0f;
        bool muted = false;
        bool stereoInverted = false;
        bool routingMidi = false;
        SubChannel subchannel(firstInput, channelsCount, midiDevice, midiChannel, gain, boost, pan, muted, stereoInverted, transpose, lowerNote, higherNote, routingMidi);
        channel.subChannels.append(subchannel);
        this->channels.append(channel);
    }
}

void LocalInputTrackSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "LocalInputTrackSettings write";
    QJsonArray channelsArray;
    for (const Channel &channel : channels) {
        QJsonObject channelObject;
        channelObject["instrument"] = channel.instrumentIndex;
        QJsonArray subchannelsArrays;
        int subchannelsCount = 0;
        for (const SubChannel &sub : channel.subChannels) {
            QJsonObject subChannelObject;
            subChannelObject["firstInput"]       = sub.firstInput;
            subChannelObject["channelsCount"]    = sub.channelsCount;
            subChannelObject["midiDevice"]       = sub.midiDevice;
            subChannelObject["midiChannel"]      = sub.midiChannel;
            subChannelObject["gain"]             = sub.gain;
            subChannelObject["boost"]            = sub.boost;
            subChannelObject["pan"]              = sub.pan;
            subChannelObject["muted"]            = sub.muted;
            subChannelObject["stereoInverted"]   = sub.stereoInverted;
            subChannelObject["transpose"]        = sub.transpose;
            subChannelObject["lowerNote"]        = sub.lowerMidiNote;
            subChannelObject["higherNote"]       = sub.higherMidiNote;

            if (subchannelsCount > 0) // skip midiRouting in first subchannel
                subChannelObject["routingMidiInput"] = sub.routingMidiToFirstSubchannel;

            QJsonArray pluginsArray;
            for (const auto &plugin : sub.getPlugins()) {
                QJsonObject pluginObject;
                pluginObject["name"]     = plugin.name;

                if (!plugin.path.isEmpty())
                    pluginObject["path"]     = plugin.path;

                pluginObject["bypassed"] = plugin.bypassed;

                if (!plugin.data.isEmpty())
                    pluginObject["data"]     = QString(plugin.data.toBase64());

                pluginObject["category"] = static_cast<quint8>(plugin.category);

                if (!plugin.manufacturer.isEmpty())
                    pluginObject["manufacturer"] = plugin.manufacturer;

                pluginsArray.append(pluginObject);
            }
            subChannelObject["plugins"] = pluginsArray;

            subchannelsArrays.append(subChannelObject);

            subchannelsCount++;
        }
        channelObject["subchannels"] = subchannelsArrays;
        channelsArray.append(channelObject);
    }
    out["channels"] = channelsArray;
}

Plugin LocalInputTrackSettings::jsonObjectToPlugin(QJsonObject pluginObject)
{
    qCDebug(jtSettings) << "LocalInputTrackSettings jsonObjectToPlugin";

    QString name = getValueFromJson(pluginObject, "name", QString());

    QString path = getValueFromJson(pluginObject, "path", QString());

    bool bypassed = getValueFromJson(pluginObject, "bypassed", false);

    QString dataString = getValueFromJson(pluginObject, "data", QString());

    auto category = static_cast<audio::PluginDescriptor::Category>(getValueFromJson(pluginObject, "category", quint8(1))); // 1 is the VST enum value

    QByteArray rawByteArray(dataString.toStdString().c_str());

    QString manufacturer = getValueFromJson(pluginObject, "manufacturer", QString());

    audio::PluginDescriptor descriptor(name, category, manufacturer, path);

    return persistence::Plugin(descriptor, bypassed, QByteArray::fromBase64(rawByteArray));
}

void LocalInputTrackSettings::read(const QJsonObject &in, bool allowSubchannels)
{
    qCDebug(jtSettings) << "LocalInputTrackSettings read (too complex to dump...)";

    if (in.contains("channels")) {
        QJsonArray channelsArray = in["channels"].toArray();
        for (int i = 0; i < channelsArray.size(); ++i) {

            QJsonObject channelObject = channelsArray.at(i).toObject();
            persistence::Channel channel(getValueFromJson(channelObject, "instrument", static_cast<int>(-1)));

            if (channelObject.contains("subchannels")) {

                QJsonArray subChannelsArray = channelObject["subchannels"].toArray();
                int subChannelsLimit = allowSubchannels ? subChannelsArray.size() : 1;

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
                    bool stereoInverted = getValueFromJson(subChannelObject, "stereoInverted", false);
                    qint8 transpose = getValueFromJson(subChannelObject, "transpose", (qint8)0);
                    quint8 lowerNote = getValueFromJson(subChannelObject, "lowerNote", (quint8)0);
                    quint8 higherNote = getValueFromJson(subChannelObject, "higherNote", (quint8)127);
                    bool routingMidi = k > 0 && getValueFromJson(subChannelObject, "routingMidiInput", false);

                    QList<Plugin> plugins;
                    if (subChannelObject.contains("plugins")) {

                        QJsonArray pluginsArray = subChannelObject["plugins"].toArray();

                        for (int p = 0; p < pluginsArray.size(); ++p) {
                            QJsonObject pluginObject = pluginsArray.at(p).toObject();
                            Plugin plugin = jsonObjectToPlugin(pluginObject);
                            bool pathIsValid = !plugin.path.isEmpty();
                            if (plugin.category == audio::PluginDescriptor::VST_Plugin)
                                pathIsValid = QFile(plugin.path).exists();

                            if (pathIsValid)
                                plugins.append(plugin);
                        }
                    }
                    persistence::SubChannel subChannel(firstInput, channelsCount, midiDevice,
                                                       midiChannel, gain, boost, pan, muted, stereoInverted, transpose, lowerNote, higherNote, routingMidi);
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
    read(in, true); // allowing multi subchannel by default
}

// ++++++++++++++++++++++++++++++++++++++++++

void Settings::storeUserName(const QString &newUserName)
{
    qCDebug(jtSettings) << "Settings storeUserName: from " << lastUserName << "; to " << newUserName;
    this->lastUserName = newUserName;
}

void Settings::setTranslation(const QString &localeName)
{
    qCDebug(jtSettings) << "Settings setTranslation: from " << translation << "; to " << localeName;
    QString name = localeName;
    if (name.contains(".")){
        name = name.left(name.indexOf("."));
    }
    translation = name;
    qDebug() << "Setting translation to" << translation;
}

void Settings::addVstPlugin(const QString &pluginPath)
{
    qCDebug(jtSettings) << "Settings addVstPlugin: " << pluginPath;
    if (!vstSettings.cachedPlugins.contains(pluginPath))
        vstSettings.cachedPlugins.append(pluginPath);
}

void Settings::addVstToBlackList(const QString &pluginPath)
{
    qCDebug(jtSettings) << "Settings addVstToBlackList: " << pluginPath;
    if (!vstSettings.blackedPlugins.contains(pluginPath))
        vstSettings.blackedPlugins.append(pluginPath);
}

void Settings::removeVstFromBlackList(const QString &pluginPath)
{
    qCDebug(jtSettings) << "Settings removeVstFromBlackList: " << pluginPath;
    vstSettings.blackedPlugins.removeOne(pluginPath);
}

QStringList Settings::getVstPluginsPaths() const
{
    qCDebug(jtSettings) << "Settings getVstPluginsPaths";
    return vstSettings.cachedPlugins;
}

void Settings::clearVstCache()
{
    qCDebug(jtSettings) << "Settings clearVstCache";
    vstSettings.cachedPlugins.clear();
}

// CLEAR VST BLACKBOX
void Settings::clearBlackBox()
{
    qCDebug(jtSettings) << "Settings clearBlackBox";
    vstSettings.blackedPlugins.clear();
}

// VST paths to scan
void Settings::addVstScanPath(const QString &path)
{
    qCDebug(jtSettings) << "Settings addVstScanPath: " << path;
    vstSettings.foldersToScan.append(path);
}

void Settings::removeVstScanPath(const QString &path)
{
    qCDebug(jtSettings) << "Settings removeVstScanPath: " << path;
    vstSettings.foldersToScan.removeOne(path);
}

QStringList Settings::getVstScanFolders() const
{
    qCDebug(jtSettings) << "Settings getVstScanFolders";
    return vstSettings.foldersToScan;
}

QStringList Settings::getBlackListedPlugins() const
{
    qCDebug(jtSettings) << "Settings getVstScanFolders";
    return vstSettings.blackedPlugins;
}

// ++++++++++++++++++

#ifdef Q_OS_MAC

void Settings::addAudioUnitPlugin(const QString &pluginPath)
{
    qCDebug(jtSettings) << "Settings addAudioUnitPlugin: " << pluginPath;
    if (!audioUnitSettings.cachedPlugins.contains(pluginPath))
        audioUnitSettings.cachedPlugins.append(pluginPath);
}

void Settings::clearAudioUnitCache()
{
    qCDebug(jtSettings) << "Settings clearAudioUnitCache";
    audioUnitSettings.cachedPlugins.clear();
}

QStringList Settings::getAudioUnitsPaths() const
{
    qCDebug(jtSettings) << "Settings getAudioUnitsPaths";
    return audioUnitSettings.cachedPlugins;
}

#endif

// +++++++++++++++++++++++++++++
void Settings::setMetronomeSettings(float gain, float pan, bool muted)
{
    qCDebug(jtSettings) << "Settings setMetronomeSettings: gain from " << metronomeSettings.gain << " to " << gain
                        << "; pan from " << metronomeSettings.pan << " to " << pan
                        << "; muted from " << metronomeSettings.muted << "" << muted;
    metronomeSettings.pan = pan;
    metronomeSettings.gain = gain;
    metronomeSettings.muted = muted;
}

void Settings::setBuiltInMetronome(const QString &metronomeAlias)
{
    qCDebug(jtSettings) << "Settings setBuiltInMetronome: from " << metronomeSettings.builtInMetronomeAlias << " to " << metronomeAlias << " (and not custom sounds)";
    metronomeSettings.builtInMetronomeAlias = metronomeAlias;
    metronomeSettings.usingCustomSounds = false;
}

void Settings::setCustomMetronome(const QString &primaryBeatAudioFile, const QString &offBeatAudioFile, const QString &accentBeatAudioFile)
{
    qCDebug(jtSettings) << "Settings setCustomMetronome: primaryBeatAudioFile from " << metronomeSettings.customPrimaryBeatAudioFile << " to " << primaryBeatAudioFile
                        << "; offBeatAudioFile from " << metronomeSettings.customOffBeatAudioFile << "to " << offBeatAudioFile
                        << "; accentBeatAudioFile from " << metronomeSettings.customAccentBeatAudioFile << " to " << accentBeatAudioFile;
    if (QFileInfo(primaryBeatAudioFile).exists() && QFileInfo(offBeatAudioFile).exists() && QFileInfo(accentBeatAudioFile).exists()) {
        metronomeSettings.customPrimaryBeatAudioFile = primaryBeatAudioFile;
        metronomeSettings.customOffBeatAudioFile = offBeatAudioFile;
        metronomeSettings.customAccentBeatAudioFile = accentBeatAudioFile;
        metronomeSettings.usingCustomSounds = true;
    }
    else {
        metronomeSettings.customPrimaryBeatAudioFile = "";
        metronomeSettings.customOffBeatAudioFile = "";
        metronomeSettings.customAccentBeatAudioFile = "";
        metronomeSettings.usingCustomSounds = false;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Settings::setFullScreenView(bool v)
{
    qCDebug(jtSettings) << "Settings setFullScreenView: from " << windowSettings.fullScreenMode << " to " << v;
    windowSettings.fullScreenMode = v;
}

// +++++++++   Window Location  +++++++++++++++++++++++
void Settings::setWindowSettings(bool windowIsMaximized, const QPointF &location, const QSize &size)
{
    qCDebug(jtSettings) << "Settings setWindowSettings: windowIsMaximized from " << windowSettings.maximized << " to " << windowIsMaximized
                        << "; location from " << windowSettings.location << " to " << location
                        << "; size from " << windowSettings.size << " to " << size;
    QPointF newLocation(location);
    double x = (newLocation.x() >= 0) ? newLocation.x() : 0;
    double y = (newLocation.x() >= 0) ? newLocation.y() : 0;
    if (x > 1)
        newLocation.setX(0);
    if (y > 1)
        newLocation.setY(0);

    windowSettings.location = newLocation;
    windowSettings.maximized = windowIsMaximized;
    windowSettings.size = size;
}

// ++++++++++++++++++++++++++++++++++++++++
void Settings::setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, QString audioInputDevice, QString audioOutputDevice)
{
    qCDebug(jtSettings) << "Settings setAudioSettings: firstIn from " << audioSettings.firstIn << " to " << firstIn
                        << "; lastIn from " << audioSettings.lastIn << " to " << lastIn
                        << "; firstOut from " << audioSettings.firstOut << " to " << firstOut
                        << "; lastOut from " << audioSettings.lastOut << " to " << lastOut
                        << "; audioInputDevice from " << audioSettings.audioInputDevice << " to " << audioInputDevice
                        << "; audioOutputDevice from " << audioSettings.audioOutputDevice << " to " << audioOutputDevice;
    audioSettings.firstIn = firstIn;
    audioSettings.firstOut = firstOut;
    audioSettings.lastIn = lastIn;
    audioSettings.lastOut = lastOut;
    audioSettings.audioInputDevice = audioInputDevice;
    audioSettings.audioOutputDevice = audioOutputDevice;
}

void Settings::setSampleRate(int newSampleRate)
{
    qCDebug(jtSettings) << "Settings setSampleRate: from " << audioSettings.sampleRate << " to " << newSampleRate;
    audioSettings.sampleRate = newSampleRate;
}

void Settings::setBufferSize(int bufferSize)
{
    qCDebug(jtSettings) << "Settings setBufferSize: from " << audioSettings.bufferSize << " to " << bufferSize;
    audioSettings.bufferSize = bufferSize;
}

bool Settings::readFile(const QList<SettingsObject *> &sections)
{
    qCDebug(jtSettings) << "Settings readFile";
    QDir configFileDir = Configurator::getInstance()->getBaseDir();
    QString absolutePath = configFileDir.absoluteFilePath(fileName);
    QFile configFile(absolutePath);

    if (configFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
        QJsonObject root = doc.object();

        if (root.contains("masterGain")) // read last master gain
            this->masterFaderGain = root["masterGain"].toDouble();
        else
            this->masterFaderGain = 1; // unit gain as default

        if (root.contains("userName")) // read user name
            this->lastUserName = root["userName"].toString();

        if (root.contains("translation")) // read Translation
            this->translation = root["translation"].toString();
        if (this->translation.isEmpty())
            this->translation = QLocale().bcp47Name().left(2);

        if (root.contains("theme"))
            this->theme = root["theme"].toString();
        if (this->theme.isEmpty())
            this->theme = "Navy_nm"; //using Navy as the new default theme

        if (root.contains("intervalProgressShape")) // read intervall progress shape
            this->ninjamIntervalProgressShape = root["intervalProgressShape"].toInt(0); // zero as default value
        else
            this->ninjamIntervalProgressShape = 0;

        tracksLayoutOrientation = root["tracksLayoutOrientation"].toInt(0); // vertical as fallback value;

        if (root.contains("usingNarrowTracks"))
            this->usingNarrowedTracks = root["usingNarrowTracks"].toBool(false);
        else
            this->usingNarrowedTracks = false;

        if (root.contains("publicChatActivated")) {
            publicChatActivated = root["publicChatActivated"].toBool(true);
        }

        // read settings sections (Audio settings, Midi settings, ninjam settings, etc...)
        for (SettingsObject *so : sections)
            so->read(root[so->getName()].toObject());

        if(root.contains("intervalsBeforeInactivityWarning")) {
            intervalsBeforeInactivityWarning = root["intervalsBeforeInactivityWarning"].toInt();
            if (intervalsBeforeInactivityWarning < 1)
                intervalsBeforeInactivityWarning = 1;
        }

        if (root.contains("recentEmojis")) {
            QJsonArray array = root["recentEmojis"].toArray();
            for (int i = 0; i < array.count(); ++i) {
                recentEmojis << array.at(i).toString();
            }

        }

        if (root.contains("chatFontSizeOffset")) {
            chatFontSizeOffset = root["chatFontSizeOffset"].toInt();
        }

        return true;
    }
    else {
        qWarning(jtConfigurator) << "Settings : Can't load Jamtaba 2 config file:"
                                 << configFile.errorString();
    }

    return false;
}

void Settings::setLooperPreferredLayersCount(quint8 layersCount)
{
    qCDebug(jtSettings) << "Settings setLooperPreferredLayersCount: from " << looperSettings.preferredLayersCount << " to " << layersCount;
    looperSettings.preferredLayersCount = layersCount <= 8 ? layersCount : 8;
}

void Settings::setLooperAudioEncodingFlag(bool encodeAudioWhenSaving)
{
    qCDebug(jtSettings) << "Settings setLooperAudioEncodingFlag: from " << looperSettings.encodingAudioWhenSaving << " to " << encodeAudioWhenSaving;
    looperSettings.encodingAudioWhenSaving = encodeAudioWhenSaving;
}

void Settings::setLooperPreferredMode(quint8 looperMode)
{
    qCDebug(jtSettings) << "Settings setLooperPreferredMode: from " << looperSettings.preferredMode << " to " << looperMode;
    looperSettings.preferredMode = looperMode;
}

bool Settings::writeFile(const QList<SettingsObject *> &sections) // io ops ...
{
    qCDebug(jtSettings) << "Settings writeFile...";
    QDir configFileDir = Configurator::getInstance()->getBaseDir();
    QFile file(configFileDir.absoluteFilePath(fileName));
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject root;

        // writing global settings
        root["userName"] = lastUserName; // write user name
        root["translation"] = translation; // write translate locale
        root["theme"] = theme;
        root["intervalProgressShape"] = ninjamIntervalProgressShape;
        root["tracksLayoutOrientation"] = tracksLayoutOrientation;
        root["usingNarrowTracks"] = usingNarrowedTracks;
        root["masterGain"] = masterFaderGain;
        root["intervalsBeforeInactivityWarning"] = static_cast<int>(intervalsBeforeInactivityWarning);
        root["chatFontSizeOffset"] = static_cast<int>(chatFontSizeOffset);
        root["publicChatActivated"] = publicChatIsActivated();

        if (!recentEmojis.isEmpty()) {
            root["recentEmojis"] = QJsonArray::fromStringList(recentEmojis);
        }

        // write settings sections
        for (SettingsObject *so : sections) {
            QJsonObject sectionObject;
            so->write(sectionObject);
            root[so->getName()] = sectionObject;
        }
        QJsonDocument doc(root);
        file.write(doc.toJson());
        qCDebug(jtCore) << "Settings writeFile: " << doc;
        return true;
    } else {
        qCritical() << file.errorString();
    }
    return false;
}

// PRESETS
bool Settings::writePresetToFile(const Preset &preset)
{
    qCDebug(jtSettings) << "Settings writePresetToFile...";
    QString absolutePath = Configurator::getInstance()->getPresetPath(preset.name);
    QFile file(absolutePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject inputTracksJsonObject;
        preset.inputTrackSettings.write(inputTracksJsonObject); // write the channels and subchannels in the json object

        QJsonObject root;
        root[preset.name] = inputTracksJsonObject;

        QJsonDocument doc(root);
        file.write(doc.toJson());
        qCDebug(jtSettings) << "Settings writePresetToFile: " << doc;
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
    qCDebug(jtSettings) << "Preset ctor";
}

// ++++++++++++++
Preset Settings::readPresetFromFile(const QString &presetFileName, bool allowMultiSubchannels)
{
    qCDebug(jtSettings) << "Preset readPresetFromFile";
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
    return Preset(); // returning an empty/invalid preset
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Settings::load()
{
    qCDebug(jtSettings) << "Settings load";
    QList<persistence::SettingsObject *> sections;
    sections.append(&audioSettings);
    sections.append(&midiSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
#ifdef Q_OS_MAC
    sections.append(&audioUnitSettings);
#endif
    sections.append(&inputsSettings);
    sections.append(&recordingSettings);
    sections.append(&privateServerSettings);
    sections.append(&meteringSettings);
    sections.append(&looperSettings);
    sections.append(&rememberSettings);
    sections.append(&collapseSettings);

    readFile(sections);
}

Settings::Settings() :
    tracksLayoutOrientation(Qt::Vertical),
    masterFaderGain(1.0),
    translation("en"), // english as default language
    theme("Navy_nm"), // flat as default theme,
    ninjamIntervalProgressShape(0),
    usingNarrowedTracks(false),
    intervalsBeforeInactivityWarning(5), // 5 intervals by default,
    chatFontSizeOffset(0),
    publicChatActivated(true)
{
    qCDebug(jtSettings) << "Settings ctor";
    // qDebug() << "Settings in " << fileDir;
}

void Settings::save(const LocalInputTrackSettings &localInputsSettings)
{
    qCDebug(jtSettings) << "Settings save";
    this->inputsSettings = localInputsSettings;
    QList<persistence::SettingsObject *> sections;
    sections.append(&audioSettings);
    sections.append(&midiSettings);
    sections.append(&windowSettings);
    sections.append(&metronomeSettings);
    sections.append(&vstSettings);
#ifdef Q_OS_MAC
    sections.append(&audioUnitSettings);
#endif
    sections.append(&inputsSettings);
    sections.append(&recordingSettings);
    sections.append(&privateServerSettings);
    sections.append(&meteringSettings);
    sections.append(&looperSettings);
    sections.append(&rememberSettings);
    sections.append(&collapseSettings);

    writeFile(sections);
}

void Settings::deletePreset(const QString &name)
{
    qCDebug(jtSettings) << "Settings deletePreset " << name;
    Configurator::getInstance()->deletePreset(name);
}

Settings::~Settings()
{
    // qCDebug(jtSettings) << "Settings destructor";
}

void Settings::setTheme(const QString theme)
{
    qCDebug(jtSettings) << "Settings setTheme: from " << this->theme << " to " << theme;
    this->theme = theme;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QString Settings::getTranslation() const
{
    // qCDebug(jtSettings) << "Settings getTranslation";
    return translation;
}

void Settings::setRemoteUserRememberingSettings(bool boost, bool level, bool pan, bool mute, bool lowCut)
{
    qCDebug(jtSettings) << "Settings setRemoteUserRememberingSettings: boost from " << rememberSettings.rememberBoost << " to " << boost
                        << "; level from " << rememberSettings.rememberLevel << " to " << level
                        << "; pan from " << rememberSettings.rememberPan << " to " << pan
                        << "; mute from " << rememberSettings.rememberMute << " to " << mute
                        << "; lowCut from " << rememberSettings.rememberLowCut << " to " << lowCut;
    rememberSettings.rememberBoost   = boost;
    rememberSettings.rememberLevel   = level;
    rememberSettings.rememberPan     = pan;
    rememberSettings.rememberLowCut  = lowCut;
    rememberSettings.rememberMute    = mute;
}

void Settings::setCollapsileSectionsRememberingSettings(bool localChannels, bool bottomSection, bool chatSection)
{
    qCDebug(jtSettings) << "Settings setCollapsileSectionsRememberingSettings: localChannels from " << rememberSettings.rememberLocalChannels << " to " << localChannels
                        << "; bottomSection from " << rememberSettings.rememberBottomSection << " to " << bottomSection
                        << "; chatSection from " << rememberSettings.rememberChatSection << " to " << chatSection;
    rememberSettings.rememberLocalChannels = localChannels;
    rememberSettings.rememberBottomSection = bottomSection;
    rememberSettings.rememberChatSection = chatSection;
}

//__________________________________________________________

MeteringSettings::MeteringSettings() :
    SettingsObject(QStringLiteral("Metering")),
    showingMaxPeakMarkers(true),
    meterOption(0), // showing RMS + Peaks
    waveDrawingMode(3), // pixeled buildings
    refreshRate(30)
{
    qCDebug(jtSettings) << "MeteringSettings ctor";
}

void MeteringSettings::read(const QJsonObject &in)
{
    this->showingMaxPeakMarkers = getValueFromJson(in, "showMaxPeak", true);
    this->meterOption = getValueFromJson(in, "meterOption", quint8(0));
    this->refreshRate = getValueFromJson(in, "refreshRate", quint8(30));
    this->waveDrawingMode = getValueFromJson(in, "waveDrawingMode", quint8(3)); // using 3 (pixeleted buildings) as default value

    qCDebug(jtSettings) << "MeteringSettings: showingMaxPeakMarkers " << showingMaxPeakMarkers
                        << "; meterOption " << meterOption
                        << "; refreshRate " << refreshRate
                        << "; waveDrawingMode " << waveDrawingMode;
}

void MeteringSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "MeteringSettings write";
    out["showMaxPeak"]      = showingMaxPeakMarkers;
    out["meterOption"]      = meterOption;
    out["refreshRate"]      = refreshRate;
    out["waveDrawingMode"]  = waveDrawingMode;
}

//________________________________________________________________

RememberSettings::RememberSettings() :
    SettingsObject(QStringLiteral("Remember")),
    rememberBoost(true),
    rememberLevel(true),
    rememberPan(true),
    rememberMute(true),
    rememberLowCut(true),
    rememberLocalChannels(true),
    rememberBottomSection(true),
    rememberChatSection(true)
{
    qCDebug(jtSettings) << "RememberSettings ctor";
}

void RememberSettings::write(QJsonObject &out) const
{
    qCDebug(jtSettings) << "RememberSettings write";
    out["boost"] = rememberBoost;
    out["level"] = rememberLevel;
    out["pan"] = rememberPan;
    out["mute"] = rememberMute;
    out["lowCut"] = rememberLowCut;

    out["localChannels"] = rememberLocalChannels;
    out["bottomSection"] = rememberBottomSection;
    out["chatSection"] = rememberChatSection;
}

void RememberSettings::read(const QJsonObject &in)
{
    rememberBoost = getValueFromJson(in, "boost", true);
    rememberLevel = getValueFromJson(in, "level", true);
    rememberPan = getValueFromJson(in, "pan", true);
    rememberMute = getValueFromJson(in, "mute", true);
    rememberLowCut = getValueFromJson(in, "lowCut", true);

    rememberLocalChannels = getValueFromJson(in, "localChannels", true);
    rememberBottomSection = getValueFromJson(in, "bottomSection", false);
    rememberChatSection = getValueFromJson(in, "chatSection", false);

    qCDebug(jtSettings) << "RememberSettings: rememberBoost " << rememberBoost
                        << "; rememberLevel " << rememberLevel
                        << "; rememberPan " << rememberPan
                        << "; rememberMute " << rememberMute
                        << "; rememberLocalChannels " << rememberLocalChannels
                        << "; rememberBottomSection " << rememberBottomSection
                        << "; rememberChatSection " << rememberChatSection;
}
