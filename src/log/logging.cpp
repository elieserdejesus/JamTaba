#include "logging.h"
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QApplication> //to check plugin or standalone

Q_LOGGING_CATEGORY(jtCore,                  "jt.Core")
Q_LOGGING_CATEGORY(jtLoginService,          "jt.LoginService")
Q_LOGGING_CATEGORY(jtNinjamCore,            "jt.Ninjam.Core")
Q_LOGGING_CATEGORY(jtNinjamProtocol,        "jt.Ninjam.Protocol")
Q_LOGGING_CATEGORY(jtNinjamGUI,             "jt.Ninjam.GUI")
Q_LOGGING_CATEGORY(jtGUI,                   "jt.GUI")
Q_LOGGING_CATEGORY(jtNinjamVorbisEncoder,   "jt.Ninjam.VorbisEncoder")
Q_LOGGING_CATEGORY(jtNinjamVorbisDecoder,   "jt.Ninjam.VorbisDecoder")
Q_LOGGING_CATEGORY(jtNinjamRoomStreamer,    "jt.Ninjam.RoomStreamer")
Q_LOGGING_CATEGORY(jtJamRecorder,           "jt.JamRecorder")
Q_LOGGING_CATEGORY(jtIpToLocation,          "jt.IpToLocation")
Q_LOGGING_CATEGORY(jtStandaloneVstPlugin,   "jt.Standalone.VstPlugin")
Q_LOGGING_CATEGORY(jtStandaloneVstHost,     "jt.Standalone.VstHost")
Q_LOGGING_CATEGORY(jtStandalonePluginFinder,"jt.Standalone.PluginFinder")
Q_LOGGING_CATEGORY(jtVstPlugin,             "jt.VstPlugin")
Q_LOGGING_CATEGORY(jtAudio,                 "jt.Audio")
Q_LOGGING_CATEGORY(jtMidi,                  "jt.Midi")
Q_LOGGING_CATEGORY(jtConfigurator,          "jt.Configurator")


/*
static bool logFileCreated = false;

void jamtabaLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString stringMsg;
    QString fullFileName(context.file);
    QString file;
    int lastPathSeparatorIndex = fullFileName.lastIndexOf(QDir::separator());
    if(lastPathSeparatorIndex){
        file = fullFileName.right(fullFileName.size() - lastPathSeparatorIndex - 1);//.toStdString().c_str();
    }
    else{
        file = fullFileName;//.toStdString().c_str();
    }

    QTextStream stream(&stringMsg);
    switch (type) {
    case QtDebugMsg:
        stream << context.category << ".DEBUG:  " << localMsg.constData() << " "  << " in " << file << " " << context.line << endl;
        break;
    case QtWarningMsg:
        stream << context.category << ".WARNING:  " << localMsg.constData() <<  context.function <<  " " << file << context.line << endl << endl;
        break;
    case QtCriticalMsg:
        stream << context.category << ".CRITICAL:  " << localMsg.constData() <<  context.function << " " << file << context.line << endl << endl;
        break;
    case QtFatalMsg:
        stream << context.category  << ".FATAL:  " << localMsg.constData() << context.function << file << context.line << endl << endl;
        break;
    default:
       stream << context.category << ".INFO:  " << localMsg.constData() <<endl;
    }
    QTextStream(stdout) << stringMsg;

    QDir logDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if(!logDir.exists()){
        //check if we are plugin or standalone
        QString s=QApplication::instance()->applicationName();
        qDebug() << "Jamtaba Instance:" << s;
        logDir.mkpath(".");
    }
    QFile outFile( logDir.absoluteFilePath("logg.txt"));
    QIODevice::OpenMode ioFlags = QIODevice::WriteOnly;
    if(logFileCreated)
        ioFlags |= QIODevice::Append;
    else{
        ioFlags |= QIODevice::Truncate;
        logFileCreated = true;
    }
    if(outFile.open(ioFlags)){
        QTextStream ts(&outFile);
        ts << stringMsg;
    }

    if(type == QtFatalMsg){
        abort();
    }
}
*/
