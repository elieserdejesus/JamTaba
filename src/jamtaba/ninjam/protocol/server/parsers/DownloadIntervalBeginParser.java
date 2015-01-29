package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import jninjam.Utils;
import jninjam.exception.NinjaMSendException;
import jninjam.protocol.server.messages.DownloadIntervalBegin;
import jninjam.protocol.server.messages.ServerMessage;

/**
 * @author elieser
 */
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint32_t    Estimated Size
 0x14   uint8_t[4]  FourCC
 0x18   uint8_t     Channel Index
 0x19   ...         Username (NUL-terminated)
 */
public class DownloadIntervalBeginParser implements ServerMessageParser {

    //private static final byte GUID_TEMP[] = new byte[16];
    
    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLenght) throws NinjaMSendException {
        final byte GUID[] = new byte[16];
        buffer.get(GUID); 
        final int estimatedSize = buffer.getInt();
        //System.out.println("estimated size: " + estimatedSize);
        final byte fourCC[] = new byte[4];
        buffer.get(fourCC);
        //System.out.println("fourCC: " + new String(fourCC));
        final byte channelIndex = buffer.get();
        final String userName = Utils.extractString(buffer);
        return new DownloadIntervalBegin(estimatedSize, channelIndex, userName, fourCC, GUID);
    }

}
