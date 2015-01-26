#include "JsonUtils.h"
#include "../model/JamRoom.h"
#include "../model/JamRoomRepository.h"
#include "../model/Peer.h"
#include <QJsonArray>
#include <QDebug>

 AbstractJamRoom *JsonUtils::jamRoomFromJson(QJsonObject json)
{
     long long id = json.value("id").toVariant().toLongLong();
     //obtenho sempre a mesma instância de JamRoom e seto os valores que chegaram do server. A classe
     //JamRoom verifica se os valores que chegaram do server são diferentes dos valores antigos.
     //Caso os valores sejam diferentes os JamRoomListeners são avisados das mudanças.
     RealTimeRoom* jamRoom = JamRoomRepository::getRealTimeJamRoom(id);
     updateJamRoom(json, jamRoom);
     return (AbstractJamRoom*)jamRoom;
}

 void JsonUtils::updateJamRoom(QJsonObject jamRoomObject, RealTimeRoom* jamRoom) {
         QString name = jamRoomObject["name"].toString();
         //long id = jamRoomObject["id"].toInt();
         int maxUsers = jamRoomObject["maxUsers"].toInt();
         bool isStatic = jamRoomObject["isStatic"].toBool();

         QMap<long long, Peer*> peersInServer;
         if (jamRoomObject["peers"] != QJsonValue::Undefined) {
             QJsonArray peersArray = jamRoomObject["peers"].toArray();

             for (int i = 0; i < peersArray.size(); ++i) {
                 Peer* peer = JsonUtils::peerFromJson( peersArray[i].toObject());
                 peersInServer.insert(peer->getId(), peer);
             }
             //jamRoom->refreshPeerList(peersInServer);
         }

         //JamRoomStyle style = JamRoomStyles.getJamRoomStyle(styleCode);
         //jamRoom.setStyle(style);
         //jamRoom->setIsStatic(isStatic);
         //jamRoom->setName(name);
         //jamRoom->setMaxUsers((int) maxUsers);
         jamRoom->set(name, isStatic, maxUsers, peersInServer);
     }

 Peer *JsonUtils::peerFromJson(QJsonObject peerObject)
 {
     long long id = peerObject.value("id").toVariant().toLongLong();
     QString name =  peerObject["name"].toString();
     QString region = peerObject["region"].toString();
     QString country = peerObject["country"].toString();
     QString channelName = peerObject["channelName"].toString();
     int publicPort = getPortFromPeer(false, peerObject);
     QString publicIp = getIpFromPeer(false, peerObject);
     QJsonValue instrumentIDObject = peerObject["instrumentID"];
     int instrumentID = (instrumentIDObject != QJsonValue::Undefined) ? instrumentIDObject.toInt() : 0;
     int version = peerObject["version"].toInt();
     //int bufferSize = Integer.parseInt(peerObject.get("bufferSize").toString());
     int sampleRate = peerObject["sampleRate"].toInt();
     QString environment = peerObject["environment"].toString();

     //bool playingInNinjam = peerObject["playingInNinjam"].toBool();
     //retorna a instância do repositório.
     Peer* p = Peer::getPeer(publicIp, publicPort, name, region, country, instrumentID, channelName, version, environment, sampleRate, id);
     //p->setPlayingInNinjam(playingInNinjam);
     return p;
 }

 QString JsonUtils::getIpFromPeer(bool isPrivateIP, QJsonObject peerJson) {
     QString ip = "NO-IP";
     if (peerJson.contains("privateIP") || peerJson.contains("publicIP")) {
         if (isPrivateIP) {
             return peerJson["privateIP"].toString();
         }
         return peerJson["publicIP"].toString();
     }
     return ip;
 }

 int JsonUtils::getPortFromPeer(bool isPrivatePort, QJsonObject peerJson) {
     int port = 0;
     if (peerJson.contains("privatePort") || peerJson.contains("publicPort")) {
         if (isPrivatePort) {
             return peerJson["privatePort"].toInt();
         }
         return peerJson["publicPort"].toInt();
     }
     return port;
 }
