package jamtaba;

import com.googlecode.objectify.Key;
import static com.googlecode.objectify.ObjectifyService.ofy;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;
import jamtaba.command.RefreshCommand;
import java.util.logging.Level;

/**
 * @author zeh
 */
public class DbUtils {

    private static final Logger LOGGER = Logger.getLogger(DbUtils.class.getName());

    public static Collection<RealtimeRoom> loadRooms() {
        return ofy().load().type(RealtimeRoom.class).list();
    }

    public static void save(Object p) {
        ofy().save().entity(p).now();
    }

    public static void save(Iterable entities) {
        ofy().save().entities(entities).now();
    }

    public static void deleteInactivePeers(int MAX_TIME_WITHOUT_UPDATES) {
        List toDelete = new ArrayList<Peer>();
        List<RealtimeRoom> roomsToSave = new ArrayList<RealtimeRoom>();
        //++++++++= CLEAN ROOMs ++++++++++++++++++=
        Collection<RealtimeRoom> rooms = DbUtils.loadRooms();
        for (RealtimeRoom room : rooms) {
            List<Peer> roomPeers = room.getPeers();
            if (!roomPeers.isEmpty()) {
                for (Peer peer : roomPeers) {
                    if (peer.getTimeSinceLastUpdate() >= RefreshCommand.MAX_TIME_WITHOUT_UPDATES) {
                        LOGGER.log(Level.WARNING, "Deleting inactive peer {0}  TimeSinceLastUpdate: {1}", new Object[]{peer.getId(), peer.getTimeSinceLastUpdate()});
                        toDelete.add(peer);
                        roomsToSave.add(room);
                        room.removePeer(peer.getId());
                    }
                }
            } else {
                if (!room.isStatic() && !room.isWaitingRoom()) {//deletable room without peers
                    toDelete.add(room);
                }
            }
        }
        ofy().delete().entities(toDelete);
        ofy().save().entities(roomsToSave);
        //LOGGER.info(inactivePeers.size() + " peers inativos deletados!");
    }

    public static RealtimeRoom getWaitingRoom() {
        Key<RealtimeRoom> key = Key.create(RealtimeRoom.class, RealtimeRoom.WAITING_ROOM_ID);
        return ofy().load().key(key).now();
    }

    public static void tryCreateCurrenVersion() {
        Version v = getCurrentVersion();
        if (v == null || v.getMajorVersion() < 2) {
            DbUtils.save(Version.fromString("2.0.2"));
        }
    }

    public static Version getCurrentVersion() {
        Key<Version> key = Key.create(Version.class, 1);
        return ofy().load().key(key).now();
    }

    static void createWaitingRoom(RealtimeRoom waitintRoom) {
        ofy().save().entity(waitintRoom).now();
    }

    public static RealtimeRoom getRoomByID(Long jamRoomID) {
        Key<RealtimeRoom> key = Key.create(RealtimeRoom.class, jamRoomID);
        return ofy().load().key(key).now();
    }

    public static void delete(Object o) {
        ofy().delete().entity(o);
    }
}
