package jninjam.protocol.client;

import java.nio.ByteBuffer;

/**
 * @author zeh
 */
public class ClientIntervalUploadWrite implements ClientMessage {

    private final byte[] GUID;
    private final ByteBuffer encodedAudioBuffer;
    private final boolean isLastPart;
    private final int payload;

    public ClientIntervalUploadWrite(byte[] GUID, ByteBuffer encodedAudioBuffer, boolean isLastPart) {
        this.GUID = GUID;
        this.encodedAudioBuffer = ByteBuffer.allocate(encodedAudioBuffer.remaining());
        this.encodedAudioBuffer.put(encodedAudioBuffer.duplicate());// encodedAudioBuffer;
        this.encodedAudioBuffer.flip();
        
        this.isLastPart = isLastPart;
        this.payload = 16 + 1 + this.encodedAudioBuffer.remaining();
        if (encodedAudioBuffer == null) {
            throw new IllegalArgumentException("encodedAudioBuffer is null");
        }
    }

    @Override
    public synchronized void serializeTo(ByteBuffer buffer) throws Exception {
        buffer.put(ClientMessageType.UPLOAD_INTERVAL_WRITE.typeCode());
        buffer.putInt(payload);//payload
        buffer.put(GUID);
        buffer.put(isLastPart ? (byte) 1 : (byte) 0);//If the Flag field bit 0 is set then the upload is complete.
        buffer.put(encodedAudioBuffer);
        //System.out.println("serialized: " + new String(GUID));
    }

    @Override
    public String toString() {
        return "ClientIntervalUploadWrite{" + "GUID=" + new String(GUID) + ", encodedAudioBuffer= " + payload + " bytes, isLastPart=" + isLastPart + '}';
    }

}
