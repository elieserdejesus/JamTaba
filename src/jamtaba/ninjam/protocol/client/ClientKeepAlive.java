package jninjam.protocol.client;

import java.nio.ByteBuffer;

/** @author elieser */
public class ClientKeepAlive implements ClientMessage{

    @Override
    public void serializeTo(ByteBuffer byteBuffer) throws Exception {
        //just the header bytes, no payload
        byteBuffer.put(ClientMessageType.KEEP_ALIVE.typeCode());
        byteBuffer.putInt(0);//payload lenght
        //return 5;
    }

    @Override
    public String toString() {
        return "ClientKeepAlive{" + '}';
    }

}
