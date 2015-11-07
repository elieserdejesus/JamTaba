package jamtaba;

import com.google.appengine.labs.repackaged.org.json.JSONArray;
import com.google.appengine.labs.repackaged.org.json.JSONException;
import com.google.appengine.labs.repackaged.org.json.JSONObject;
import jamtaba.ninjam.NinjamServer;
import jamtaba.ninjam.NinjamUser;
import java.util.Collection;
import java.util.List;
import java.util.logging.Logger;

public class VsJsonUtils {

    private static final Logger LOGGER = Logger.getLogger(VsJsonUtils.class.getName());

//    public static String getJsonToConnectionInRoom(RealtimeRoom connectedRoom) throws JSONException {
//        JSONObject json = new JSONObject();
//        //json.put("peer", peerAsJSONObject(connectedPeer));
//        //json.put("room", jamRoomToJSONObject(connectedRoom));
//        return json.toString();// toJSONString();
//    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
    public static String getJsonToResponse(Collection<RealtimeRoom> realtimeRooms, Collection<NinjamServer> ninjamServers, Peer connectedPeer) throws JSONException {
        JSONObject json = new JSONObject();
        
        JSONArray jamRoomsArray = new JSONArray();
        for (RealtimeRoom jamRoom : realtimeRooms) {
            jamRoomsArray.put(jamRoomToJSONObject(jamRoom));
        }
        for (NinjamServer ninjamServer : ninjamServers) {
            jamRoomsArray.put(ninjamServerToJSONObject(ninjamServer));
        }
        json.put("rooms", jamRoomsArray);
        
        if (connectedPeer != null) {
            json.put("peer", peerAsJSONObject(connectedPeer));
        }
        
        json.put("clientCompatibility", clientVersionIsCompatible(connectedPeer) );
        json.put("newVersionAvailable", newVersionIsAvailable(connectedPeer));
        return json.toString();
    }

    private static boolean newVersionIsAvailable(Peer connectedPeer){
        Version clientVersion = Version.fromString(connectedPeer.getVersion());
        Version serverVersion = DbUtils.getCurrentVersion();
        return serverVersion.isNewer(clientVersion);
    }
    
    private static boolean clientVersionIsCompatible(Peer connectedPeer){
        Version clientVersion = Version.fromString(connectedPeer.getVersion());
        Version serverVersion = DbUtils.getCurrentVersion();
        return clientVersion.isCompatibleWith(serverVersion);
    }
    
    public static String getJsonToResponse(List<RealtimeRoom> jamRooms, Collection<NinjamServer> ninjamServers) throws JSONException {
        return getJsonToResponse(jamRooms, ninjamServers, null);
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=
    private static JSONArray peerListToJsonArray(List<Peer> peers) throws JSONException {
        JSONArray array = new JSONArray();
        for (Peer peer : peers) {
            if (peer != null) {
                array.put(peerAsJSONObject(peer));
            }
        }
        return array;
    }

//    public static String getJsonToJamRoomsList(List<RealtimeRoom> jamRooms) throws JSONException {
//        JSONObject json = new JSONObject();
//        json.put("rooms", realTimeRoomsToJsonArray(jamRooms));
//        return json.toString();
//    }

    public static String getJsonToJamRoom(RealtimeRoom jamRoom) throws JSONException {
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

        RealtimeRoom peerRoom = peer.getRoom();
        if (peerRoom != null) {
            peerMap.put("room", jamRoomToJSONObject(peerRoom, false));
        }
        return peerMap;
    }

    private static JSONObject ninjamServerToJSONObject(NinjamServer server) throws JSONException {
        JSONObject serverObject = new JSONObject();
        serverObject.put("id", server.getUniqueName().hashCode());
        serverObject.put("type", "ninjam");
        serverObject.put("name", server.getHostName());
        serverObject.put("port", server.getPort());
        serverObject.put("maxUsers", server.getMaxUsers());
        serverObject.put("bpm", server.getBpm());
        serverObject.put("bpi", server.getBpi());
        serverObject.put("streamUrl", server.getStreamUrl());
        //users
        JSONArray usersArray = new JSONArray();
        for (NinjamUser user : server.getUsers()) {
            JSONObject userObject = new JSONObject();
            userObject.put("id", user.getFullName().hashCode());
            userObject.put("name", user.getName());
            userObject.put("ip", user.getIp());
            userObject.put("countryCode", user.getCountryCode());
            usersArray.put(userObject);
        }
        serverObject.put("users", usersArray);
        return serverObject;
    }
    
    private static JSONObject jamRoomToJSONObject(RealtimeRoom jamRoom) throws JSONException {
        return jamRoomToJSONObject(jamRoom, true);
    }

    private static JSONObject jamRoomToJSONObject(RealtimeRoom jamRoom, boolean includePeersList) throws JSONException {
        JSONObject jamRoomObject = new JSONObject();

        jamRoomObject.put("id", jamRoom.getId());
        jamRoomObject.put("name", jamRoom.getName());
        jamRoomObject.put("type", "realtime");
        //jamRoomObject.put("styleCode", jamRoom.getStyleCode());
        jamRoomObject.put("maxUsers", jamRoom.getMaxUsers());
        jamRoomObject.put("isStatic", jamRoom.isStatic());
        if (includePeersList) {
            jamRoomObject.put("peers", peerListToJsonArray(jamRoom.getPeers()));
        }
        return jamRoomObject;
    }

    public static String getJsonToListPeersInJamRoom(RealtimeRoom jamRoom, List<Peer> activePeers) throws JSONException {
        JSONObject json = new JSONObject();
        json.put("peersOnline", peerListToJsonArray(activePeers));
        json.put("room", jamRoomToJSONObject(jamRoom));
        return json.toString();
    }

}
