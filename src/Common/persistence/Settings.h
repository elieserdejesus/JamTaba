#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QPointF>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QFile>
#include "Configurator.h"
#include "audio/core/PluginDescriptor.h"

namespace Persistence {
class Settings;

class SettingsObject // base class for the settings components
{

public:
    SettingsObject(const QString &name);
    virtual ~SettingsObject();
    virtual void write(QJsonObject &out) const = 0;
    virtual void read(const QJsonObject &in) = 0;
    inline QString getName() const
    {
        return name;
    }

protected:
    QString name;

    static int getValueFromJson(const QJsonObject &json, const QString &propertyName, int fallBackValue);
    static float getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                  float fallBackValue);
    static QString getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                    QString fallBackValue);
    static bool getValueFromJson(const QJsonObject &json, const QString &propertyName, bool fallBackValue);
    static QJsonArray getValueFromJson(const QJsonObject &json, const QString &propertyName, QJsonArray fallBackValue);
    static QJsonObject getValueFromJson(const QJsonObject &json, const QString &propertyName, QJsonObject fallBackValue);
};
// +++++++++++++++++++++++++++++++++++++++++++
class AudioSettings : public SettingsObject
{
public:
    AudioSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    int sampleRate;
    int bufferSize;
    int firstIn;
    int firstOut;
    int lastIn;
    int lastOut;
    int audioDevice;
    float encodingQuality;
};
// +++++++++++++++++++++++++++++++++++++
class MidiSettings : public SettingsObject
{
public:
    MidiSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    QList<bool> inputDevicesStatus;
};

// +++++++++++++++++++++++++++++++++++
class PrivateServerSettings : public SettingsObject
{

public:
    PrivateServerSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    void addPrivateServerData(const QString &serverName, int serverPort, const QString &password = QString());
    inline QList<QString> getLastServers() const { return lastServers; }
    inline quint32 getLastPort() const { return lastPort; }
    inline QString getLastPassword() const { return lastPassword; }
private:
    QList<QString> lastServers;
    int lastPort;
    QString lastPassword;
};

// +++++++++++++++++++++++++++++++++++
class MetronomeSettings : public SettingsObject
{
public:
    MetronomeSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    float pan;
    float gain;
    bool muted;
    bool usingCustomSounds;
    QString customPrimaryBeatAudioFile;
    QString customSecondaryBeatAudioFile;
    QString builtInMetronomeAlias;
};

// +++++++++++++++++++++++++++++++++++++++++++++++
class WindowSettings : public SettingsObject
{
public:
    WindowSettings();
    QPointF location;
    QSize size;
    bool maximized;
    bool fullScreenMode;
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
};

// +++++++++++++++++++++++++++++++++++++++++++
class VstSettings : public SettingsObject
{
public:
    VstSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    QStringList cachedPlugins;
    QStringList foldersToScan;
    QStringList blackedPlugins;// vst in blackbox....
};

class AudioUnitSettings  : public SettingsObject
{
public:
    AudioUnitSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    QStringList cachedPlugins;
};

// ++++++++++++++++++++++++
class RecordingSettings : public SettingsObject
{
public:
    RecordingSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    bool saveMultiTracksActivated;
    QMap <QString, bool> jamRecorderActivated;
    QString recordingPath;

    inline bool isJamRecorderActivated(QString key) const
    {
        if (jamRecorderActivated.contains(key))
            return jamRecorderActivated[key];
        return false;
    }
    inline void setJamRecorderActivated(QString key, bool value)
    {
        jamRecorderActivated[key] = value;
    }
};
// +++++++++++++++++++++++++++++++++
class Plugin
{
public:

    Plugin(const Audio::PluginDescriptor &descriptor, bool bypassed, const QByteArray &data = QByteArray());
    QString path;
    QString name;
    QString manufacturer;
    bool bypassed;
    QByteArray data;// saved data to restore in next jam session
    Audio::PluginDescriptor::Category category; // VST, AU, NATIVE plugin
};
// +++++++++++++++++++++++++++++++++
class Subchannel
{
public:
    Subchannel(int firstInput, int channelsCount, int midiDevice, int midiChannel, float gain,
               int boost, float pan, bool muted, bool stereoInverted, qint8 transpose, quint8 lowerMidiNote, quint8 higherMidiNote, bool routingMidiToFirstSubchannel);
    int firstInput;
    int channelsCount;
    int midiDevice;
    int midiChannel;
    float gain;
    int boost;// [-1, 0, +1]
    float pan;
    bool muted;
    bool stereoInverted;
    qint8 transpose; //midi transpose
    quint8 lowerMidiNote; //midi rey range
    quint8 higherMidiNote;
    bool routingMidiToFirstSubchannel;

    inline QList<Persistence::Plugin> getPlugins() const
    {
        return plugins;
    }

    inline void setPlugins(const QList<Plugin> &newPlugins)
    {
        plugins = newPlugins;
    }

    inline bool isMidi() const
    {
        return midiDevice >= 0;
    }

    inline bool isNoInput() const
    {
        return channelsCount <= 0;
    }

    inline bool isMono() const
    {
        return channelsCount == 1;
    }

