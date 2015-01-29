package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import jninjam.exception.NinjaMSendException;
import jninjam.protocol.server.messages.DownloadIntervalWrite;
import jninjam.protocol.server.messages.ServerMessage;
import jamtaba.network.profile.measuring.IntegerAccumulatorMeasurer;
import jamtaba.pool.RawByteBufferPool;

/**
 * @author elieser
 */
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint8_t     Flags
 0x11   ...         Audio Data
 */
public class DownloadIntervalWriteParser implements ServerMessageParser {

    //max payload recebido do reaper foi 2076
    //private static final RawByteBufferPool pool = new RawByteBufferPool(6000, 9500);//8500 bytes per byte array

    //private static final byte GUID_TEMP[] = new byte[16];
    
    IntegerAccumulatorMeasurer createdBytes = new IntegerAccumulatorMeasurer(1000);
    
    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLenght) throws NinjaMSendException {
        final byte GUID[] = new byte[16];
        buffer.get(GUID);
        final byte flags = buffer.get();

        int lenght = payloadLenght - 17;

        byte encodedAudioData[] = new byte[lenght];// pool.pickFromPool();
        createdBytes.accumulate(lenght);
        //System.out.println("created bytes/s: " + lenght);
        if(lenght > encodedAudioData.length){
            throw new IllegalStateException("not enough space in byte array. Required space: " + lenght + "  byteArray.lenght: " + encodedAudioData.length);
        }
        buffer.get(encodedAudioData, 0, lenght);
        return new DownloadIntervalWrite(GUID, flags, ByteBuffer.wrap(encodedAudioData, 0, lenght));
    }

}
