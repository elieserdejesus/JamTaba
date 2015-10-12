#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QJsonObject>

namespace Login {

class RealTimeRoom;
class AbstractJamRoom;
class NinjamRoom;
class RealTimePeer;

class JsonUtils
{
    static void updateJamRoom(QJsonObject jamRoomObject, RealTimeRoom *jamRoom);
    static QString getIpFromPeer(bool isPrivateIP, QJsonObject peerJson);
    static int getPortFromPeer(bool isPrivatePort, QJsonObject peerJson);
public:
    static RealTimeRoom *realTimeRoomFromJson(QJsonObject json);
    static NinjamRoom* ninjamServerFromJson(QJsonObject json);
    static RealTimePeer* peerFromJson(QJsonObject peerObject);
};

}

#endif // JSONUTILS_H