    inline bool isStereo() const
    {
        return channelsCount == 2;
    }

    inline bool isStereoInverted() const
    {
        return stereoInverted;
    }

private:
    QList<Persistence::Plugin> plugins;
};
// +++++++++++++++++++++++++++++++++
class Channel
{
public:
    Channel(const QString &name);
    QString name;
    QList<Subchannel> subChannels;
};
// +++++++++++++++++++++++++++++++++
class LocalInputTrackSettings : public SettingsObject
{
public:
    LocalInputTrackSettings(bool createOneTrack = false);
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    void read(const QJsonObject &in, bool allowSubchannels);
    QList<Channel> channels;

    static Plugin jsonObjectToPlugin(QJsonObject jsonObject);

    inline bool isValid() const
    {
        return !channels.isEmpty();
    }
};
// +++++++++PRESETS+++++++++++++++
class Preset
{
public:
    Preset(const QString &name = "default",
           const LocalInputTrackSettings &inputSettings = LocalInputTrackSettings());
    void write(QJsonObject &out) const;
    void read(const QJsonObject &in);

    bool isValid() const
    {
        return inputTrackSettings.isValid();
    }

    LocalInputTrackSettings inputTrackSettings;
    QString name;
};

class MeteringSettings : public SettingsObject
{
public:
    MeteringSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;

    bool showingMaxPeakMarkers;
    quint8 meterOption; // 0 - peak + RMS, 1 - peak only or 2 - RMS only
    quint8 refreshRate; // in Hertz
    quint8 waveDrawingMode;
};

// ++++++++++++++++++++++++
class Settings
{
private:
    static QString fileName;
    AudioSettings audioSettings;
    MidiSettings midiSettings;
    WindowSettings windowSettings;
    MetronomeSettings metronomeSettings;
    VstSettings vstSettings;
#ifdef Q_OS_MAC
    AudioUnitSettings audioUnitSettings;
#endif
    LocalInputTrackSettings inputsSettings;
    RecordingSettings recordingSettings;
    PrivateServerSettings privateServerSettings;
    MeteringSettings meteringSettings;

    QString lastUserName;// the last nick name choosed by user
    QString translation;// the translation language (en, fr, jp, pt, etc.) being used in chat
    QString theme; //the style sheet used
    int ninjamIntervalProgressShape;// Circle, Ellipe or Line
    float masterFaderGain;// last master fader gain
    Qt::Orientation tracksLayoutOrientation; //horizontal or vertical
    bool usingNarrowedTracks; //narrow or wide tracks?

    bool readFile(const QList<SettingsObject *> &sections);
    bool writeFile(const QList<SettingsObject *> &sections);

public:
    Settings();
    ~Settings();

    inline void storeWaveDrawingMode(quint8 mode)
    {
        meteringSettings.waveDrawingMode = mode;
    }

    inline quint8 getLastWaveDrawingMode() const
    {
        return meteringSettings.waveDrawingMode;
    }

    inline float getEncodingQuality() const
    {
        return audioSettings.encodingQuality;
    }

    inline void setEncodingQuality(float quality)
    {
        audioSettings.encodingQuality = quality;
    }

    void setBuiltInMetronome(const QString &metronomeAlias);
    inline QString getBuiltInMetronome() const { return metronomeSettings.builtInMetronomeAlias; }

    void setCustomMetronome(const QString &primaryBeatAudioFile, const QString &secondaryBeatAudioFile);

    inline bool isUsingCustomMetronomeSounds() const
    {
        return metronomeSettings.usingCustomSounds;
    }

    inline QString getMetronomeFirstBeatFile() const
    {
        return metronomeSettings.customPrimaryBeatAudioFile;
    }

    void setTheme(const QString theme);

    inline QString getTheme() const { return theme; }

    inline QString getMetronomeSecondaryBeatFile() const
    {
        return metronomeSettings.customSecondaryBeatAudioFile;
    }

    inline void storeTracksSize(bool narrowedTracks)
    {
        usingNarrowedTracks = narrowedTracks;
    }

    inline bool isUsingNarrowedTracks() const
    {
        return usingNarrowedTracks;
    }

    inline LocalInputTrackSettings getInputsSettings() const
    {
        return inputsSettings;
    }

    void save(const LocalInputTrackSettings &inputsSettings);
    void load();

    inline float getLastMasterGain() const
    {
        return masterFaderGain;
    }

    inline void storeMasterGain(float newMasterFaderGain)
    {
        masterFaderGain = newMasterFaderGain;
    }

    inline Qt::Orientation getLastTracksLayoutOrientation() const
    {
        return tracksLayoutOrientation;
    }

    inline void storeTracksLayoutOrientation(Qt::Orientation newOrientation)
    {
        tracksLayoutOrientation = newOrientation;
    }

    bool writePresetToFile(const Preset &preset);
    void deletePreset(const QString &name);
    QStringList getPresetList();
    Preset readPresetFromFile(const QString &presetFileName, bool allowMultiSubchannels = true);

    inline int getLastSampleRate() const
    {
        return audioSettings.sampleRate;
    }

    inline int getLastBufferSize() const
    {
        return audioSettings.bufferSize;
    }

