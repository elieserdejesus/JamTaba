#ifndef JAMROOMREPOSITORY_H
#define JAMROOMREPOSITORY_H

#include <QMap>
#include <memory>

namespace Ninjam {
    class Server;
}

namespace Model {

class RealTimeRoom;
class NinjamRoom;

class JamRoomRepository  {

private:

    static QMap<long long, std::shared_ptr<RealTimeRoom>> realTimeRooms;
    static QMap<long long, std::shared_ptr<NinjamRoom>> ninjamRooms;

public:
    static RealTimeRoom *getRealTimeJamRoom(long long id);
    static NinjamRoom* getNinjamRoom(long long id, Ninjam::Server* server);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
    public static synchronized NinjaMRoom getNinjamMRoom(NinjaMServer ninjaMServer){
        NinjaMRoom room = ninjaMRooms.get(ninjaMServer);
        if(room == null){
            room = new NinjaMRoom(ninjaMServer, NinjaMService.getInstance());
            ninjaMRooms.put(ninjaMServer, room);
        }

        Map<Long, Peer> peersInServer = new HashMap<Long, Peer>();
        for (NinjaMUser ninjaMUser : ninjaMServer.getUsers()) {
            if(!ninjaMUser.isBot()){
                Peer peer = Peer.getPeer(ninjaMUser);
                peersInServer.put(peer.getId(), peer);
            }
        }
        room.refreshPeerList(peersInServer);
        return room;
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
    */
};

}
#endif // JAMROOMREPOSITORY_H
