package jninjam.protocol.client.chat;

import java.nio.ByteBuffer;
import jninjam.protocol.client.ClientMessage;

/** @author elieser */

/*
The client-to-server commands are:

MSG <text> -- broadcasts a message
PRIVMSG <username> <text> -- sends a message to a user
TOPIC <text> -- sets the server topic (requires permissions)
ADMIN topic|kick|bpm|bpi <value> -- administrator commands
A client-to-server MSG may be !vote bpm|bpi <value>. This allows users to change the bpm and bpi settings.
*/

public class ChatMessage implements ClientMessage {

    protected String text;
    protected ClientChatCommandType commandType;
    
    public ChatMessage(String text) {
        this.text = text;
        this.commandType = ClientChatCommandType.MSG;
    }

    @Override
    public void serializeTo(ByteBuffer byteBuffer) throws Exception {
        byteBuffer.put((byte)0xc0);
        byte commandBytes[] = (commandType.commandName + '\0').getBytes("utf-8");
        byte textBytes[] = (text + '\0').getBytes("utf-8");
        int payloadLenght = commandBytes.length + textBytes.length;
        byteBuffer.putInt(payloadLenght);
        //++++++++++++++=
        byteBuffer.put(commandBytes);
        byteBuffer.put(textBytes);
        //System.out.println(new String(commandBytes));
        //System.out.println(new String(textBytes));
        //return byteBuffer.position();
    }
    
    //+++++++++
    public static enum ClientChatCommandType {
        MSG("MSG"), PRIVMSG("PRIVMSG"), TOPIC("TOPIC"), ADMIN("ADMIN");

        private ClientChatCommandType(String command) {
            this.commandName = command;
        }

        private final String commandName;

        private static ClientChatCommandType fromString(String string) {
            string = string.trim();
            if (string.equalsIgnoreCase("MSG")) return MSG;
            else if(string.equals("PRIVMSG")) return PRIVMSG;
            else if(string.equals("TOPIC")) return TOPIC;
            else if(string.equals("ADMIN")) return ADMIN;
            else throw new IllegalArgumentException("invalid client chat command: " + string);
        }
    }

}
