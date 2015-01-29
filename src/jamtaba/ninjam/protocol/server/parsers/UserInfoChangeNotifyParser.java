package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import jninjam.NinjaMUser;
import jninjam.Utils;
import jninjam.protocol.server.messages.ServerMessage;
import jninjam.protocol.server.messages.UserChannel;
import jninjam.protocol.server.messages.UserInfoChangeNotify;

/**
 * @author elieser
 */
public class UserInfoChangeNotifyParser implements ServerMessageParser {

    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLenght) {
        if (payloadLenght <= 0) {//no users
            return new UserInfoChangeNotify();
        }
        Map<NinjaMUser, List<UserChannel>> allUsersChannels = new HashMap<NinjaMUser, List<UserChannel>>();
        int bufferStart = buffer.position();
        while (buffer.position() - bufferStart < payloadLenght) {
            boolean active = (buffer.get() & 255) > 0;
            byte channelIndex = buffer.get();
            short volume = buffer.getShort();//(dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
            byte pan = buffer.get();//Pan [-128, 127]
            byte flags = buffer.get();
            String userFullName = Utils.extractString(buffer);
            //System.out.println("user fullname: '" + userFullName + "'");
            List<UserChannel> userChannels = allUsersChannels.get(NinjaMUser.getUser(userFullName));
            if (userChannels == null) {
                userChannels = new ArrayList<UserChannel>();
                allUsersChannels.put(NinjaMUser.getUser(userFullName), userChannels);
            }
            String channelName = Utils.extractString(buffer);
            userChannels.add( new UserChannel(NinjaMUser.getUser(userFullName), channelName, active, channelIndex, volume, pan, flags));
        }
        return new UserInfoChangeNotify(allUsersChannels);
    }

}
