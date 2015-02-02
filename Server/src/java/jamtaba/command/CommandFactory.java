package jamtaba.command;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import jamtaba.RequestUtils;
import jamtaba.RealtimeRoom;

/**
 *
 * @author Elieser
 */
public class CommandFactory {

    public static Command getCommand(HttpServletRequest req) {
        String cmd = req.getParameter("cmd");
        boolean isJamRoomCommand = RequestUtils.requestContainJamRoomID(req);
        if ("KEEP_ALIVE".equals(cmd)) {
            return new RefreshCommand();
        }
        if ("CONNECT".equals(cmd)) {
            if (isJamRoomCommand) {
                return new ConnectInJamRoom();
            }
            return new ConnectInServer();
        }
        if ("DISCONNECT".equals(cmd)) {
            if (isJamRoomCommand) {
                return new ConnectInJamRoom(RealtimeRoom.WAITING_ROOM_ID);//back to waiting room
            }
            return new DisconnectFromServer();
        }
        if ("NEW".equals(cmd)) {
            return new CreateJamRoom();
        }
        if ("UPDATE".equals(cmd)) {
            if(req.getParameter("instrumentID") != null ){//updating instrument ID
                return new ChangeInstrumentID();
            }if(req.getParameter("channelName") != null){//updating channel name
                return new ChangeChannelName();
            }
            else{//updating ninjam playing status
                return new SetPlayingInNinjam();
            }
        }

        return new InvalidCommand(cmd);
    }

    private static class InvalidCommand implements Command {

        private String cmd;

        public InvalidCommand(String cmd) {
            this.cmd = cmd;
        }

        public void execute(HttpServletRequest req, HttpServletResponse resp) throws ServletException {
            throw new ServletException("Comando inválido - " + cmd);
        }

        public boolean requiredDataIsAvailable(HttpServletRequest req) {
            return true;
        }

    }
}
