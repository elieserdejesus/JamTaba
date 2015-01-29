package jninjam.protocol.server.messages;

/** @author elieser */
/*
Informs clients that bpm/bpi have changed. The payload layout is:

Offset Type     Field
0x0    uint16_t Beats Per Minute
0x02   uint16_t Beats Per Interval
Changing bpm/bpi takes effect on the next interval.
*/

public class ConfigChangeNotify extends ServerMessage{

    private final short bpm;
    private final short bpi;
    
    public ConfigChangeNotify(short bpm, short bpi) {
        super(ServerMessageType.CONFIG_CHANGE_NOTIFY);
        this.bpm = bpm;
        this.bpi = bpi;
    }

    public short getBpi() {
        return bpi;
    }

    @Override
    public String toString() {
        return "ConfigChangeNotify{" + "bpm=" + bpm + ", bpi=" + bpi + '}';
    }

    public short getBpm() {
        return bpm;
    }
    
}
