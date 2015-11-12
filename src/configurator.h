#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>

class Configurator
{
public:
    Configurator();
    bool homeExists();//check if Jamtaba 2 folder exists in application data
    void exportLogFile();
    void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    ~Configurator();
private :
     QString logFilename;//contains the name of the log file
};
extern Configurator *JTBConfig;
#endif // CONFIGURATOR_H
