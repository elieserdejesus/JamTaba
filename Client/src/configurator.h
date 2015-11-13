#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QString>
#include <QStandardPaths>
#include <QDir>

void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

struct SHOMEDIR{bool exist;QDir Dir;};

class Configurator
{
private :
    QString logFilename;//contains the name of the log file
    SHOMEDIR HomeDir;

public:
    Configurator();

        void exportLogFile();
        QString getLogConfigFilePath();
        //check if Jamtaba 2 folder exists in application data
         bool homeExists();
         inline QDir getHomeDir(){return HomeDir.Dir ;}

    ~Configurator();

};
extern Configurator *JTBConfig;
#endif // CONFIGURATOR_H
