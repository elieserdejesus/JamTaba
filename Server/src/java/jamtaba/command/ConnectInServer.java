package jamtaba.command;

import jamtaba.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class ConnectInServer extends AbstractCommand {

    private static final Logger LOGGER = Logger.getLogger(ConnectInServer.class.getName());

    @Override
    public void execute(HttpServletRequest req, HttpServletResponse resp) throws ServletException {
        LOGGER.log(Level.INFO, "Connecting in server");
        Peer connectedPeer = getConnectedPeer(req);
        if (connectedPeer != null) {
            Version clientVersion = Version.fromString(connectedPeer.getVersion());
            Version serverVersion = DbUtils.getCurrentVersion();
            if (clientVersion.isCompatibleWith(serverVersion)) {
                super.execute(req, resp);
            }
        } else {
            throw new ServletException("Connected peer is null");
        }
    }

    @Override
    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {
        RequestUtils.putConnectedPeerIDInSession(req, connectedPeer);
        ConnectInJamRoom connectInWaitingRoomCommand = new ConnectInJamRoom(RealtimeRoom.WAITING_ROOM_ID);
        connectInWaitingRoomCommand.doCommandAction(req, connectedPeer);
        addToSave(connectInWaitingRoomCommand.getEntitiesToSave());
    }

    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        String requiredParameters[] = {"publicIp", "publicPort", "userName", "version", "sampleRate", "environment"};
        for (String parameter : requiredParameters) {
            if (req.getParameter(parameter) == null) {
                return false;
            }
        }
        return true;
    }
}
