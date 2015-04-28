#pragma once

#include <QDebug>
#include <QString>
#include <QSettings>


namespace Persistence{

class ConfigStore {

    ~ConfigStore();

private:
    ConfigStore();//private construtor

    QSettings* settings;

    static const ConfigStore* instance;// = new ConfigStore();

     static const QString KEY_LOCAL_AUDIO_CONTROLS_STATE;
     static const QString KEY_LAST_USER_NAME;
     static const QString KEY_LAST_CHANNEL_NAME;

     //audio
     static const QString KEY_LAST_BUFFER_SIZE;
     static const QString KEY_LAST_SAMPLE_RATE;
     static const QString KEY_LAST_INPUT_DEVICE;
     static const QString KEY_LAST_OUTPUT_DEVICE;
     static const QString KEY_FIRST_AUDIO_INPUT;
     static const QString KEY_FIRST_AUDIO_OUTPUT;
     static const QString KEY_LAST_AUDIO_INPUT;
     static const QString KEY_LAST_AUDIO_OUTPUT;

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


////++++++++++++++++++++++++++++++++++++++++
//    public static BoostProcessor.BoostValue getLastBoostValue() {
//        try {
//            int boostCode = Integer.parseInt(readProperty(KEY_LAST_BOOST_VALUE));
//            switch (boostCode) {
//                case 0:
//                    return BoostProcessor.NO_BOOST;
//                case 1:
//                    return BoostProcessor.POSITIVE_BOOST;
//                case -1:
//                    return BoostProcessor.NEGATIVE_BOOST;
//            }
//        } catch (Exception e) {
//            LOGGER.log(Level.INFO, e.getMessage(), e);
//        }
//        return BoostProcessor.NO_BOOST;
//    }

//    public static void storeBoostValue(BoostProcessor.BoostValue v) {
//        int boostCode = 0;
//        if (v == BoostProcessor.NEGATIVE_BOOST) {
//            boostCode = -1;
//        } else if (v == BoostProcessor.POSITIVE_BOOST) {
//            boostCode = 1;
//        }
//        saveProperty(KEY_LAST_BOOST_VALUE, QString.valueOf(boostCode));
//    }

    //++++++++++++++++++++++++++++++++++++++++
    static void storeAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize) ;

    static int getFirstAudioInput() ;
    static int getLastAudioInput() ;
    static int getFirstAudioOutput();
    static int getLastAudioOutput();

    static void storeSendGain(float preGain) ;
    static float getLastSendGain() ;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static QString getLastUserName();
    static QString getLastChannelName();

    static int getLastInputDevice();
    
    static int getLastOutputDevice();



    static void storeLasUserName(QString userName);

    static void storeLastChannelName(QString channelName) ;

//    /**
//     * *
//     * O número de vezes que o usuário utilizou o programa
//     */
//    public static void storeSessionsCount() {
//        int currentSessionsCount = getSessionsCount();
//        saveProperty(KEY_SESSIONS_COUNT, (currentSessionsCount + 1) + "");
//    }

//    public static bool isUserFirstSession() {
//        return getSessionsCount() == 0;
//    }

//    public static int getSessionsCount() {
//        QString sessionsCountQString = readProperty(KEY_SESSIONS_COUNT);
//        if (sessionsCountQString == null) {
//            return 0;
//        }
//        return Integer.parseInt(sessionsCountQString);
//    }
};

}
