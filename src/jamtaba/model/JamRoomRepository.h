#ifndef JAMROOMREPOSITORY_H
#define JAMROOMREPOSITORY_H

#include <QMap>
#include <memory>

class RealTimeRoom;

class JamRoomRepository  {

private:

    static QMap<long long, std::shared_ptr<RealTimeRoom>> realTimeRooms;

public:
    static RealTimeRoom *getRealTimeJamRoom(long long id);

    //protected static ConcurrentMap<NinjaMServer, NinjaMRoom> ninjaMRooms = new ConcurrentHashMap<NinjaMServer, NinjaMRoom>();
    //protected static final ConcurrentMap<Long, JamRoom> allRooms = new ConcurrentHashMap<Long, JamRoom>();

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
    public static synchronized RealtimeRoom getVSJamRoom(Long id) {
        RealtimeRoom jamRoom = vsRooms.get(id);
        if (jamRoom == null) {
            jamRoom = new RealtimeRoom(id);
            LOGGER.log(Level.FINE, "Jam instanciada: {0}", id);
            //@todo deletar instancias que não serão mais utilizadas?
            vsRooms.put(id, jamRoom);
        }
        return jamRoom;
    }
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

#endif // JAMROOMREPOSITORY_H
