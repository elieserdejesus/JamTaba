#include "AutoUpdater.h"
#include <exception>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "AutoUpdaterDialog.h"
#include <stdexcept>
#include "WindowsExecutableVersionExtractor.h"

QString AutoUpdater::EXECUTABLE_PATH = "./../Standalone/debug/Jamtaba2.exe";

AutoUpdater::AutoUpdater(int argc, char** argv)
 :QApplication(argc, argv)
{
    setQuitOnLastWindowClosed(true);
}

void AutoUpdater::setView(AutoUpdaterDialog *viewDialog){
    this->viewDialog = viewDialog;
}

void AutoUpdater::start(){
    QString downloadLink = "https://github.com/elieserdejesus/JamTaba/releases/download/v2.0.4/Jamtaba2_installer.32.bits.exe";
            //"https://download-installer.cdn.mozilla.net/pub/firefox/releases/41.0.2/win32/pt-BR/Firefox%20Setup%20Stub%2041.0.2.exe";
    downloader.reset(new Downloader(downloadLink, ".", "Teste-firefox.exe"));

    connect(downloader.data(), SIGNAL(downloadStarted()), this, SLOT(downloadStarted()));
    connect(downloader.data(), SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    connect(downloader.data(), SIGNAL(downloadProgressed(qint64,qint64)), this, SLOT(downloadProgressed(qint64,qint64)));
    connect(downloader.data(), SIGNAL(downloadError(QString)), this, SLOT(downloadError(QString)));

    try{
        qInfo() << "Starting downloader...";
        downloader->start();
    }
    catch(const std::runtime_error & e){
        qCritical() << e.what();
    }


//    try{
//        qDebug() << "Extracting info from executable...";
//        QString executableVersionString = extractVersionInfoFromExecutable();
//        qDebug() << "Executable version info:" << executableVersionString;

//        qInfo() << "Checking if a new version is available ...";
//        if(updateIsRequired(executableVersionString)){
//            qInfo() << "New version is available!";
//            qInfo() << "Downloading the new version...";

//        }
//        else{
//            qInfo() << "The current version "<< executableVersionString << " is the latest version!";
//        }
//    }
//    catch(const std::runtime_error& ex){
//        qCritical() << ex.what();
//        quitAndRunJamtaba();
//    }

}

bool AutoUpdater::updateIsRequired(QString executableVersionString){
    //check Jamtaba server if a new version is available
    QString serverEndPoint = "http://jamtaba2.appspot.com/vs?version=" + executableVersionString;
    QNetworkRequest request;
    request.setUrl(serverEndPoint);
    QNetworkReply* reply = httpClient.get(request);

    QEventLoop eventLoop; //synchronous http here to simplify
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    if( reply->error() != QNetworkReply::NoError){
        throw std::runtime_error(reply->errorString().toStdString());
    }

    //The json content is: {"clientCompatibility":true,"newVersionAvailable":false}
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject root = doc.object();
    qDebug() << "JSON DEBUG clientCompatibility:" << root["clientCompatibility"].toBool() << "  newVersionAvailable:" << root["newVersionAvailable"].toBool();
    bool jamtabaIsCompatible = root["clientCompatibility"].toBool(true);
    bool newVersionAvailable = root["newVersionAvailable"].toBool(false);

    return !jamtabaIsCompatible || newVersionAvailable;

}

QString AutoUpdater::extractVersionInfoFromExecutable(){
    ExecutableVersionExtractor* versionExtractor = new WindowsExecutableVersionExtractor();
    QString executableVersionString = versionExtractor->getVersionString(EXECUTABLE_PATH);
    if(executableVersionString.isEmpty()){
        throw std::runtime_error("Can't extract the info version from Jamtaba executable!");
    }
    return executableVersionString;
}

void AutoUpdater::runJamtabaExecutable(QString executablePath){
    qInfo() << "Starting executable...";
}

void AutoUpdater::quitAndRunJamtaba(){
    runJamtabaExecutable(EXECUTABLE_PATH);
    exit(0);
}

void AutoUpdater::downloadStarted(){
    qDebug() << "download started";
    viewDialog->setDownloadProgress(0);
}

void AutoUpdater::downloadFinished(){
    qDebug() << "download finished";
    viewDialog->setDownloadProgress(100);
}

void AutoUpdater::downloadProgressed(qint64 received, qint64 total){
    if(total > 0){
        int progress = ((double)received/total) * 100;
        viewDialog->setDownloadProgress(  progress );
    }
}

void AutoUpdater::downloadError(QString errorMsg){
    viewDialog->setStatusText(errorMsg);
}
