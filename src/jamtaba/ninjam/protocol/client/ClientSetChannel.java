package jninjam.protocol.client;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * @author elieser
/*
 Offset Type     Field
 0x0    uint16_t Channel Parameter Size (bytes)
 0x2    ...      Channel Name (NUL-terminated)
 x+0x0  int16_t  Volume (dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
 x+0x2  int8_t   Pan [-128, 127]
 x+0x3  uint8_t  Flags
 x+0x4  ...      Zero Padding (up to Channel Parameter Size)

 Each channel has its parameters added to the message so there may be zero or more channels described in one message. 
 If there are no channels then the payload is empty and zero length.

 */
public class ClientSetChannel implements ClientMessage {

    private static final Logger LOGGER = Logger.getLogger(ClientSetChannel.class.getName());

    private static final byte messageType = (byte) 0x82;

    @Override
    public String toString() {
        return "ClientSetChannel{" + "payloadLenght=" + payloadLenght + ", channelName=" + channelNames + '}';
    }
    private int payloadLenght;
    private String channelNames[] = {"channel" + '\0'};//, "Canal 2" + '\0'};
    private final short volume = (short) 0;
    private final byte pan = 0;
    private final byte flags = 0;//????

    public ClientSetChannel(String channelName) {
        channelNames[0] = channelName + '\0';
        this.payloadLenght = 2;//parameters size (short)
        try {
            for (int i = 0; i < channelNames.length; i++) {
                byte channelNameBytes[] = channelNames[i].getBytes("utf-8");
                payloadLenght += (channelNameBytes.length + 2 + 1 + 1);//volume + pan + flags
            }
        } catch (UnsupportedEncodingException e) {
            LOGGER.log(Level.SEVERE, e.getMessage(), e);
        }

    }

    //++++++++++++
    @Override
    public void serializeTo(ByteBuffer byteBuffer) throws Exception {
        byteBuffer.put(messageType);
        byteBuffer.putInt(payloadLenght);
        //++++++++
        byteBuffer.putShort((short) 4);//parameter size (4 bytes - volume (2 bytes) + pan (1 byte) + flags (1 byte))
        for (int i = 0; i < channelNames.length; i++) {
            byteBuffer.put(channelNames[i].getBytes("utf-8"));
            byteBuffer.putShort(volume);
            byteBuffer.put(pan);
            byteBuffer.put(flags);
        }
        if(byteBuffer.position() != payloadLenght + 5){
            throw new IllegalStateException("wrong buffer position!");
        }
    }

}
