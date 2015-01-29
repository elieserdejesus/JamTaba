package jninjam.protocol.server.messages;

import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import jninjam.NinjaMUser;

/**
 * @author elieser
 */
public class UserInfoChangeNotify extends ServerMessage {

    private final Map<NinjaMUser, List<UserChannel>> usersChannels;

    @Override
    public String toString() {
        String str = "UserInfoChangeNotify{\n";
        for (NinjaMUser user : usersChannels.keySet()) {
            str += "\t" + user + "\n";
        }
        str += "}";
        return str;
    }

    public Collection<NinjaMUser> getUsers(){
        return usersChannels.keySet();
    }
    
    public UserInfoChangeNotify() {
        this(Collections.EMPTY_MAP);
    }

    public UserInfoChangeNotify(Map<NinjaMUser, List<UserChannel>> allUsersChannels) {
        super(ServerMessageType.USER_INFO_CHANGE_NOTIFY);
        this.usersChannels = allUsersChannels;
    }

    public Map<NinjaMUser, List<UserChannel>> getUsersChannels() {
        return usersChannels;
    }
}
