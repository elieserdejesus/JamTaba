#pragma once

//#include <QDebug>
#include <QString>
#include <QSettings>


namespace Persistence{

class ConfigStore {

    ~ConfigStore();

private:
    ConfigStore();//private construtor

    mutable QSettings settings;

    static const ConfigStore* instance;// = new ConfigStore();

    static const QString KEY_LAST_USER_NAME;
    static const QString KEY_LAST_CHANNEL_NAME;

    //audio
    static const QString KEY_LAST_BUFFER_SIZE;
    static const QString KEY_LAST_SAMPLE_RATE;
    static const QString KEY_LAST_INPUT_DEVICE;
    static const QString KEY_LAST_OUTPUT_DEVICE;

    //global audio preferences
    static const QString KEY_FIRST_GLOBAL_AUDIO_INPUT;
    static const QString KEY_FIRST_GLOBAL_AUDIO_OUTPUT;
    static const QString KEY_LAST_GLOBAL_AUDIO_INPUT;
    static const QString KEY_LAST_GLOBAL_AUDIO_OUTPUT;

    //local audio preferences
    //static const QString KEY_LAST_LOCAL_AUDIO_INPUT;

    //midi
    static const QString KEY_MIDI_INPUT;

    static const QString KEY_LAST_SEND_GAIN;
    static const QString KEY_LAST_BOOST_VALUE;
    static const QString KEY_LANGUAGE;
    static const QString KEY_INSTRUMENT_ID;

    //window
    static const QString KEY_WINDOW_MAXIMIZED;
    static const QString KEY_WINDOW_LOCATION;

    //ninjam
    static const QString KEY_LAST_PRIVATE_SERVER_NAME;
    static const QString KEY_LAST_PRIVATE_SERVER_PORT;
    static const QString KEY_LAST_PRIVATE_SERVER_PASS;
    static const QString KEY_METRONOME_GAIN;
    static const QString KEY_METRONOME_PAN;

    //record
    static const QString KEY_RECORD_PATH;
    static const QString KEY_RECORD_STATUS;

    static const QString DEFAULT_RECORD_PATH;

    static void saveProperty(QString key, QVariant value);
    static QVariant readProperty(QString key);

public:

    static QString getSettingsFilePath();

    static int getLastSampleRate() ;
    static int getLastBufferSize() ;

    //VST
    static void addVstPlugin(QString pluginPath);
    static QStringList getVstPluginsPaths();
    static void clearVstCache();

    //VST paths
    static void addVstScanPath(QString path);
    static void removeVstScanPath(int index);
    static QStringList getVstScanPaths();
    //+++++++++++++++++++++++++++++
    static void storeMetronomeSettings(float gain, float pan);
    
    static float getMetronomeGain();
    
    static float getMetronomePan();
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static void storeRecordStatus(bool status);
    
    static bool getRecordStatus();
    
    static void storeRecordPath(QString absoluteRecordPath) ;

    static QString getRecordPath() ;

    static void storePrivateServer(QString server, int port, QString password);

    static QString getLastPrivateServerHost();
    static int getLastPrivateServerPort();
    static QString getLastPrivateServerPassword();

    //+++++++++++++++++++++
    static void storeLocalControlsState(bool localControlsAreInMiniMode) ;

    static bool isLocalAudioControlsInMiniMode() ;

    static bool audioIOIsConfigured();

    //+++++++++   Window Location  +++++++++++++++++++++++
    static void storeWindowLocation(QPointF) ;

    static QPointF getLastWindowLocation() ;

    //+++++++++   Window Size +++++++++++++++++++++++
    static void storeWindowState(bool windowIsMaximized) ;

    static bool windowWasMaximized() ;

    //+++++++++   Instrument +++++++++++++++++++++++
    static void storeInstrument(int instrumentID);

    static int getLastInstrumentID() ;

    //+++++++++   LANGUAGE +++++++++++++++++++++++
    static void storeLanguage(QString lang) ;

    static QString getLastLanguage() ;

    //++++++++++++++++++++++++++++++++++++++++
    static void storeIOSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize, int midiDevice) ;

    static int getFirstGlobalAudioInput() ;
    static int getLastGlobalAudioInput() ;
    static int getFirstGlobalAudioOutput();
    static int getLastGlobalAudioOutput();
    static int getLastInputDevice();
    static int getLastOutputDevice();

    static int getLastMidiDeviceIndex() ;

    static void storeSendGain(float preGain) ;
    static float getLastSendGain() ;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static QString getLastUserName();
    static QString getLastChannelName(int channelIndex);

    static void storeLasUserName(QString userName);
    static void storeLastChannelName(QString channelName) ;
};

}
