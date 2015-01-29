package jninjam.protocol.server.messages;

import jninjam.Utils;

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
 If the GUID field is zero then the download should be stopped.

 If the FourCC field is zero then the download is complete.

 If the FourCC field contains "OGGv" then this is a valid Ogg Vorbis encoded download.
 */
public class DownloadIntervalBegin extends ServerMessage {

    private final String GUID;
    private final int estimatedSize;
    private final byte fourCC[];// = new byte[4];
    private final byte channelIndex;
    private final String userName;

    public DownloadIntervalBegin(int estimatedSize, byte channelIndex, String userName, byte fourCC[], byte GUID[]) {
        super(ServerMessageType.DOWNLOAD_INTERVAL_BEGIN);
        this.estimatedSize = estimatedSize;
        this.channelIndex = channelIndex;
        this.userName = userName;
        this.fourCC = fourCC;
        this.GUID = new String(GUID);// Utils.bytesToHexString(GUID);
    }

    
    
    public byte getChannelIndex() {
        return channelIndex;
    }

    public int getEstimatedSize() {
        return estimatedSize;
    }

    public String getGUID() {
        return GUID;
    }

    @Override
    public String toString() {
        return "DownloadIntervalBegin{ \n "
                + "\tfourCC='"+ new String(fourCC) +"'\n "
                + "\tGUID={"+ GUID+"}\n "
                + "\tisValidOggDownload="+ isValidOggDownload() + "\n "
                + "\tdownloadShoudBeStopped="+ downloadShouldBeStopped()  +"\n "
                + "\tdownloadIsComplete="+ downloadIsComplete() +"\n "
                + "\testimatedSize=" + estimatedSize + "\n "
                + "\tchannelIndex=" + channelIndex + "\n "
                + "\tuserName=" + userName + "\n}\n";
    }

    public String getUserName() {
        return userName;
    }
    
    public boolean isValidOggDownload(){
        return "OGGv".equals(new String(fourCC));
    }
    
    public boolean downloadShouldBeStopped(){
        return   GUID.charAt(0) == '0' && GUID.charAt(GUID.length()-1) == '0';
    }
    
    public boolean downloadIsComplete(){
        return ((fourCC[0] & 255) == 0 && fourCC[3] == 0);// || downloadShouldBeStopped();
    }
    
}
