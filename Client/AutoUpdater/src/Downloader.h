#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QFile>

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(QString downloadUrl, QString pathToSave, QString destFileName);
    void start();
private:
    QString downloadUrl;
    QString redirectUrl;
    QString pathToSave;
    QString destFileName;
    QNetworkAccessManager httpClient;
    QScopedPointer<QNetworkReply> reply;

    QScopedPointer<QFile> downloadedFile;

    void doHttpRequest(QString url);

signals:
    void downloadStarted();
    void downloadFinished();
    void downloadProgressed(qint64 received, qint64 total);
    void downloadError(QString error);

private slots:
    void bytesAvailable();
    void replyError(QNetworkReply::NetworkError);

    void httpClientReplyFinished(QNetworkReply*);

};

#endif // DOWNLOADER_H
