#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QString>

class Configurator
{
public:
    Configurator();
    bool homeExists();//check if Jamtaba 2 folder exists in application data
    void exportLogFile();
    void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    QString getLogConfigFilePath();

    ~Configurator();
private :
     QString logFilename;//contains the name of the log file
     bool logFileCreated ;
};
extern Configurator *JTBConfig;
#endif // CONFIGURATOR_H
