package jninjam.protocol.server.messages;

/** @author elieser */
public class ServerAuthReply extends ServerMessage{

    private final byte flag;
    private final String message;
    private final byte maxChannels;

    public ServerAuthReply(byte flag, byte maxChannels, String reponseMessage) {
        super(ServerMessageType.AUTH_REPLY);
        this.flag = flag;
        this.message = reponseMessage;
        this.maxChannels = maxChannels;
    }

    @Override
    public String toString() {
        return "ServerAuthReply{" + "flag=" + flag + ", errorMessage='" + message + "', maxChannels=" + maxChannels + '}';
    }

    public String getErrorMessage() {
        return message;
    }
    
    public String getNewUserName(){
        if(!userIsAuthenticated()){
            throw new IllegalStateException("user is note authenticated!");
        }
        return message;
    }

    public boolean userIsAuthenticated() {
        return flag == 1;
    }

    public byte getMaxChannels() {
        return maxChannels;
    }
    
}
