package jamtaba.command;

import java.io.IOException;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import jamtaba.DbUtils;
import jamtaba.Room;
import jamtaba.Peer;
import jamtaba.PeerCleaner;
import jamtaba.RequestUtils;
import jamtaba.VsJsonUtils;

public abstract class AbstractCommand implements Command {

    private final Set entitiesToSave = new HashSet();
    
    private static final Logger LOGGEr = Logger.getLogger(AbstractCommand.class.getName());

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
            Collection<Room> jamRooms = DbUtils.loadRooms();
            //++++++++++++++++++
            resp.getWriter().print(VsJsonUtils.getJsonToResponse(jamRooms, peer));

        } catch (Exception e) {
            throw new ServletException(e);
        }
    }

    /**
     *
     * @param req
     * @param connectedPeer
     * @param rooms
     * @return A iterable of objects to persist
     * @throws ServletException
     */
    protected abstract void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException;

    protected void removeConnectedPeerFromCurrentRoom(Peer connectedPeer) {
        if (connectedPeer != null) {
            Room currentRoom = connectedPeer.getRoom();
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

    protected Peer getConnectedPeer(HttpServletRequest req) throws IOException {
        Peer peer;
        if (!RequestUtils.peerIsConnected(req)) {
            peer = RequestUtils.buildPeerFromRequest(req);
//            if (peer.getId() == null) {
//                DbUtils.save(peer);
//            }
        } else {
            
            peer = RequestUtils.getPeerFromSession(req);
            if(peer == null){
                LOGGEr.log(Level.SEVERE, "O peer criado a partir da sessão estava nulo! ID do peer na sessão: {0}", req.getSession(false).getAttribute(RequestUtils.CONNECTED_PEER_ID));
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
