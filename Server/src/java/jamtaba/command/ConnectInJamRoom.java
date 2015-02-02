package jamtaba.command;

import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import jamtaba.DbUtils;
import jamtaba.RealtimeRoom;
import jamtaba.Peer;
import jamtaba.RequestUtils;

public class ConnectInJamRoom extends AbstractCommand {

    private static final Logger LOGGER = Logger.getLogger(ConnectInJamRoom.class.getName());

    private Long roomID;

    public ConnectInJamRoom() {
        roomID = null;
    }

    public ConnectInJamRoom(Long roomID) {
        this.roomID = roomID;
    }

    @Override
    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {
        if (this.roomID == null) {
            this.roomID = RequestUtils.getJamRoomIdFromRequest(req);
        }
        synchronized (this) {
            //leave current room
            removeConnectedPeerFromCurrentRoom(connectedPeer);

            //connect in new room
            RealtimeRoom jamRoomToConnect = DbUtils.getRoomByID(roomID);
            if (jamRoomToConnect != null && !jamRoomToConnect.isFull()) {
                boolean connectedInRoom = connectedPeer.getRoom() != null;
                if(connectedInRoom && connectedPeer.getRoom().getId().equals(roomID)){
                    return;//avoid duplicated peers
                }
                jamRoomToConnect.addPeer(connectedPeer);
                connectedPeer.setPlayingInNinjam(false);//só para garantir caso o client não set isso adequadamente
                addToSave(jamRoomToConnect);
                addToSave(connectedPeer);
            } else {
                LOGGER.severe("sala cheia!");
                throw new ServletException("Sala cheia ou inexistente!");
            }
        }
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        return RequestUtils.peerIsConnected(req);
    }

}
