package jamtaba.ninjam;

import java.util.HashMap;
import java.util.Map;
//import jninjam.protocol.server.messages.UserChannel;

/**
 * @author elieser
 */
public class NinjaMUser {

    private final String name;
    private final String ip;
    private final String fullName;

    public NinjaMUser(String fullName) {
        this.fullName = fullName;
        String fullNameParts[] = fullName.split("@");
        this.name = fullNameParts[0];
        if (fullNameParts.length > 1) {
            this.ip = fullNameParts[1];
        } else {
            this.ip = "";
        }
    }

    public String getIp() {
        return ip;
    }

    public String getName() {
        return name;
    }

    public String getFullName() {
        return fullName;
    }

   
}
