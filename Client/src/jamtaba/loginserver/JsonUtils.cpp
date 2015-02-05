//#include "JsonUtils.h"
//#include "../loginserver/JamRoom.h"
//#include <QJsonArray>
//#include <QDebug>

//using namespace Login;

//NinjamRoom *JsonUtils::ninjamServerFromJson(QJsonObject json)
//{
//    qDebug() << "JSON: " << json << endl;
//    long long id = json.value("id").toVariant().toLongLong();

//    //vou usar essa mesma estratégia de atualizar as salas?

//    //obtenho sempre a mesma instância de JamRoom e seto os valores que chegaram do server. A classe
//    //JamRoom verifica se os valores que chegaram do server são diferentes dos valores antigos.
//    //Caso os valores sejam diferentes os JamRoomListeners são avisados das mudanças.
//    QString serverHost = json["name"].toString();
//    int serverPort = json["port"].toInt();
//    NinjamRoom* server = JamRoomRepository::getNinjamRoom(id, Ninjam::Server::getServer(serverHost, serverPort));

//    QJsonArray usersArray = json["users"].toArray();
//    for (int i = 0; i < usersArray.size(); ++i) {
//        QJsonObject userObject = usersArray[i].toObject();
//        QString name = userObject["name"];
//        QString ip = userObject["ip"];
//        server->addPeer(Ninja);//como pegar esse peers?
//    }

//    return server;

//    /*
//JSONObject serverObject = new JSONObject();
//        serverObject.put("id", server.getUniqueName().hashCode());
//        serverObject.put("name", server.getHostName());
//        serverObject.put("port", server.getPort());
//        serverObject.put("maxUsers", server.getMaxUsers());
//        serverObject.put("bpm", server.getBpm());
//        serverObject.put("bpi", server.getBpi());
//        serverObject.put("streamUrl", server.getStreamUrl());
//        //users
//        JSONArray usersArray = new JSONArray();
//        for (NinjaMUser user : server.getUsers()) {
//            JSONObject userObject = new JSONObject();
//            userObject.put("name", user.getName());
//            userObject.put("ip", user.getIp());
//            usersArray.put(userObject);
//        }
//        serverObject.put("users", usersArray);
//     */
//}


//RealTimeRoom *JsonUtils::realTimeRoomFromJson(QJsonObject json)
//{
//     long long id = json.value("id").toVariant().toLongLong();
//     //obtenho sempre a mesma instância de JamRoom e seto os valores que chegaram do server. A classe
//     //JamRoom verifica se os valores que chegaram do server são diferentes dos valores antigos.
//     //Caso os valores sejam diferentes os JamRoomListeners são avisados das mudanças.
//     RealTimeRoom* jamRoom = JamRoomRepository::getRealTimeJamRoom(id);
//     updateJamRoom(json, jamRoom);
//     return jamRoom;
//}

// void JsonUtils::updateJamRoom(QJsonObject jamRoomObject, RealTimeRoom* jamRoom) {
//         QString name = jamRoomObject["name"].toString();
//         int maxUsers = jamRoomObject["maxUsers"].toInt();
//         bool isStatic = jamRoomObject["isStatic"].toBool();

//         QMap<long long, RealTimePeer*> peersInServer;
//         if (jamRoomObject["peers"] != QJsonValue::Undefined) {
//             QJsonArray peersArray = jamRoomObject["peers"].toArray();

//             for (int i = 0; i < peersArray.size(); ++i) {
//                 RealTimePeer* peer = JsonUtils::peerFromJson( peersArray[i].toObject());
//                 peersInServer.insert(peer->getId(), peer);
//             }
//             //jamRoom->refreshPeerList(peersInServer);
//         }
//         jamRoom->set(name, isStatic, maxUsers, peersInServer);
//     }

// RealTimePeer *JsonUtils::peerFromJson(QJsonObject peerObject)
// {
//     long long id = peerObject.value("id").toVariant().toLongLong();
//     QString name =  peerObject["name"].toString();
//     QString region = peerObject["region"].toString();
//     QString country = peerObject["country"].toString();
//     QString channelName = peerObject["channelName"].toString();
//     int publicPort = getPortFromPeer(false, peerObject);
//     QString publicIp = getIpFromPeer(false, peerObject);
//     QJsonValue instrumentIDObject = peerObject["instrumentID"];
//     int instrumentID = (instrumentIDObject != QJsonValue::Undefined) ? instrumentIDObject.toInt() : 0;
//     int version = peerObject["version"].toInt();
//     //int bufferSize = Integer.parseInt(peerObject.get("bufferSize").toString());
//     int sampleRate = peerObject["sampleRate"].toInt();
//     QString environment = peerObject["environment"].toString();

//     //bool playingInNinjam = peerObject["playingInNinjam"].toBool();
//     //retorna a instância do repositório.
//     RealTimePeer* p = RealTimePeer::getPeer(publicIp, publicPort, name, region, country, instrumentID, channelName, version, environment, sampleRate, id);
//     //p->setPlayingInNinjam(playingInNinjam);
//     return p;
// }

// QString JsonUtils::getIpFromPeer(bool isPrivateIP, QJsonObject peerJson) {
//     QString ip = "NO-IP";
//     if (peerJson.contains("privateIP") || peerJson.contains("publicIP")) {
//         if (isPrivateIP) {
//             return peerJson["privateIP"].toString();
//         }
//         return peerJson["publicIP"].toString();
//     }
//     return ip;
// }

// int JsonUtils::getPortFromPeer(bool isPrivatePort, QJsonObject peerJson) {
//     int port = 0;
//     if (peerJson.contains("privatePort") || peerJson.contains("publicPort")) {
//         if (isPrivatePort) {
//             return peerJson["privatePort"].toInt();
//         }
//         return peerJson["publicPort"].toInt();
//     }
//     return port;
// }
