package jninjam.protocol.server.parsers;

import java.nio.ByteBuffer;
import jninjam.protocol.server.messages.ConfigChangeNotify;
import jninjam.protocol.server.messages.ServerMessage;

/** @author elieser */

/*
Offset Type     Field
0x0    uint16_t Beats Per Minute
0x02   uint16_t Beats Per Interval
*/

public class ConfigChangeNotifyParser implements ServerMessageParser{

    @Override
    public ServerMessage parse(ByteBuffer buffer, int payloadLenght) {
        short bpm = buffer.getShort();
        short bpi = buffer.getShort();
        return new ConfigChangeNotify(bpm, bpi);
    }

}
