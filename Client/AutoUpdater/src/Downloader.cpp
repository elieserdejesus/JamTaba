#include "Downloader.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <stdexcept>

Downloader::Downloader(QString downloadUrl, QString pathToSave, QString destFileName)
    :downloadUrl(downloadUrl), pathToSave(pathToSave), destFileName(destFileName){

    connect(&httpClient, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpClientReplyFinished(QNetworkReply*)));

}

void Downloader::start(){

    //try create the path to store the downloaded file
    QDir path;
    if(!path.mkpath(pathToSave)){
        QString errorString = "Can't create the path to store the downloaded file in" + pathToSave;
        throw std::runtime_error(errorString.toStdString());
    }
    else{
        qInfo() << "Path created at " << path.absolutePath();
    }

    downloadedFile.reset( new QFile(path.absoluteFilePath(destFileName)));

    if(!downloadedFile->open(QIODevice::WriteOnly | QIODevice::Truncate)){//erase the old file content
        throw std::runtime_error("Can't open the file to write ");
    }
    else{
        qInfo() << "Downloaded bytes will be writed in " << QFileInfo(*downloadedFile).absoluteFilePath();
    }

    doHttpRequest(downloadUrl);
}

void Downloader::doHttpRequest(QString url){

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader( "User-Agent" , "Mozilla Firefox" ); // :) Probabilly is not really need!

    qInfo() << endl << "Starting http request to" << url;
    reply.reset( httpClient.get(request));
    if(reply->error() != QNetworkReply::NoError){
        throw std::runtime_error(reply->errorString().toStdString());
    }

    connect(reply.data(), SIGNAL(finished()), this, SIGNAL(downloadFinished()));
    connect(reply.data(), SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgressed(qint64,qint64)));
    connect(reply.data(), SIGNAL(readyRead()), this, SLOT(bytesAvailable()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));

    emit downloadStarted();
}

//handling http redirection: http://stackoverflow.com/questions/14809310/qnetworkreply-and-301-redirect
void Downloader::httpClientReplyFinished(QNetworkReply *reply){

    if(!redirectUrl.isEmpty()){//avoid redirect loops
        return;
    }

    redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

    if(!redirectUrl.isEmpty() && redirectUrl != this->downloadUrl) { //we're being redirected?

        QUrl originalUrl(downloadUrl);
        QUrl resolvedUrl = originalUrl.resolved(redirectUrl);
        qDebug() << "Redirecting...";
        doHttpRequest(resolvedUrl.toString());//We'll do another request to the redirection url.
    }
    else{
        redirectUrl.clear();
    }
}

void Downloader::replyError(QNetworkReply::NetworkError){
    QString errorString = "NetworkError!";
    if(reply){
        errorString = reply->errorString();
    }
    qDebug() << "Network error: " << errorString;
    emit downloadError(errorString);
}

void Downloader::bytesAvailable(){
    if(downloadedFile && reply){
        downloadedFile->write(reply->readAll());//store the downloaded bytes in the file
    }

}
