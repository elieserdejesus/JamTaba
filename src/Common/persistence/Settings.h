#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QPointF>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QFile>
#include <QSize>
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
    QString customOffBeatAudioFile;
    QString customAccentBeatAudioFile;
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
    QStringList blackedPlugins; // vst in blackbox....
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

class MultiTrackRecordingSettings : public SettingsObject
{
public:
    MultiTrackRecordingSettings();
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

class LooperSettings : public SettingsObject
{
public:
    LooperSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;

    quint8 preferredLayersCount; // how many layers in each looper?
    quint8 preferredMode; // store the last used looper mode
    QString loopsFolder; // where looper audio files will be saved
    bool encodingAudioWhenSaving;
    quint8 waveFilesBitDepth;
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
    QByteArray data; // saved data to restore in next jam session
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
    int boost; // [-1, 0, +1]
    float pan;
    bool muted;
    bool stereoInverted;
    qint8 transpose; // midi transpose
    quint8 lowerMidiNote; // midi rey range
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
    MultiTrackRecordingSettings recordingSettings;
    PrivateServerSettings privateServerSettings;
    MeteringSettings meteringSettings;
    LooperSettings looperSettings;

    QString lastUserName; // the last nick name choosed by user
    QString translation; // the translation language (en, fr, jp, pt, etc.) being used in chat
    QString theme; // the style sheet used
    int ninjamIntervalProgressShape; // Circle, Ellipe or Line
    float masterFaderGain; // last master fader gain
    Qt::Orientation tracksLayoutOrientation; // horizontal or vertical
    bool usingNarrowedTracks; // narrow or wide tracks?

    bool readFile(const QList<SettingsObject *> &sections);
    bool writeFile(const QList<SettingsObject *> &sections);

public:
    Settings();
    ~Settings();

    void storeWaveDrawingMode(quint8 mode);
    quint8 getLastWaveDrawingMode() const;

    float getEncodingQuality() const;
    void setEncodingQuality(float quality);

    void setBuiltInMetronome(const QString &metronomeAlias);
    QString getBuiltInMetronome() const;
    void setCustomMetronome(const QString &primaryBeatAudioFile, const QString &offBeatAudioFile, const QString &accentBeatAudioFile);
    bool isUsingCustomMetronomeSounds() const;
    QString getMetronomeFirstBeatFile() const;
    QString getMetronomeOffBeatFile() const;
    QString getMetronomeAccentBeatFile() const;

    void setTheme(const QString theme);
    QString getTheme() const;

    void storeTracksSize(bool narrowedTracks);

    bool isUsingNarrowedTracks() const;

    LocalInputTrackSettings getInputsSettings() const;
    void save(const LocalInputTrackSettings &inputsSettings);
    void load();

    float getLastMasterGain() const;
    void storeMasterGain(float newMasterFaderGain);

    Qt::Orientation getLastTracksLayoutOrientation() const;
    void storeTracksLayoutOrientation(Qt::Orientation newOrientation);

    bool writePresetToFile(const Preset &preset);
    void deletePreset(const QString &name);
    QStringList getPresetList();
    Preset readPresetFromFile(const QString &presetFileName, bool allowMultiSubchannels = true);

    int getLastSampleRate() const;
    int getLastBufferSize() const;

    QList<QString> getLastPrivateServers() const;
    quint32 getLastPrivateServerPort() const;
    QString getLastPrivateServerPassword() const;
    void addPrivateServer(const QString &server, int serverPort, const QString &password);

    // recording settings
    MultiTrackRecordingSettings getMultiTrackRecordingSettings() const;
    bool isSaveMultiTrackActivated() const;
    void setSaveMultiTrack(bool saveMultiTracks);
    bool isJamRecorderActivated(QString key) const;
    void setJamRecorderActivated(QString key, bool value);
    QString getRecordingPath() const;
    void setMultiTrackRecordingPath(const QString &newPath);

    // user name
    QString getUserName() const;
    void storeUserName(const QString &newUserName);
    void storeLastChannelName(int channelIndex, const QString &channelName);

    void setIntervalProgressShape(int shape);
    int getIntervalProgressShape() const;

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
    float getMetronomeGain() const;
    float getMetronomePan() const;
    bool getMetronomeMuteStatus() const;

    // +++++++++   Window  +++++++++++++++++++++++
    QPointF getLastWindowLocation() const;
    QSize getLastWindowSize() const;
    void setWindowSettings(bool windowIsMaximized, const QPointF &location, const QSize &size);

