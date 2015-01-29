package jninjam.protocol.server.messages;

/**
 * @author elieser
 */
public class ServerKeepAlive extends ServerMessage {

    private static final ServerKeepAlive instance = new ServerKeepAlive();
    
    public static ServerKeepAlive getInstance(){
        return instance;
    }
    
    private ServerKeepAlive() {
        super(ServerMessageType.KEEP_ALIVE);
    }

    @Override
    public String toString() {
        return "ServerKeepAlive{" + '}';
    }

}
