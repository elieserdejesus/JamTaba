#pragma once

#include <QMap>
#include <QJsonObject>


namespace Model {
    class Peer;
    class AbstractJamRoom;
    class RealTimeRoom;
    class NinjamRoom;
}

namespace Login {

class LoginServiceResponse {

private:
    QMap<long long, Model::RealTimeRoom*> realTimeRoomsMap;
    QMap<long long, Model::NinjamRoom*> ninjamServersMap;
    Model::Peer* connectedPeer;
    Model::AbstractJamRoom* currentRoom;
    int totalOnlineUsers;// = 0;
    Model::AbstractJamRoom* waitingRoom;

    //LoginServiceResponse(QList<AbstractJamRoom*> rooms, const Peer& connectedPeer, const AbstractJamRoom& currentRoom) ;

    static QList<Model::RealTimeRoom*> buildRealTimeJamRoomList(QJsonObject rootJsonObject);
    static QList<Model::NinjamRoom*> buildNinjamJamRoomList(QJsonObject rootJsonObject);

public:

    LoginServiceResponse(QString json) ;

    const Model::AbstractJamRoom* getWaitingRoom() const ;

    //void addRoom(AbstractJamRoom room) ;

    //const Model::AbstractJamRoom* getRoom(long id) const;

    QList<Model::RealTimeRoom*> getRealtimeRooms() const;
    QList<Model::NinjamRoom*> getNinjamRooms() const;

    const Model::Peer* getConnectedPeer() const;

    //void setCurrentRoom(NinjaMRoom room);

    const Model::AbstractJamRoom* getCurrentRoom() const;

    int getTotalOnlineUsers() const;

};

}