    inline QList<QString> getLastPrivateServers() const
    {
        return privateServerSettings.getLastServers();
    }

    inline quint32 getLastPrivateServerPort() const
    {
        return privateServerSettings.getLastPort();
    }

    inline QString getLastPrivateServerPassword() const
    {
        return privateServerSettings.getLastPassword();
    }

    void addPrivateServer(const QString &server, int serverPort, const QString &password);

    // recording settings
    inline RecordingSettings getRecordingSettings() const
    {
        return recordingSettings;
    }

    inline bool isSaveMultiTrackActivated() const
    {
        return recordingSettings.saveMultiTracksActivated;
    }

    inline void setSaveMultiTrack(bool saveMultiTracks)
    {
        recordingSettings.saveMultiTracksActivated = saveMultiTracks;
    }

    inline bool isJamRecorderActivated(QString key) const
    {
        return recordingSettings.isJamRecorderActivated(key);
    }

    inline void setJamRecorderActivated(QString key, bool value)
    {
        recordingSettings.setJamRecorderActivated(key, value);
    }

    inline QString getRecordingPath() const
    {
        return recordingSettings.recordingPath;
    }

    inline void setRecordingPath(const QString &newPath)
    {
        recordingSettings.recordingPath = newPath;
    }

    // user name
    inline QString getUserName() const
    {
        return lastUserName;
    }

    void storeUserName(const QString &newUserName);
    void storeLastChannelName(int channelIndex, const QString &channelName);

    void setIntervalProgressShape(int shape)
    {
        this->ninjamIntervalProgressShape = shape;
    }

    inline int getIntervalProgressShape() const
    {
        return this->ninjamIntervalProgressShape;
    }

    // VST
    void addVstPlugin(const QString &pluginPath);
    void addVstToBlackList(const QString &pluginPath);
    void removeVstFromBlackList(const QString &pluginPath);
    QStringList getVstPluginsPaths() const;
    QStringList getBlackListedPlugins() const;
    void clearVstCache();
    void clearBlackBox();

    // VST paths
    void addVstScanPath(const QString &path);
    void removeVstScanPath(const QString &path);
    QStringList getVstScanFolders() const;

    // AU plugins
#ifdef Q_OS_MAC
    void addAudioUnitPlugin(const QString &pluginPath);
    void clearAudioUnitCache();
    QStringList getAudioUnitsPaths() const;
#endif

    // ++++++++++++++ Metronome ++++++++++
    void setMetronomeSettings(float gain, float pan, bool muted);

    inline float getMetronomeGain() const
    {
        return metronomeSettings.gain;
    }

    inline float getMetronomePan() const
    {
        return metronomeSettings.pan;
    }

    inline bool getMetronomeMuteStatus() const
    {
        return metronomeSettings.muted;
    }

    // +++++++++   Window  +++++++++++++++++++++++
    inline QPointF getLastWindowLocation() const
    {
        return windowSettings.location;
    }

    inline QSize getLastWindowSize() const
    {
        return windowSettings.size;
    }

    void setWindowSettings(bool windowIsMaximized, const QPointF &location, const QSize &size);

    inline bool windowWasMaximized() const
    {
        return windowSettings.maximized;
    }

    inline bool windowsWasFullScreenViewMode() const
    {
        return windowSettings.fullScreenMode;
    }

    void setFullScreenView(bool v);
    // ++++++++++++++++++++++++++++++++++++++++
    void setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice);

    void setSampleRate(int newSampleRate);
    void setBufferSize(int bufferSize);

    inline int getFirstGlobalAudioInput() const
    {
        return audioSettings.firstIn;
    }

    inline int getLastGlobalAudioInput() const
    {
        return audioSettings.lastIn;
    }

    inline int getFirstGlobalAudioOutput() const
    {
        return audioSettings.firstOut;
    }

    inline int getLastGlobalAudioOutput() const
    {
        return audioSettings.lastOut;
    }

    inline int getLastAudioDevice() const
    {
        return audioSettings.audioDevice;
    }

    inline void setMidiSettings(const QList<bool> &inputDevicesStatus)
    {
        midiSettings.inputDevicesStatus = inputDevicesStatus;
    }

    inline QList<bool> getMidiInputDevicesStatus() const
    {
        return midiSettings.inputDevicesStatus;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // TRANSLATION
    QString getTranslation() const;
    void setTranslation(const QString &localeName);

    QString getLastChannelName(int channelIndex) const;

    //Metering
    inline quint8 getMeterOption() const { return meteringSettings.meterOption; }
    inline bool isShowingMaxPeaks() const { return meteringSettings.showingMaxPeakMarkers; }
    inline quint8 getMeterRefreshRate() const { return meteringSettings.refreshRate; }
    inline void storeMeterOption(quint8 meterOption) { meteringSettings.meterOption = meterOption; }
    inline void storeMeterShowingMaxPeaks(bool showingMaxPeaks) { meteringSettings.showingMaxPeakMarkers = showingMaxPeaks; }
    inline void storeMeterRefreshRate(quint8 newRate) { meteringSettings.refreshRate = newRate; }
};
}

#endif
