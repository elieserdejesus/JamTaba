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

namespace persistence {

class Settings;

class SettingsObject // base class for the settings components
{

public:
    explicit SettingsObject(const QString &name);
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
    QString audioInputDevice;
    QString audioOutputDevice;
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

class CollapseSettings : public SettingsObject
{
public:
    CollapseSettings();
    bool localChannelsCollapsed;
    bool bottomSectionCollapsed;
    bool chatSectionCollapsed;
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
    QString recordingPath;
    QString dirNameDateFormat;

    inline bool isJamRecorderActivated(const QString &key) const
    {
        if (jamRecorderActivated.contains(key))
            return jamRecorderActivated[key];
        return false;
    }
    inline void setJamRecorderActivated(const QString &key, bool value)
    {
        jamRecorderActivated[key] = value;
    }

    inline Qt::DateFormat getDirNameDateFormat() const
    {
        if (QString::compare("Qt::ISODate", dirNameDateFormat) == 0) {
            return Qt::ISODate;
        } else {
            return Qt::TextDate;
        }
    }
    inline void setDirNameDateFormat(const Qt::DateFormat dateFormat)
    {
        switch (dateFormat) {
        case Qt::ISODate:
            dirNameDateFormat = "Qt::ISODate";
            break;
        default:
            dirNameDateFormat = "Qt::TextDate";
            break;
        }
    }

private:
    static QString getDefaultRecordingPath();
    QMap <QString, bool> jamRecorderActivated;
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

private:
    void setDefaultLooperFilesPath();
};

// +++++++++++++++++++++++++++++++++

class Plugin
{
public:

    Plugin(const audio::PluginDescriptor &descriptor, bool bypassed, const QByteArray &data = QByteArray());
    QString path;
    QString name;
    QString manufacturer;
    bool bypassed;
    QByteArray data; // saved data to restore in next jam session
    audio::PluginDescriptor::Category category; // VST, AU, NATIVE plugin
};

// +++++++++++++++++++++++++++++++++

class SubChannel
{
public:
    SubChannel(int firstInput, int channelsCount, int midiDevice, int midiChannel, float gain,
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

    inline QList<persistence::Plugin> getPlugins() const
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
    QList<persistence::Plugin> plugins;
};

// +++++++++++++++++++++++++++++++++

class Channel
{
public:
    explicit Channel(int instrumentIndex);
    int instrumentIndex;
    QList<SubChannel> subChannels;
};

// +++++++++++++++++++++++++++++++++

class LocalInputTrackSettings : public SettingsObject
{
public:
    explicit LocalInputTrackSettings(bool createOneTrack = false);
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

class RememberCollapsableSectionsSettings : public SettingsObject
{
    RememberCollapsableSectionsSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;


};

class RememberSettings : public SettingsObject
{
public:
    RememberSettings();
    void write(QJsonObject &out) const override;
    void read(const QJsonObject &in) override;

    // user settings
    bool rememberPan;
    bool rememberBoost;
    bool rememberLevel; // fader
    bool rememberMute;
    bool rememberLowCut;

    // collapsible section settings
    bool rememberLocalChannels; // local channels are collapsed?
    bool rememberBottomSection; // bottom section (master fader) is collapsed?
    bool rememberChatSection; // chat is collapsed?

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
    RememberSettings rememberSettings;
    CollapseSettings collapseSettings;

    QStringList recentEmojis;

    QString lastUserName;               // the last nick name choosed by user
    QString translation;                // the translation language (en, fr, jp, pt, etc.) being used in chat
    QString theme;                      // the style sheet used
    int ninjamIntervalProgressShape;    // Circle, Ellipe or Line
    float masterFaderGain;              // last master fader gain
    quint8 tracksLayoutOrientation;     // horizontal or vertical
    bool usingNarrowedTracks;           // narrow or wide tracks?
    bool publicChatActivated;

    uint intervalsBeforeInactivityWarning;

    qint8 chatFontSizeOffset;

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

    quint8 getLastTracksLayoutOrientation() const;
    void storeTracksLayoutOrientation(quint8 newOrientation);

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
    bool isJamRecorderActivated(const QString &key) const;
    void setJamRecorderActivated(const QString &key, bool value);
    QString getRecordingPath() const;
    void setMultiTrackRecordingPath(const QString &newPath);
    QString getDirNameDateFormat() const;
    void setDirNameDateFormat(const QString &newDateFormat);

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

    QStringList getRecentEmojis() const;
    void setRecentEmojis(const QStringList &emojis);

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
    void setAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, QString audioInputDevice, QString audioOutputDevice);
    void setSampleRate(int newSampleRate);
    void setBufferSize(int bufferSize);

