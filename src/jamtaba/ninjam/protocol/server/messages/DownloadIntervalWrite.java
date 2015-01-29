package jninjam.protocol.server.messages;

import java.nio.ByteBuffer;
import jninjam.Utils;

/**
 * @author elieser
 */
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint8_t     Flags
 0x11   ...         Audio Data
 If the Flags field has bit 0 set then this download should be aborted.
 */
public class DownloadIntervalWrite extends ServerMessage {

    private final String GUID;
    private final byte flags;
    private final ByteBuffer encodedAudioData;

    //private static DownloadIntervalWrite instance;
    @Override
    public String toString() {
        return "DownloadIntervalWrite{ flags=" + flags + " GUID={" + GUID + "} downloadIsComplete=" + downloadIsComplete() + ", audioData=" + encodedAudioData.limit() + " bytes }";
    }

    public DownloadIntervalWrite(byte GUID[], byte flags, ByteBuffer encodedAudioData) {
        super(ServerMessageType.DOWNLOAD_INTERVAL_WRITE);
        this.GUID = new String(GUID);// Utils.bytesToHexString(GUID);
        this.encodedAudioData = encodedAudioData;
        this.flags = flags;
    }

    public String getGUID() {
        return GUID;
    }

    public ByteBuffer getEncodedAudioData() {
        return encodedAudioData;
    }

    public boolean downloadIsComplete() {
        return flags == 1;
    }

}