    bool windowWasMaximized() const;
    bool windowsWasFullScreenViewMode() const;
    void setFullScreenView(bool v);
    // ++++++++++++++++++++++++++++++++++++++++
    void setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int audioDevice);
    void setSampleRate(int newSampleRate);
    void setBufferSize(int bufferSize);

    int getFirstGlobalAudioInput() const;
    int getLastGlobalAudioInput() const;
    int getFirstGlobalAudioOutput() const;
    int getLastGlobalAudioOutput() const;
    int getLastAudioDevice() const;

    void setMidiSettings(const QList<bool> &inputDevicesStatus);

    QList<bool> getMidiInputDevicesStatus() const;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // TRANSLATION
    QString getTranslation() const;
    void setTranslation(const QString &localeName);

    QString getLastChannelName(int channelIndex) const;

    // Metering
    quint8 getMeterOption() const;
    bool isShowingMaxPeaks() const;
    quint8 getMeterRefreshRate() const;
    void storeMeterOption(quint8 meterOption);
    void storeMeterShowingMaxPeaks(bool showingMaxPeaks);
    void storeMeterRefreshRate(quint8 newRate);

    // Looper
    quint8 getLooperPreferredMode() const;
    quint8 getLooperPreferredLayersCount() const;
    QString getLooperSavePath() const;
    bool getLooperAudioEncodingFlag() const;
    QString getLooperFolder() const;
    quint8 getLooperBitDepth() const;

    void setLooperPreferredLayersCount(quint8 layersCount);
    void setLooperPreferredMode(quint8 looperMode);
    void setLooperAudioEncodingFlag(bool encodeAudioWhenSaving);
    void setLooperFolder(const QString &folder);
    void setLooperBitDepth(quint8 bitDepth);
};

inline void Settings::setLooperBitDepth(quint8 bitDepth)
{
    looperSettings.waveFilesBitDepth = bitDepth;
}

inline quint8 Settings::getLooperBitDepth() const
{
    return looperSettings.waveFilesBitDepth;
}

inline QString Settings::getLooperFolder() const
{
    return looperSettings.loopsFolder;
}

inline void Settings::setLooperFolder(const QString &folder)
{
    looperSettings.loopsFolder = folder;
}

inline bool Settings::getLooperAudioEncodingFlag() const
{
    return looperSettings.encodingAudioWhenSaving;
}

inline QString Settings::getLooperSavePath() const
{
    return looperSettings.loopsFolder;
}

inline quint8 Settings::getLooperPreferredLayersCount() const
{
    return looperSettings.preferredLayersCount;
}

inline quint8 Settings::getLooperPreferredMode() const
{
    return looperSettings.preferredMode;
}

inline quint8 Settings::getMeterOption() const
{
    return meteringSettings.meterOption;
}

inline bool Settings::isShowingMaxPeaks() const
{
    return meteringSettings.showingMaxPeakMarkers;
}

inline quint8 Settings::getMeterRefreshRate() const
{
    return meteringSettings.refreshRate;
}

inline void Settings::storeMeterOption(quint8 meterOption)
{
    meteringSettings.meterOption = meterOption;
}

inline void Settings::storeMeterShowingMaxPeaks(bool showingMaxPeaks)
{
    meteringSettings.showingMaxPeakMarkers = showingMaxPeaks;
}

inline void Settings::storeMeterRefreshRate(quint8 newRate)
{
    meteringSettings.refreshRate = newRate;
}

inline int Settings::getFirstGlobalAudioInput() const
{
    return audioSettings.firstIn;
}

inline int Settings::getLastGlobalAudioInput() const
{
    return audioSettings.lastIn;
}

inline int Settings::getFirstGlobalAudioOutput() const
{
    return audioSettings.firstOut;
}

inline int Settings::getLastGlobalAudioOutput() const
{
    return audioSettings.lastOut;
}

inline int Settings::getLastAudioDevice() const
{
    return audioSettings.audioDevice;
}

inline void Settings::setMidiSettings(const QList<bool> &inputDevicesStatus)
{
    midiSettings.inputDevicesStatus = inputDevicesStatus;
}

inline QList<bool> Settings::getMidiInputDevicesStatus() const
{
    return midiSettings.inputDevicesStatus;
}

inline bool Settings::windowWasMaximized() const
{
    return windowSettings.maximized;
}

