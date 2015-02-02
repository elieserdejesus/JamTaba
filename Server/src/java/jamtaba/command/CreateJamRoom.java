package jamtaba.command;

import jamtaba.DbUtils;
import jamtaba.Peer;
import jamtaba.RequestUtils;
import jamtaba.RealtimeRoom;
import java.util.Collections;
import java.util.List;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;


public class CreateJamRoom extends AbstractCommand {

    private static int japaneseRooms = 0;

    @Override
    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {
        String roomName = req.getParameter("roomName");
        //check for kanji
        if(JapaneseUtils.containJapaneseSymbols(roomName)){
            roomName = "Japanese Room " + (++japaneseRooms);
        }
        //Integer roomStyleCode = Integer.parseInt(req.getParameter("roomStyleCode"));
        int maxUsers = Integer.parseInt(req.getParameter("maxUsers"));
        List<Peer> peers = Collections.EMPTY_LIST;
        RealtimeRoom room = new RealtimeRoom(roomName, maxUsers, peers, false);
        DbUtils.save(room);
        ConnectInJamRoom connectInJamRoomCommand = new ConnectInJamRoom(room.getId());
        connectInJamRoomCommand.doCommandAction(req, connectedPeer);
        addToSave(connectInJamRoomCommand.getEntitiesToSave());
    }

    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        boolean requireds[] = {
            RequestUtils.peerIsConnected(req),
            req.getParameter("roomName") != null,
            req.getParameter("maxUsers") != null
        };
        for (boolean b : requireds) {
            if (!b) {
                return false;//se um dos parametros não estiver no request retorna false
            }
        }
        return true;
    }
}
