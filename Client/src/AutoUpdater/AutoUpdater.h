#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QApplication>
#include <QNetworkAccessManager>
#include "Downloader.h"
#include <QScopedPointer>

class AutoUpdaterDialog;

class ExecutableVersionExtractor{
public:
    virtual QString getVersionString(QString executablePath) = 0;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++
class AutoUpdater : public QApplication
{
    Q_OBJECT
public:
    AutoUpdater(int argc, char** argv);

    void start();
    void setView(AutoUpdaterDialog* viewDialog);
private:
    QNetworkAccessManager httpClient;

    QString extractVersionInfoFromExecutable();

    bool updateIsRequired(QString executableVersionString);

    void runJamtabaExecutable(QString executablePath);

    void quitAndRunJamtaba();

    static QString EXECUTABLE_PATH;

    QScopedPointer<Downloader> downloader;

    AutoUpdaterDialog* viewDialog;

private slots:
    void downloadProgressed(qint64 received, qint64 total);
    void downloadStarted();
    void downloadFinished();
    void downloadError(QString);
};

#endif // AUTOUPDATER_H
