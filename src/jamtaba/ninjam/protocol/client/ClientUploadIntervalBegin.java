package jninjam.protocol.client;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
* @author zeh
Offset Type        Field
0x0    uint8_t[16] GUID (binary)
0x10   uint32_t    Estimated Size
0x14   uint8_t[4]  FourCC
0x18   uint8_t     Channel Index
0x19   ...         Username (NUL-terminated)
*/
public class ClientUploadIntervalBegin implements ClientMessage {

    private final byte GUID[];
    private final int estimatedSize;
    private final byte[] fourCC;
    private final byte channelIndex;
    private byte[] userNameBytes;

    
    
    public ClientUploadIntervalBegin(byte[] GUID, byte channelIndex, String userName) {
        this(GUID, channelIndex, userName, "OGGv");
    }
    
    /* this constructor is used only when is necessary abort interval upload. This occurs when user toggle transmit. */
    public ClientUploadIntervalBegin(byte channelIndex, String userName) {
        this( new byte[16], channelIndex, userName, ""+ '\0'+'\0'+'\0'+'\0');//empty fourCC
    }
    
    public ClientUploadIntervalBegin(byte[] GUID, byte channelIndex, String userName, String fourCC) {
        this.GUID = GUID;
        this.estimatedSize = 0;
        this.fourCC = fourCC.getBytes();
        this.channelIndex = channelIndex;
        try {
            this.userNameBytes = (userName + '\0').getBytes("utf-8");
        } catch (UnsupportedEncodingException ex) {
            Logger.getLogger(ClientUploadIntervalBegin.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    @Override
    public void serializeTo(ByteBuffer buffer) throws Exception {
        buffer.put(ClientMessageType.UPLOAD_INTERVAL_BEGIN.typeCode());
        int payload = GUID.length + 4 + 4 + 1 + userNameBytes.length;
        buffer.putInt(payload);
        buffer.put(GUID);
        buffer.putInt(estimatedSize);//estimated size
        buffer.put(fourCC);//four CC
        buffer.put(channelIndex);
        buffer.put(userNameBytes);
        if(buffer.position() != payload + 5){
            throw new IllegalStateException("problema na quantidade de bytes");
        }
    }

}
