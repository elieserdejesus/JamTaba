#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QJsonObject>

class AbstractJamRoom;
class RealTimeRoom;
class Peer;

class JsonUtils
{
    static void updateJamRoom(QJsonObject jamRoomObject, RealTimeRoom *jamRoom);
    static QString getIpFromPeer(bool isPrivateIP, QJsonObject peerJson);
    static int getPortFromPeer(bool isPrivatePort, QJsonObject peerJson);
public:
    static AbstractJamRoom *jamRoomFromJson(QJsonObject json);
    static Peer* peerFromJson(QJsonObject peerObject);
};

#endif // JSONUTILS_H
