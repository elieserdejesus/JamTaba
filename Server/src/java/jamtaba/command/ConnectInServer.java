package jamtaba.command;

import jamtaba.*;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class ConnectInServer extends AbstractCommand {

    @Override
    public void execute(HttpServletRequest req, HttpServletResponse resp) throws ServletException {
        Peer connectedPeer = null;
        try{
             connectedPeer = getConnectedPeer(req);
        }
        catch(IOException ex){
            throw new ServletException(ex);
        }
        Version clientVersion = Version.fromString(connectedPeer.getVersion());
        Version serverVersion = DbUtils.getCurrentVersion();
        if (clientVersion.isCompatibleWith(serverVersion)) {
            super.execute(req, resp);
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
