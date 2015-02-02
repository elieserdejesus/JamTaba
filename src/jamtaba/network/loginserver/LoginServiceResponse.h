#pragma once

#include <QMap>
#include <QJsonObject>


namespace Model {
    class Peer;
    class AbstractJamRoom;
}

namespace Login {

class LoginServiceResponse {

private:
    QMap<long long, Model::AbstractJamRoom*> roomsMap;
    Model::Peer* connectedPeer;
    Model::AbstractJamRoom* currentRoom;
    int totalOnlineUsers;// = 0;
    Model::AbstractJamRoom* waitingRoom;

    //LoginServiceResponse(QList<AbstractJamRoom*> rooms, const Peer& connectedPeer, const AbstractJamRoom& currentRoom) ;

    static QList<Model::AbstractJamRoom*> buildJamRoomList(QJsonObject rootJsonObject);

public:

    LoginServiceResponse(QString json) ;

    const Model::AbstractJamRoom* getWaitingRoom() const ;

    //void addRoom(AbstractJamRoom room) ;

    const Model::AbstractJamRoom* getRoom(long id) const;

    QList<Model::AbstractJamRoom*> getRooms() const;

    const Model::Peer* getConnectedPeer() const;

    //void setCurrentRoom(NinjaMRoom room);

    const Model::AbstractJamRoom* getCurrentRoom() const;

    int getTotalOnlineUsers() const;

};

}