    int getFirstGlobalAudioInput() const;
    int getLastGlobalAudioInput() const;
    int getFirstGlobalAudioOutput() const;
    int getLastGlobalAudioOutput() const;
    QString getLastAudioInputDevice() const;
    QString getLastAudioOutputDevice() const;

    void setMidiSettings(const QList<bool> &inputDevicesStatus);

    QList<bool> getMidiInputDevicesStatus() const;

    qint8 getChatFontSizeOffset() const;
    void storeChatFontSizeOffset(qint8 sizeOffset);

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

    // Remember settings
    void setRemoteUserRememberingSettings(bool boost, bool level, bool pan, bool mute, bool lowCut);
    void setCollapsileSectionsRememberingSettings(bool localChannels, bool bottomSection, bool chatSection);
    bool isRememberingBoost() const;
    bool isRememberingLevel() const;
    bool isRememberingPan() const;
    bool isRememberingMute() const;
    bool isRememberingLowCut() const;

    // remembering collapsible sections
    bool isRememberingLocalChannels() const;
    bool isRememberingBottomSection() const;
    bool isRememberingChatSection() const;

    void setLocalChannelsCollapsed(bool collapsed);
    void setBottomSectionCollapsed(bool collapsed);
    void setChatSectionCollapsed(bool collapsed);
    bool isLocalChannelsCollapsed() const;
    bool isBottomSectionCollapsed() const;
    bool isChatSectionCollapsed() const;

    uint getIntervalsBeforeInactivityWarning() const;

    bool publicChatIsActivated() const;
    void setPublicChatActivated(bool activated);
};

inline void Settings::setPublicChatActivated(bool activated)
{
    publicChatActivated = activated;
}

inline bool Settings::publicChatIsActivated() const
{
    return publicChatActivated;
}

inline void Settings::setLocalChannelsCollapsed(bool collapsed)
{
    collapseSettings.localChannelsCollapsed = collapsed;
}

inline void Settings::setBottomSectionCollapsed(bool collapsed)
{
    collapseSettings.bottomSectionCollapsed = collapsed;
}

inline void Settings::setChatSectionCollapsed(bool collapsed)
{
    collapseSettings.chatSectionCollapsed = collapsed;
}

inline bool Settings::isLocalChannelsCollapsed() const
{
    return collapseSettings.localChannelsCollapsed;
}

inline bool Settings::isBottomSectionCollapsed() const
{
    return collapseSettings.bottomSectionCollapsed;
}

inline bool Settings::isChatSectionCollapsed() const
{
    return collapseSettings.chatSectionCollapsed;
}

inline void Settings::storeChatFontSizeOffset(qint8 sizeOffset)
{
    chatFontSizeOffset = sizeOffset;
}

inline qint8 Settings::getChatFontSizeOffset() const
{
    return chatFontSizeOffset;
}

inline QStringList Settings::getRecentEmojis() const
{
    return recentEmojis;
}

inline void Settings::setRecentEmojis(const QStringList &emojis)
{
    recentEmojis = emojis;
}

inline uint Settings::getIntervalsBeforeInactivityWarning() const
{
    return intervalsBeforeInactivityWarning;
}

inline bool Settings::isRememberingLocalChannels() const
{
    return rememberSettings.rememberLocalChannels;
}

inline bool Settings::isRememberingBottomSection() const
{
    return rememberSettings.rememberBottomSection;
}

inline bool Settings::isRememberingChatSection() const
{
    return rememberSettings.rememberChatSection;
}

inline bool Settings::isRememberingMute() const
{
    return rememberSettings.rememberMute;
}

inline bool Settings::isRememberingLowCut() const
{
    return rememberSettings.rememberLowCut;
}

inline bool Settings::isRememberingBoost() const
{
    return rememberSettings.rememberBoost;
}

inline bool Settings::isRememberingLevel() const
{
    return rememberSettings.rememberLevel;
}

inline bool Settings::isRememberingPan() const
{
    return rememberSettings.rememberPan;
}

// -----------------------------------------------------

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

inline QString Settings::getLastAudioInputDevice() const
{
    return audioSettings.audioInputDevice;
}

inline QString Settings::getLastAudioOutputDevice() const
{
    return audioSettings.audioOutputDevice;
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

inline bool Settings::isJamRecorderActivated(const QString &key) const
{
    return recordingSettings.isJamRecorderActivated(key);
}

inline void Settings::setJamRecorderActivated(const QString &key, bool value)
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

inline QString Settings::getDirNameDateFormat() const
{
    return recordingSettings.dirNameDateFormat;
}

inline void Settings::setDirNameDateFormat(const QString &newDateFormat)
{
    recordingSettings.dirNameDateFormat = newDateFormat;
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

inline quint8 Settings::getLastTracksLayoutOrientation() const
{
    return tracksLayoutOrientation;
}

inline void Settings::storeTracksLayoutOrientation(quint8 newOrientation)
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
