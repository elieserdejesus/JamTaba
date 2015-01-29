package jninjam.protocol.server.messages;

import jninjam.NinjaMUser;

/**
 *
 * @author zeh
 */
public class UserChannel {

    private final NinjaMUser user;
    private String name;
    private boolean active;
    private final int index;
    private short volume;//(dB gain, 0=0dB, 10=1dB, -30=-3dB, etc)
    private byte pan;//Pan [-128, 127]
    private byte flags;

    @Override
    public String toString() {
        return "UserChannel{" + "name=" + name + ", active=" + active + ", index=" + index + ", volume=" + volume + ", pan=" + pan + ", flags=" + getFlags() + '}';
    }

    public UserChannel(NinjaMUser user, String name, boolean active, int channelIndex, short volume, byte pan, byte flags) {
        this.user = user;
        this.name = name;
        this.active = active;
        this.index = channelIndex;
        this.volume = volume;
        this.pan = pan;
        this.flags = flags;
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 37 * hash + (this.user != null ? this.user.hashCode() : 0);
        hash = 37 * hash + this.index;
        return hash;
    }

    public boolean isActive() {
        return active && flags == 0;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final UserChannel other = (UserChannel) obj;
        if (this.user != other.user && (this.user == null || !this.user.equals(other.user))) {
            return false;
        }
        if (this.index != other.index) {
            return false;
        }
        return true;
    }

    

    public int getIndex() {
        return index;
    }
    
    public String getName() {
        return name;
    }

    public NinjaMUser getUser() {
        return user;
    }

    public void setName(String name) {
        this.name = name;
    }

    public byte getFlags() {
        return flags;
    }

    public void setFlags(byte flags) {
        this.flags = flags;
    }

}
