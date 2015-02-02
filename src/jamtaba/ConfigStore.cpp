#include "ConfigStore.h"
#include <QDebug>
#include <QApplication>

using namespace Persistence;

const ConfigStore* ConfigStore::instance = new ConfigStore();

//++++++++++++++++++
const QString ConfigStore::KEY_LOCAL_AUDIO_CONTROLS_STATE = "GUI/localControlsInMiniMode";
const QString ConfigStore::KEY_LAST_USER_NAME = "USER/userName";
const QString ConfigStore::KEY_LAST_CHANNEL_NAME = "USER/channelName";

//audio
const QString ConfigStore::KEY_LAST_BUFFER_SIZE = "AUDIO/bufferSize";
const QString ConfigStore::KEY_LAST_SAMPLE_RATE = "AUDIO/sampleRate";
const QString ConfigStore::KEY_LAST_INPUT_DEVICE = "AUDIO/inputDevice";
const QString ConfigStore::KEY_LAST_OUTPUT_DEVICE = "AUDIO/outputDevice";
const QString ConfigStore::KEY_FIRST_AUDIO_INPUT = "AUDIO/firstIn";
const QString ConfigStore::KEY_FIRST_AUDIO_OUTPUT = "AUDIO/firstOut";
const QString ConfigStore::KEY_LAST_AUDIO_INPUT = "AUDIO/lastIn";
const QString ConfigStore::KEY_LAST_AUDIO_OUTPUT = "AUDIO/lastOut";


const QString ConfigStore::KEY_LAST_SEND_GAIN = "LOCAL INPUT/sendGain";
const QString ConfigStore::KEY_LAST_BOOST_VALUE = "LOCAL INPUT/inputBoost";
const QString ConfigStore::KEY_LANGUAGE = "language";
const QString ConfigStore::KEY_INSTRUMENT_ID = "instrument";

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
void ConfigStore::storeRecordStatus(bool status){
    saveProperty(ConfigStore::KEY_RECORD_STATUS, status);
}

bool ConfigStore::getRecordStatus(){
    return readProperty(ConfigStore::KEY_RECORD_STATUS) == "true";
}

void ConfigStore::storeRecordPath(QString absoluteRecordPath) {
    saveProperty(ConfigStore::KEY_RECORD_PATH, absoluteRecordPath);
}

//    QString ConfigStore::getRecordPath() {
//        QString prop = readProperty(ConfigStore::KEY_RECORD_PATH);
//        if (prop != null) {
//            Path path = FileSystems.getDefault().getPath(prop);
//            if (path.toFile().exists()) {
//                return path;
//            }
//            else{
//                LOGGER.severe("Record path don't existis, using default record path - " + prop);
//            }
//        }
//        return FileSystems.getDefault().getPath(DEFAULT_RECORD_PATH);
//    }

//    public  void storePrivateServer(PrivateRoomDialog.PrivateLoginData data) {
//        saveProperty(ConfigStore::KEY_LAST_PRIVATE_SERVER_NAME, data.getServerName());
//        saveProperty(ConfigStore::KEY_LAST_PRIVATE_SERVER_PASS, data.getPassword());
//        saveProperty(ConfigStore::KEY_LAST_PRIVATE_SERVER_PORT, String.valueOf(data.getServerPort()));
//    }

//    public  PrivateRoomDialog.PrivateLoginData getLastPrivateServerData() {
//        String serverName = readProperty(ConfigStore::KEY_LAST_PRIVATE_SERVER_NAME);
//        int serverPort = 2049;
//        try {
//            serverPort = Integer.parseInt(readProperty(ConfigStore::KEY_LAST_PRIVATE_SERVER_PORT));
//        } catch (NumberFormatException e) {
//            //
//        }
//        String password = readProperty(ConfigStore::KEY_LAST_PRIVATE_SERVER_PASS);
//        return new PrivateRoomDialog.PrivateLoginData(serverName, serverPort, password);
//    }

//    //+++++++++++++++++++++
//    public  void storeLocalControlsState(boolean localControlsAreInMiniMode) {
//        saveProperty(ConfigStore::KEY_LOCAL_AUDIO_CONTROLS_STATE, String.valueOf(localControlsAreInMiniMode));
//    }

