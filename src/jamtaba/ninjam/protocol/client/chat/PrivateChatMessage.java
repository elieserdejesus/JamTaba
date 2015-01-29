package jninjam.protocol.client.chat;

import java.nio.ByteBuffer;

/** @author elieser */
public class PrivateChatMessage extends ChatMessage{

    private String user;
    
    public PrivateChatMessage(String text, String user) {
        super(text);
        this.user = user;
    }

    @Override
    public void serializeTo(ByteBuffer buffer) throws Exception {
        super.serializeTo(buffer); //To change body of generated methods, choose Tools | Templates.
    }

}
