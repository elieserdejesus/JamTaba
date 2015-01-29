package jninjam.protocol.client;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;
import jninjam.NinjaMUser;

/**
 * @author elieser
 */
public class ClientSetUserMask implements ClientMessage {

    private static final Logger LOGGER = Logger.getLogger(ClientSetUserMask.class.getName());

    private Collection<byte[]> usersFullNames;
    private int payload;
    private static final int FLAG = 0xFFFFFFFF;

    public ClientSetUserMask(Collection<NinjaMUser> users) {
        try {
            this.usersFullNames = new ArrayList<byte[]>(users.size());
            int totalPayload = 4 * users.size();//4 bytes (int) flag
            for (NinjaMUser user : users) {
                String name = user.getFullName();
                if (name.charAt(name.length()-1) != '\0') {
                    name = name + '\0';
                }
                byte nameBytes[] = name.getBytes("utf-8");
                usersFullNames.add(nameBytes);
                totalPayload += nameBytes.length;
            }
            payload = totalPayload;
        } catch (UnsupportedEncodingException ex) {
            LOGGER.log(Level.SEVERE, ex.getMessage(), ex);
        }
    }

    @Override
    public void serializeTo(ByteBuffer byteBuffer) throws Exception {
        byteBuffer.put((byte) 0x81);
        byteBuffer.putInt(payload);//payload lenght, computed in constructor
        //++++++++++++  END HEADER ++++++++++++
        for (byte[] userNameBytes : usersFullNames) {
            byteBuffer.put(userNameBytes);
            byteBuffer.putInt(FLAG);
        }
        //return byteBuffer.position();
    }

    @Override
    public String toString() {
        return "ClientSetUserMask{" + "userNames=" + usersFullNames.size() + ", flag=" + FLAG + '}';
    }

}
