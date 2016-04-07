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

namespace Persistence {
class Settings;

class SettingsObject // base class for the settings components
{
protected:
    QString name;
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
    static int getValueFromJson(const QJsonObject &json, const QString &propertyName, int fallBackValue);
    static float getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                  float fallBackValue);
    static QString getValueFromJson(const QJsonObject &json, const QString &propertyName,
                                    QString fallBackValue);
    static bool getValueFromJson(const QJsonObject &json, const QString &propertyName, bool fallBackValue);
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
    QString server;
    int serverPort;
    QString password;
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
    bool maximized;
    bool fullViewMode;
    bool fullScreenViewMode;
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
// ++++++++++++++++++++++++
class RecordingSettings : public SettingsObject
{
public:
    RecordingSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;
    bool saveMultiTracksActivated;
    QString recordingPath;
};
// +++++++++++++++++++++++++++++++++
class Plugin
{
public:
    Plugin(const QString &path, bool bypassed, const QByteArray &data);
    QString path;
    bool bypassed;
    QByteArray data;// saved data to restore in next jam session
};
// +++++++++++++++++++++++++++++++++
class Subchannel
{
public:
    Subchannel(int firstInput, int channelsCount, int midiDevice, int midiChannel, float gain,
               int boost, float pan, bool muted, qint8 transpose, quint8 lowerMidiNote, quint8 higherMidiNote);
    int firstInput;
    int channelsCount;
    int midiDevice;
    int midiChannel;
    float gain;
    int boost;// [-1, 0, +1]
    float pan;
    bool muted;
    qint8 transpose; //midi transpose
    quint8 lowerMidiNote; //midi rey range
    quint8 higherMidiNote;

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
    void read(const QJsonObject &in, bool allowMultiSubchannels);
    QList<Channel> channels;

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

// ++++++++++++++++++++++++
class Settings
{
private:
    QString fileDir;
    static QString fileName;
    AudioSettings audioSettings;
    MidiSettings midiSettings;
    WindowSettings windowSettings;
    MetronomeSettings metronomeSettings;
    VstSettings vstSettings;
    LocalInputTrackSettings inputsSettings;
    RecordingSettings recordingSettings;
    PrivateServerSettings privateServerSettings;
    QString lastUserName;// the last nick name choosed by user
    QString translation;// the translation language (en, fr, jp, pt, etc.) being used in chat
    int ninjamIntervalProgressShape;// Circle, Ellipe or Line
    float masterFaderGain;// last master fader gain
    Qt::Orientation tracksLayoutOrientation; //horizontal or vertical
    bool usingNarrowedTracks; //narrow or wide tracks?

    bool readFile(APPTYPE type, const QList<SettingsObject *> &sections);// io ops ...
    bool writeFile(APPTYPE type, const QList<SettingsObject *> &sections);// io ops ...

public:
    Settings();
    ~Settings();

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

    // private server
    inline QString getLastPrivateServer() const
    {
        return privateServerSettings.server;
    }

    inline int getLastPrivateServerPort() const
    {
        return privateServerSettings.serverPort;
    }

    inline QString getLastPrivateServerPassword() const
    {
        return privateServerSettings.password;
    }

    void setPrivateServerData(const QString &server, int serverPort, const QString &password);

    // recording settings
    inline bool isSaveMultiTrackActivated() const
    {
        return recordingSettings.saveMultiTracksActivated;
    }

    inline RecordingSettings getRecordingSettings() const
    {
        return recordingSettings;
    }

    inline QString getRecordingPath() const
    {
        return recordingSettings.recordingPath;
    }

    inline void setSaveMultiTrack(bool saveMultiTracks)
    {
        recordingSettings.saveMultiTracksActivated = saveMultiTracks;
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

    void setUserName(const QString &newUserName);
    void storeLasUserName(const QString &userName);
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

    void setWindowSettings(bool windowIsMaximized, bool usingFullView, QPointF location);
    inline bool windowWasMaximized() const
    {
        return windowSettings.maximized;
    }

    inline bool windowsWasFullViewMode() const
    {
        return windowSettings.fullViewMode;
    }

    inline bool windowsWasFullScreenViewMode() const
    {
        return windowSettings.fullScreenViewMode;
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
    QString getLastUserName() const;

    // TRANSLATION
    QString getTranslation() const;
    void setTranslation(const QString &localeName);

    QString getLastChannelName(int channelIndex) const;
};
}

#endif
