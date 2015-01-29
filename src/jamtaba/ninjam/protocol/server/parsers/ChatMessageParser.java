package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import java.util.logging.Logger;
import jninjam.Utils;
import jninjam.protocol.server.messages.ServerChatMessage;
import jninjam.protocol.server.messages.ServerMessage;

/**
 * @author elieser
 */

/*
 Offset Type Field
 0x0    ...  Command (NUL-terminated)
 a+0x0  ...  Argument 1 (NUL-terminated)
 b+0x0  ...  Argument 2 (NUL-terminated)
 c+0x0  ...  Argument 3 (NUL-terminated)
 d+0x0  ...  Argument 4 (NUL-terminated)
    
 The server-to-client commands are:
 MSG <username> <text> -- a broadcast message
 PRIVMSG <username> <text> -- a private message
 TOPIC <username> <text> -- server topic change
 JOIN <username> -- user enters server
 PART <username> -- user leaves server
 USERCOUNT <users> <maxusers> -- server status
 */
public class ChatMessageParser implements ServerMessageParser {

    private static final Logger LOGGER = Logger.getLogger(ChatMessageParser.class.getName());

    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLengt) {
        int startIndex = buffer.position();
        String command = Utils.extractString(buffer);
        String arguments[] = new String[4];
        int parsedArgs = 0;
        while(buffer.position() - startIndex < payloadLengt && parsedArgs < 4){
            arguments[parsedArgs++] = Utils.extractString(buffer);
        }
        ServerChatMessage chatMessage = new ServerChatMessage(command, arguments);
        return chatMessage;
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}
