package jamtaba;

import com.google.appengine.labs.repackaged.org.json.JSONArray;
import com.google.appengine.labs.repackaged.org.json.JSONException;
import com.google.appengine.labs.repackaged.org.json.JSONObject;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;

public class VsJsonUtils {

    private static final Logger LOGGER = Logger.getLogger(VsJsonUtils.class.getName());

    public static String getJsonToConnectionInRoom(Room connectedRoom) throws JSONException {
        JSONObject json = new JSONObject();
        //json.put("peer", peerAsJSONObject(connectedPeer));
        //json.put("room", jamRoomToJSONObject(connectedRoom));
        return json.toString();// toJSONString();
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
    public static String getJsonToResponse(Collection<Room> jamRooms, Peer connectedPeer) throws JSONException {
        JSONObject json = new JSONObject();
        json.put("rooms", jamRoomListToJsonArray(jamRooms));
        if (connectedPeer != null) {
            json.put("peer", peerAsJSONObject(connectedPeer));
        }
        return json.toString();
    }

    public static String getJsonToResponse(List<Room> jamRooms) throws JSONException {
        return getJsonToResponse(jamRooms, null);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
    private static JSONArray jamRoomListToJsonArray(Collection<Room> rooms) throws JSONException {
        JSONArray jamRoomsArray = new JSONArray();
        for (Room jamRoom : rooms) {
            jamRoomsArray.put(jamRoomToJSONObject(jamRoom));
        }
        return jamRoomsArray;
    }

    private static JSONArray peerListToJsonArray(List<Peer> peers) throws JSONException {
        JSONArray array = new JSONArray();
        for (Peer peer : peers) {
            if (peer != null) {
                array.put(peerAsJSONObject(peer));
            }
        }
        return array;
    }

    public static String getJsonToJamRoomsList(List<Room> jamRooms) throws JSONException {
        JSONObject json = new JSONObject();
        json.put("rooms", jamRoomListToJsonArray(jamRooms));
        return json.toString();
    }

    public static String getJsonToJamRoom(Room jamRoom) throws JSONException {
        JSONObject room = new JSONObject();
        room.put("room", jamRoomToJSONObject(jamRoom));
        return room.toString();
    }

    private static JSONObject peerAsJSONObject(Peer peer) throws JSONException {
        JSONObject peerMap = new JSONObject();
        peerMap.put("id", peer.getId());
        peerMap.put("name", peer.getUserName());
        peerMap.put("publicIP", peer.getPublicIp());
        peerMap.put("publicPort", peer.getPublicPort());
        peerMap.put("region", peer.getRegion());
        peerMap.put("country", peer.getCountry());
        peerMap.put("playingInNinjam", peer.isPlayingInNinjam());
        peerMap.put("instrumentID", peer.getInstrumentID());
        peerMap.put("channelName", peer.getChannelName());
        peerMap.put("version", peer.getVersion());
        peerMap.put("environment", peer.getEnvironment());
        peerMap.put("bufferSize", 0);//peer.getAudioBufferSize());
        peerMap.put("sampleRate", peer.getSampleRate());

        Room peerRoom = peer.getRoom();
        if (peerRoom != null) {
            peerMap.put("room", jamRoomToJSONObject(peerRoom, false));
        }
        return peerMap;
    }

    private static JSONObject jamRoomToJSONObject(Room jamRoom) throws JSONException {
        return jamRoomToJSONObject(jamRoom, true);
    }

    private static JSONObject jamRoomToJSONObject(Room jamRoom, boolean includePeersList) throws JSONException {
        JSONObject jamRoomObject = new JSONObject();

        jamRoomObject.put("id", jamRoom.getId());
        jamRoomObject.put("name", jamRoom.getName());
        //jamRoomObject.put("styleCode", jamRoom.getStyleCode());
        jamRoomObject.put("maxUsers", jamRoom.getMaxUsers());
        jamRoomObject.put("isStatic", jamRoom.isStatic());
        if (includePeersList) {
            jamRoomObject.put("peers", peerListToJsonArray(jamRoom.getPeers()));
        }
        return jamRoomObject;
    }

    public static String getJsonToListPeersInJamRoom(Room jamRoom, List<Peer> activePeers) throws JSONException {
        JSONObject json = new JSONObject();
        json.put("peersOnline", peerListToJsonArray(activePeers));
        json.put("room", jamRoomToJSONObject(jamRoom));
        return json.toString();
    }

}
