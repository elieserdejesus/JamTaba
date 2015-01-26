#ifndef LOGINSERVICERESPONSE_H
#define LOGINSERVICERESPONSE_H

#include <QMap>
#include <QJsonObject>

class Peer;
class AbstractJamRoom;

class LoginServiceResponse {

private:
    QMap<long long, AbstractJamRoom*> roomsMap;
    Peer* connectedPeer;
    AbstractJamRoom* currentRoom;
    int totalOnlineUsers;// = 0;
    AbstractJamRoom* waitingRoom;

    //LoginServiceResponse(QList<AbstractJamRoom*> rooms, const Peer& connectedPeer, const AbstractJamRoom& currentRoom) ;

    static QList<AbstractJamRoom*> buildJamRoomList(QJsonObject rootJsonObject);

public:

    LoginServiceResponse(QString json) ;

    const AbstractJamRoom* getWaitingRoom() const ;

    //void addRoom(AbstractJamRoom room) ;

    const AbstractJamRoom* getRoom(long id) const;

    QList<AbstractJamRoom*> getRooms() const;

    const Peer* getConnectedPeer() const;

    //void setCurrentRoom(NinjaMRoom room);

    const AbstractJamRoom* getCurrentRoom() const;

    int getTotalOnlineUsers() const;

};

#endif // LOGINSERVICERESPONSE_H
