/*
 comando enviado para o servidor sempre que um usuário entra em uma sala do ninjam, assim ele não é mais exibido na sala de espera
 */
package jamtaba.command;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import jamtaba.Peer;
import jamtaba.RequestUtils;

/**
  * @author elieser
 */
public class SetPlayingInNinjam extends AbstractCommand {

    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {
        boolean playingInNinjam = Boolean.parseBoolean(req.getParameter("playingInNinjam"));
        connectedPeer.setPlayingInNinjam(playingInNinjam);
        addToSave(connectedPeer);
    }

    @Override
    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        return RequestUtils.peerIsConnected(req) && req.getParameter("playingInNinjam") != null;
    }

}
