#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QUuid>

#include <cmath>

#include "ninjam/Service.h"
#include "ninjam/Server.h"
#include "ninjam/User.h"

#include "Windows.h"

#include "audio/vorbis/VorbisEncoder.h"
#include "audio/vorbis/Vorbis.h"

QByteArray createGUID()
{
    QUuid uuid = QUuid::createUuid();
    return uuid.toRfc4122();
}

using namespace Ninjam;
using namespace Audio;

int main(int argc, char **argv)
{

    QCoreApplication app(argc, argv);

    Service service;

    QObject::connect(&service, &Service::connectedInServer, [&](const Ninjam::Server &server){

        qDebug() << "connected in server" << server.getUniqueName();

        Sleep(2000); // from windows.h // wait until send the encoded audio to server

        QFile file(":/audio_and_video.ogv");
        //QFile file(":/audio_only.ogg");
        if (!file.open(QIODevice::ReadOnly))
            qCritical() << file.errorString();

        QByteArray allData(file.readAll());

        // send the encoded data to server
        quint8 channelIndex = 0;
        bool isAudioInterval = true;

        QByteArray guid = QUuid::createUuid().toRfc4122();

        service.sendIntervalBegin(guid, channelIndex, isAudioInterval);

        while (!allData.isEmpty()) {
            auto size = qMin(allData.size(), 1024); // data is sended in 1024 bytes chunks
            QByteArray chunk(allData.data(), size);
            service.sendIntervalPart(guid, chunk, false);
            allData.remove(0, size);
        }

        service.sendIntervalPart(guid, QByteArray(), true); //last part

    });

    QObject::connect(&service, &Service::serverBpiChanged, [](quint16 bpi, quint16){
        qDebug() << "Bpi changed to " << bpi << ", sending data";
    });

    QObject::connect(&service, &Service::serverTopicMessageReceived , [](const QString &topic){
        qDebug() << "Server topic:" << topic;
    });

    QObject::connect(&service, &Service::disconnectedFromServer, [](const Ninjam::Server &server){
        qDebug() << "disconnected from server" << server.getUniqueName();
    });

    QObject::connect(&service, &Service::publicChatMessageReceived, [](const Ninjam::User &sender, const QString &message){
        qDebug() << "Chat message received from" << sender.getName() << ":" << message;
    });

    QObject::connect(&service, &Service::error, [](const QString &msg){
        qDebug() << "Error:" << msg;
    });

    auto serverIP = "localhost";
    auto serverPort = 2049;
    auto userName = "tester";
    auto userChannels = QStringList() << "channel";
    service.startServerConnection(serverIP, serverPort, userName, userChannels);

    return app.exec();
}
