/*
 comando enviado para o servidor sempre que um usuário modifica o ícone do seu instrumento
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
public class ChangeInstrumentID extends AbstractCommand {

    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {
        int instrumentID = Integer.parseInt(req.getParameter("instrumentID"));
        connectedPeer.setInstrumentID(instrumentID);
        addToSave(connectedPeer);
    }

    @Override
    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        return RequestUtils.peerIsConnected(req) && req.getParameter("instrumentID") != null;
    }

}
