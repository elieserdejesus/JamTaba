/*
 Este comando enviado pelos peers periodicamente. O servidor registra o momento em que a ultima
 requisição chegou. Se o peer ficar um tempo sem atualizar o seu lastUpdateTime o servidor vai
 eliminar o peer por inatividade. Isso resolve o problema dos peers 'fantasmas' que poderiam aparecer
 caso a desconexão do servidor falhe.
 */
package jamtaba.command;

import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import jamtaba.Peer;
import jamtaba.RequestUtils;

/**
 *
 * @author elieser
 */
public class RefreshCommand extends AbstractCommand {

    public static final int MAX_TIME_WITHOUT_UPDATES = 1 * 60 * 1000;//depois de 1 minuto sem se comunicar com o servidor o peer é considerado desconectado

    private static final Logger LOGGER = Logger.getLogger(RefreshCommand.class.getName());

    @Override
    protected void doCommandAction(HttpServletRequest req, Peer connectedPeer) throws ServletException {

    }

    public boolean requiredDataIsAvailable(HttpServletRequest req) {
        return RequestUtils.peerIsConnected(req);
    }

}
