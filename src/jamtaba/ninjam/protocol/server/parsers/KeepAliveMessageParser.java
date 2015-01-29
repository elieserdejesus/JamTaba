package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import jninjam.protocol.server.messages.ServerKeepAlive;
import jninjam.protocol.server.messages.ServerMessage;

/** @author elieser */
public class KeepAliveMessageParser implements ServerMessageParser {
    
    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLenght) {
        return ServerKeepAlive.getInstance() ;
    }

    
    
}