inline bool Settings::windowsWasFullScreenViewMode() const
{
    return windowSettings.fullScreenMode;
}


inline float Settings::getMetronomeGain() const
{
    return metronomeSettings.gain;
}

inline float Settings::getMetronomePan() const
{
    return metronomeSettings.pan;
}

inline bool Settings::getMetronomeMuteStatus() const
{
    return metronomeSettings.muted;
}

// +++++++++   Window  +++++++++++++++++++++++
inline QPointF Settings::getLastWindowLocation() const
{
    return windowSettings.location;
}

inline QSize Settings::getLastWindowSize() const
{
    return windowSettings.size;
}

inline void Settings::setIntervalProgressShape(int shape)
{
    ninjamIntervalProgressShape = shape;
}

inline int Settings::getIntervalProgressShape() const
{
    return ninjamIntervalProgressShape;
}

inline MultiTrackRecordingSettings Settings::getMultiTrackRecordingSettings() const
{
    return recordingSettings;
}

inline bool Settings::isSaveMultiTrackActivated() const
{
    return recordingSettings.saveMultiTracksActivated;
}

inline void Settings::setSaveMultiTrack(bool saveMultiTracks)
{
    recordingSettings.saveMultiTracksActivated = saveMultiTracks;
}

inline bool Settings::isJamRecorderActivated(QString key) const
{
    return recordingSettings.isJamRecorderActivated(key);
}

inline void Settings::setJamRecorderActivated(QString key, bool value)
{
    recordingSettings.setJamRecorderActivated(key, value);
}

inline QString Settings::getRecordingPath() const
{
    return recordingSettings.recordingPath;
}

inline void Settings::setMultiTrackRecordingPath(const QString &newPath)
{
    recordingSettings.recordingPath = newPath;
}

// user name
inline QString Settings::getUserName() const
{
    return lastUserName;
}

inline float Settings::getLastMasterGain() const
{
    return masterFaderGain;
}

inline int Settings::getLastSampleRate() const
{
    return audioSettings.sampleRate;
}

inline int Settings::getLastBufferSize() const
{
    return audioSettings.bufferSize;
}

inline QList<QString> Settings::getLastPrivateServers() const
{
    return privateServerSettings.getLastServers();
}

inline quint32 Settings::getLastPrivateServerPort() const
{
    return privateServerSettings.getLastPort();
}

inline QString Settings::getLastPrivateServerPassword() const
{
    return privateServerSettings.getLastPassword();
}

inline void Settings::storeMasterGain(float newMasterFaderGain)
{
    masterFaderGain = newMasterFaderGain;
}

inline Qt::Orientation Settings::getLastTracksLayoutOrientation() const
{
    return tracksLayoutOrientation;
}

inline void Settings::storeTracksLayoutOrientation(Qt::Orientation newOrientation)
{
    tracksLayoutOrientation = newOrientation;
}

inline QString Settings::getTheme() const
{
    return theme;
}

inline QString Settings::getBuiltInMetronome() const
{
    return metronomeSettings.builtInMetronomeAlias;
}

inline QString Settings::getMetronomeOffBeatFile() const
{
    return metronomeSettings.customOffBeatAudioFile;
}

inline QString Settings::getMetronomeAccentBeatFile() const
{
    return metronomeSettings.customAccentBeatAudioFile;
}

inline void Settings::storeTracksSize(bool narrowedTracks)
{
    usingNarrowedTracks = narrowedTracks;
}

inline bool Settings::isUsingNarrowedTracks() const
{
    return usingNarrowedTracks;
}

inline LocalInputTrackSettings Settings::getInputsSettings() const
{
    return inputsSettings;
}

inline bool Settings::isUsingCustomMetronomeSounds() const
{
    return metronomeSettings.usingCustomSounds;
}

inline QString Settings::getMetronomeFirstBeatFile() const
{
    return metronomeSettings.customPrimaryBeatAudioFile;
}


inline void Settings::storeWaveDrawingMode(quint8 mode)
{
    meteringSettings.waveDrawingMode = mode;
}

inline quint8 Settings::getLastWaveDrawingMode() const
{
    return meteringSettings.waveDrawingMode;
}

inline float Settings::getEncodingQuality() const
{
    return audioSettings.encodingQuality;
}

inline void Settings::setEncodingQuality(float quality)
{
    audioSettings.encodingQuality = quality;
}

} // namespace

#endif
