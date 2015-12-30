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
    SettingsObject(QString name);
    virtual void write(QJsonObject &out) = 0;
    virtual void read(QJsonObject in) = 0;
    inline QString getName() const
    {
        return name;
    }

protected:
    static int getValueFromJson(const QJsonObject &json, QString propertyName, int fallBackValue);
    static float getValueFromJson(const QJsonObject &json, QString propertyName,
                                  float fallBackValue);
    static QString getValueFromJson(const QJsonObject &json, QString propertyName,
                                    QString fallBackValue);
    static bool getValueFromJson(const QJsonObject &json, QString propertyName, bool fallBackValue);
};
// +++++++++++++++++++++++++++++++++++++++++++
class AudioSettings : public SettingsObject
{
public:
    AudioSettings();
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
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
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
    QList<bool> inputDevicesStatus;
};

// +++++++++++++++++++++++++++++++++++
class PrivateServerSettings : public SettingsObject
{
public:
    PrivateServerSettings();
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
    QString server;
    int serverPort;
    QString password;
};

// +++++++++++++++++++++++++++++++++++
class MetronomeSettings : public SettingsObject
{
public:
    MetronomeSettings();
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
    float pan;
    float gain;
    bool muted;
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
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
};

// +++++++++++++++++++++++++++++++++++++++++++
class VstSettings : public SettingsObject
{
public:
    VstSettings();
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
    QStringList cachedPlugins;
    QStringList foldersToScan;
    QStringList blackedPlugins;// vst in blackbox....
};
// ++++++++++++++++++++++++
class RecordingSettings : public SettingsObject
{
public:
    RecordingSettings();
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
    bool saveMultiTracksActivated;
    QString recordingPath;
};
// +++++++++++++++++++++++++++++++++
class Plugin
{
public:
    Plugin(QString path, bool bypassed, QByteArray data);
    QString path;
    bool bypassed;
    QByteArray data;// saved data to restore in next jam session
};
// +++++++++++++++++++++++++++++++++
class Subchannel
{
public:
    Subchannel(int firstInput, int channelsCount, int midiDevice, int midiChannel, float gain,
               int boost, float pan, bool muted);
    int firstInput;
    int channelsCount;
    int midiDevice;
    int midiChannel;
    float gain;
    int boost;// [-1, 0, +1]
    float pan;
    bool muted;

    inline QList<Persistence::Plugin> getPlugins() const
    {
        return plugins;
    }

    inline void setPlugins(QList<Plugin> newPlugins)
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
    Channel(QString name);
    QString name;
    QList<Subchannel> subChannels;
};
// +++++++++++++++++++++++++++++++++
class LocalInputTrackSettings : public SettingsObject
{
public:
    LocalInputTrackSettings(bool createOneTrack = false);
    void write(QJsonObject &out) override;
    void read(QJsonObject in) override;
    void read(QJsonObject in, bool allowMultiSubchannels);
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
    Preset(QString name = "default",
           LocalInputTrackSettings inputSettings = LocalInputTrackSettings());
    void write(QJsonObject &out);
    void read(QJsonObject in);

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
    // PresetsSettings presetSettings;
    RecordingSettings recordingSettings;
    PrivateServerSettings privateServerSettings;
    QString lastUserName;// the last nick name choosed by user
    QString translation;// the translation being used in chat
    int ninjamIntervalProgressShape;// Circle, Ellipe or Line
    bool readFile(APPTYPE type, QList<SettingsObject *> sections);// io ops ...
    bool writeFile(APPTYPE type, QList<SettingsObject *> sections);// io ops ...

public:
    Settings();
    ~Settings();

    inline LocalInputTrackSettings getInputsSettings() const
    {
        return inputsSettings;
    }

    void save(LocalInputTrackSettings inputsSettings);
    void load();

    bool writePresetToFile(Preset preset);
    void deletePreset(QString name);
    QStringList getPresetList();
    Preset readPresetFromFile(QString presetFileName, bool allowMultiSubchannels = true);

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

    void setPrivateServerData(QString server, int serverPort, QString password);

    // recording settings
    inline bool isSaveMultiTrackActivated() const
    {
        return recordingSettings.saveMultiTracksActivated;
    }

    inline QString getRecordingPath() const
    {
        return recordingSettings.recordingPath;
    }

    inline void setSaveMultiTrack(bool saveMultiTracks)
    {
        recordingSettings.saveMultiTracksActivated = saveMultiTracks;
    }

    inline void setRecordingPath(QString newPath)
    {
        recordingSettings.recordingPath = newPath;
    }

    // user name
    inline QString getUserName() const
    {
        return lastUserName;
    }

    void setUserName(QString newUserName);
    void storeLasUserName(QString userName);
    void storeLastChannelName(int channelIndex, QString channelName);

    void setIntervalProgressShape(int shape)
    {
        this->ninjamIntervalProgressShape = shape;
    }

    inline int getIntervalProgressShape() const
    {
        return this->ninjamIntervalProgressShape;
    }

    // VST
    void addVstPlugin(QString pluginPath);
    void addVstToBlackList(QString pluginPath);
    void RemVstFromBlackList(int index);
    QStringList getVstPluginsPaths() const;
    QStringList getBlackListedPlugins() const;
    void clearVstCache();
    void clearBlackBox();

    // VST paths
    void addVstScanPath(QString path);
    void removeVstScanPath(QString path);
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
    void setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice,
                          int sampleRate, int bufferSize);

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

    inline void setMidiSettings(QList<bool> inputDevicesStatus)
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
    void setTranslation(QString translate);

    QString getLastChannelName(int channelIndex) const;
};
}

#endif