//    public  boolean isLocalAudioControlsInMiniMode() {
//        String prop = readProperty(ConfigStore::KEY_LOCAL_AUDIO_CONTROLS_STATE);
//        if (prop != null) {
//            try {
//                return Boolean.valueOf(prop);
//            } catch (Exception e) {
//                return false;
//            }
//        }
//        return false;
//    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ConfigStore::storeAudioSettings(int firstIn, int lastIn, int firstOut, int lastOut, int inputDevice, int outputDevice, int sampleRate, int bufferSize){
    saveProperty(KEY_FIRST_AUDIO_INPUT,  firstIn);
    saveProperty(KEY_LAST_AUDIO_INPUT ,  lastIn);
    saveProperty(KEY_FIRST_AUDIO_OUTPUT, firstOut);
    saveProperty(KEY_LAST_AUDIO_OUTPUT,  lastOut);
    saveProperty(KEY_LAST_INPUT_DEVICE,  inputDevice);
    saveProperty(KEY_LAST_OUTPUT_DEVICE, outputDevice);
    saveProperty(KEY_LAST_SAMPLE_RATE, sampleRate);
    saveProperty(KEY_LAST_BUFFER_SIZE, bufferSize);
}

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

//    //+++++++++   Instrument +++++++++++++++++++++++
//    public  void storeInstrument(int instrumentID) {
//        saveProperty(ConfigStore::KEY_INSTRUMENT_ID, String.valueOf(instrumentID));
//    }

//    public  int getLastInstrumentID() {
//        String stringID = readProperty(ConfigStore::KEY_INSTRUMENT_ID);
//        if (stringID != null) {
//            try {
//                return Integer.parseInt(stringID);
//            } catch (Exception e) {
//                LOGGER.log(Level.SEVERE, e.getMessage(), e);
//            }
//        }
//        return 0;
//    }

//+++++++++   LANGUAGE +++++++++++++++++++++++
//    public  void storeLanguage(String lang) {
//        saveProperty(ConfigStore::KEY_LANGUAGE, lang);
//    }

//    public  String getLastLanguage() {
//        return readProperty(ConfigStore::KEY_LANGUAGE);
//    }


//++++++++++++++++++++++++++++++++++++++++
//    public  BoostProcessor.BoostValue getLastBoostValue() {
//        try {
//            int boostCode = Integer.parseInt(readProperty(ConfigStore::KEY_LAST_BOOST_VALUE));
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

//    public  void storeBoostValue(BoostProcessor.BoostValue v) {
//        int boostCode = 0;
//        if (v == BoostProcessor.NEGATIVE_BOOST) {
//            boostCode = -1;
//        } else if (v == BoostProcessor.POSITIVE_BOOST) {
//            boostCode = 1;
//        }
//        saveProperty(ConfigStore::KEY_LAST_BOOST_VALUE, String.valueOf(boostCode));
//    }

//++++++++++++++++++++++++++++++++++++++++

int ConfigStore::getFirstAudioInput() {
    return readProperty(ConfigStore::KEY_FIRST_AUDIO_INPUT).toInt();
}

int ConfigStore::getLastAudioInput(){
    return readProperty(ConfigStore::KEY_LAST_AUDIO_INPUT).toInt();
}

int ConfigStore::getFirstAudioOutput() {
    return readProperty(ConfigStore::KEY_FIRST_AUDIO_OUTPUT).toInt();
}

int ConfigStore::getLastAudioOutput(){
    return readProperty(ConfigStore::KEY_LAST_AUDIO_OUTPUT).toInt();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ConfigStore::ConfigStore(){
    //QString propertiesFile = QApplication::applicationDirPath().left(1) + ":/jamtaba.ini";
    props = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Jamtaba 2", "jamtaba");
}

ConfigStore::~ConfigStore(){
    delete props;
}

void ConfigStore::storeSendGain(float preGain) {
    saveProperty(ConfigStore::KEY_LAST_SEND_GAIN, preGain);
}

float ConfigStore::getLastSendGain() {
    bool ok;
    float result = readProperty(ConfigStore::KEY_LAST_SEND_GAIN).toFloat(&ok);
    return ok ? result : 1;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
    instance->props->setValue(key, value);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QVariant ConfigStore::readProperty(QString key) {
    return instance->props->value(key);
}
