package jamtaba.command;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import jamtaba.RealtimeRoom;
import jamtaba.Peer;
import jamtaba.RequestUtils;

public class ConnectInServer extends AbstractCommand {

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
            if(req.getParameter(parameter) == null){
                return false;
            }
        }
        return true;
    }
}
