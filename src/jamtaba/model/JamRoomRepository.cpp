#include "JamRoomRepository.h"
#include "JamRoom.h"

QMap<long long, std::shared_ptr<RealTimeRoom>> JamRoomRepository::realTimeRooms;

RealTimeRoom *JamRoomRepository::getRealTimeJamRoom(long long id)
{
    if(!realTimeRooms.contains(id)){
        realTimeRooms.insert(id, std::shared_ptr< RealTimeRoom>(new RealTimeRoom(id)));
    }
    return realTimeRooms[id].get();
}
