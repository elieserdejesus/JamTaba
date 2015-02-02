#include "JamRoomRepository.h"
#include "JamRoom.h"

using namespace Model;
using Ninjam::Server;

QMap<long long, std::shared_ptr<RealTimeRoom>> JamRoomRepository::realTimeRooms;
QMap<long long, std::shared_ptr<NinjamRoom>> JamRoomRepository::ninjamRooms;

RealTimeRoom *JamRoomRepository::getRealTimeJamRoom(long long id)
{
    if(!realTimeRooms.contains(id)){
        realTimeRooms.insert(id, std::shared_ptr< RealTimeRoom>(new RealTimeRoom(id)));
    }
    return realTimeRooms[id].get();
}

NinjamRoom* JamRoomRepository::getNinjamRoom(long long id, Ninjam::Server* ninjamServer){
    if(!ninjamRooms.contains(id)){
        ninjamRooms.insert(id, std::shared_ptr<NinjamRoom>(new NinjamRoom(id, ninjamServer)));
    }
    return ninjamRooms[id].get();
}
