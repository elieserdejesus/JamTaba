package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import jninjam.Utils;
import jninjam.protocol.server.messages.ServerAuthReply;
import jninjam.protocol.server.messages.ServerMessage;

/**
 * @author elieser
 */
public class AuthReplyParser implements ServerMessageParser {
   
    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLenght) {
        byte flag = buffer.get();
        String serverMessage = Utils.extractString(buffer);// new String(errorMessageBytes);
        byte maxChannels = buffer.get();
        return new ServerAuthReply(flag, maxChannels, serverMessage);
    }

}
