package jamtaba.ninjam;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author zeh
 */
public class NinjamServer {

    private final int port;
    private final String host;
    private final int maxUsers;
    private final int bpm;
    private final short bpi;
    //private final boolean isActive;
    private final String streamUrl;

    private final Map<String, NinjamUser> users = new HashMap<String, NinjamUser>();

    
    public NinjamServer(String host, int port, int maxUsers, int bpm, short bpi, String streamUrl) {
        this.port = port;
        this.host = host;
        this.streamUrl = streamUrl;
        this.maxUsers = maxUsers;
        this.bpm = bpm;
        this.bpi = bpi;
    }

    public String getStreamUrl() {
        return streamUrl;
    }

    public void addUser(NinjamUser user) {
        if (!users.containsKey(user.getFullName())) {
            users.put(user.getFullName(), user);
        }
    }

    public short getBpi() {
        return bpi;
    }

    public int getBpm() {
        return bpm;
    }

    public int getMaxUsers() {
        return maxUsers;
    }

    public List<NinjamUser> getUsers() {
        return new ArrayList(users.values());
    }

    public int getPort() {
        return port;
    }

    public String getHostName() {
        return host;
    }

    private static String getUniqueName(String host, int port) {
        return host + ":" + port;
    }

    public String getUniqueName() {
        return getUniqueName(host, port);
    }


}
