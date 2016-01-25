package jamtaba.command;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import jamtaba.DbUtils;
import jamtaba.RealtimeRoom;
import jamtaba.Peer;
import jamtaba.PeerCleaner;
import jamtaba.RequestUtils;

public abstract class AbstractCommand implements Command {

    private final Set entitiesToSave = new HashSet();

    private static final Logger LOGGER = Logger.getLogger(AbstractCommand.class.getName());

    public void execute(HttpServletRequest req, HttpServletResponse resp) throws ServletException {
        try {
            Peer peer = getConnectedPeer(req);
            if (peer == null) {
                throw new ServletException(new IllegalStateException("getConnectedPeer return a null value when executing " + this.getClass().getSimpleName()));
            }
            peer.updateLastUpdateTime();
            DbUtils.save(peer);
            PeerCleaner.cleanInactivePeers();
            //++++++++++++++++++++++++++++++++++++++++++++++++
            entitiesToSave.clear();
            //++++++++
            doCommandAction(req, peer);
            //+++++++++++++++
            DbUtils.save(entitiesToSave);

        } catch (Exception e) {
            throw new ServletException(e);
        }
    }

    protected abstract void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException;

    protected void removeConnectedPeerFromCurrentRoom(Peer connectedPeer) {
        if (connectedPeer != null) {
            RealtimeRoom currentRoom = connectedPeer.getRoom();
            if (currentRoom != null) {
                currentRoom.removePeer(connectedPeer.getId());
                connectedPeer.setJamRoom(null);
                if (currentRoom.isEmpty() && !currentRoom.isStatic()) {
                    DbUtils.delete(currentRoom);
                } else {
                    addToSave(currentRoom);
                }
                addToSave(connectedPeer);
            }
        }
    }

    protected void addToSave(Collection c) {
        entitiesToSave.addAll(c);
    }

    protected void addToSave(Object o) {
        entitiesToSave.add(o);
    }

    public Collection getEntitiesToSave() {
        return entitiesToSave;
    }

    public static Peer getConnectedPeer(HttpServletRequest req) {
        Peer peer;
        if (!RequestUtils.peerIsConnected(req)) {
            peer = RequestUtils.buildPeerFromRequest(req);
            LOGGER.log(Level.CONFIG, "Peer {0} not connected, creating peer", peer.getUserName());
        } else {
            peer = RequestUtils.getPeerFromSession(req);
            if (peer == null) {
                boolean peerIDInTheSession = req.getSession().getAttribute(RequestUtils.CONNECTED_PEER_ID) != null;
                LOGGER.log(Level.SEVERE, "The peer is null. Peer ID is in the session:{0} PeerID:{1}", new Object[]{peerIDInTheSession, req.getSession().getAttribute(RequestUtils.CONNECTED_PEER_ID)});
            }
        }
        return peer;
    }

    public boolean saveConnectedPeerIsRequired() {
        return true;
    }

    public boolean saveWaitingRoomIsRequired() {
        return true;
    }

}
