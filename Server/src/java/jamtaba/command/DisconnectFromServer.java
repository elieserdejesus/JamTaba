package jamtaba.command;

import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import jamtaba.DbUtils;
import jamtaba.Peer;
import jamtaba.RequestUtils;

public class DisconnectFromServer extends AbstractCommand {

    private static final Logger LOGGER = Logger.getLogger(DisconnectFromServer.class.getName());

    @Override
    public void execute(HttpServletRequest req, HttpServletResponse resp) throws ServletException {
        try {
            Peer connectedPeer = getConnectedPeer(req);
            if (connectedPeer != null) {
                removeConnectedPeerFromCurrentRoom(connectedPeer);
                DbUtils.save(getEntitiesToSave());
                DbUtils.delete(connectedPeer);
            } else {
                throw new IllegalStateException("getConnectedPeer return a null value when executing " + this.getClass().getSimpleName());
            }
        } catch (Exception e) {
            throw new ServletException(e);
        } finally {
            RequestUtils.clearSession(req);
        }
    }

    @Override
    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {

    }

    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        boolean peerIsConnected = RequestUtils.peerIsConnected(req);
        //LOGGER.log(Level.INFO, "peer na sessão: {0}", peerIsConnected);
        return peerIsConnected;
    }
}
