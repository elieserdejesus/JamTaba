/*
 comando enviado para o servidor sempre que um usuário modifica o nome do canal local
 */
package jamtaba.command;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import jamtaba.Peer;
import jamtaba.RequestUtils;

/**
 *
 * @author elieser
 */
public class ChangeChannelName extends AbstractCommand {

    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {
        String channelName = req.getParameter("channelName");
        connectedPeer.setChannelName(channelName);
        addToSave(connectedPeer);
    }

    @Override
    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        return RequestUtils.peerIsConnected(req) && req.getParameter("channelName") != null;
    }

}
