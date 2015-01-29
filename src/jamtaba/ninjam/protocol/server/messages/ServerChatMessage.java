package jninjam.protocol.server.messages;

import java.util.ArrayList;
import java.util.List;

/**
 * @author elieser
 */
public class ServerChatMessage extends ServerMessage {

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

    public static enum ServerChatCommandType {
        MSG("MSG"), PRIVMSG("PRIVMSG"), TOPIC("TOPIC"), JOIN("JOIN"), PART("PART"), USERCOUNT("USERCOUNT");

        private ServerChatCommandType(String command) {
            this.command = command;
        }

        private final String command;

        private static ServerChatCommandType fromString(String string) {
            string = string.trim();
            if (string.equalsIgnoreCase("MSG")) return MSG;
            else if(string.equals("PRIVMSG")) return PRIVMSG;
            else if(string.equals("TOPIC")) return TOPIC;
            else if(string.equals("JOIN")) return JOIN;
            else if(string.equals("PART")) return PART;
            else if(string.equals("USERCOUNT")) return USERCOUNT;
            else throw new IllegalArgumentException("invalid server chat command: " + string);
        }
    }
    //+++++++++++++++++++++++++++++
    
    private final ServerChatCommandType commandType;
    private final List<String> arguments;

    @Override
    public String toString() {
        String result = "ServerChatMessage{" + "command=" + commandType + ", arguments=";// + arguments + '}';
        for (int i = 0; i < arguments.size(); i++) {
            result += arguments.get(i) ;
            if(i < arguments.size()-1){
                result += ", ";
            }
        }
        result += '}';
        return result;
    }
    
    public ServerChatMessage(String command, String arguments[]) {
        super(ServerMessageType.CHAT_MESSAGE);
        this.commandType = ServerChatCommandType.fromString(command);
        this.arguments = new ArrayList<String>();
        for (String arg : arguments) {
            this.arguments.add(arg);
        }
    }

    public List<String> getArguments() {
        return arguments;
    }

    public ServerChatCommandType getCommand() {
        return commandType;
    }

}
