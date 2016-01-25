package jamtaba;

import com.googlecode.objectify.Key;
import javax.servlet.http.HttpServletRequest;
import static com.googlecode.objectify.ObjectifyService.ofy;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.http.HttpSession;

public class RequestUtils {

    public static final String CONNECTED_PEER_ID = "connectedPeerID";
    private static final Logger LOGGER = Logger.getLogger(RequestUtils.class.getName());

    public static void putConnectedPeerIDInSession(HttpServletRequest req, Peer peer) {
        if (peer.getId() == null) {
            DbUtils.save(peer);
            LOGGER.log(Level.INFO, "Saving connected peer PeerID:{0}", peer.getId());
        }
        req.getSession().setAttribute(CONNECTED_PEER_ID, peer.getId());
        LOGGER.log(Level.INFO, "Putting connected peer in the session PeerID:{0}", peer.getId());
    }

    public static void clearSession(HttpServletRequest req) {
        HttpSession session = req.getSession(false);
        session.removeAttribute(CONNECTED_PEER_ID);
        session.setAttribute(CONNECTED_PEER_ID, null);
        session.invalidate();
    }

    public static Peer getPeerFromSession(HttpServletRequest req) {
        Long ID = (Long) req.getSession().getAttribute(CONNECTED_PEER_ID);
        return ofy().load().key(Key.create(Peer.class, ID)).now();//return null if not found
    }

    public static long getPeerTimeStamp(HttpServletRequest req) {
        if (req.getParameter("peerTimeStamp") != null) {
            return Long.parseLong(req.getParameter("peerTimeStamp"));
        }
        return 0L;
    }

    public static boolean peerIsConnected(HttpServletRequest req) {
        return req.getSession().getAttribute(CONNECTED_PEER_ID) != null;
    }

    public static Peer buildPeerFromRequest(HttpServletRequest req) {
        String userName = req.getParameter("userName");
        String publicIp = req.getParameter("publicIp");
        String publicPort = req.getParameter("publicPort");

        String country = req.getHeader("X-AppEngine-Country");
        String region = req.getHeader("X-AppEngine-Region");
        int instrumentID = (req.getParameter("instrumentID") != null) ? Integer.parseInt(req.getParameter("instrumentID")) : 0;
        String channelName = (req.getParameter("channelName") != null) ? req.getParameter("channelName") : "";
        String environment = req.getParameter("environment");
        String version = req.getParameter("version");
        //int bufferSize = Integer.parseInt(req.getParameter("bufferSize"));
        int sampleRate = Integer.parseInt(req.getParameter("sampleRate"));
        //String city = req.getHeader("X-AppEngine-City");

        return new Peer(publicIp, publicPort, userName, region, country, instrumentID, channelName, version, sampleRate, environment);
    }

    public static boolean requestContainJamRoomID(HttpServletRequest req) {
        return req.getParameter("jamRoomID") != null;
    }

    public static Long getJamRoomIdFromRequest(HttpServletRequest req) {
        return Long.parseLong(req.getParameter("jamRoomID"));
    }

//    public static Long getPeerIDFromRequest(HttpServletRequest req){
//        return Long.parseLong(req.getParameter("id"));
//    }
}
