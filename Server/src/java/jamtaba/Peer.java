package jamtaba;

import com.googlecode.objectify.Ref;
import com.googlecode.objectify.annotation.Cache;
import com.googlecode.objectify.annotation.Entity;
import com.googlecode.objectify.annotation.Id;
import com.googlecode.objectify.annotation.Load;
import java.util.Date;
import java.util.logging.Logger;

@Cache
@Entity
public class Peer {

    private static final Logger LOGGER = Logger.getLogger(Peer.class.getName());

    @Id
    private Long id;

    //@Parent
    @Load
    private Ref<RealtimeRoom> roomRef;

    private String publicIp;

    private String publicPort;

    private String userName;

    private Date lastUpdate;

    private String region;

    private String country;

    private String channelName;//local channel name

    /**
     * jamtaba version
     */
    String version;
    //private int audioBufferSize;
    private int sampleRate;
    private String environment;//vst host name or standalone 

    private boolean playingInNinjamRoom;

    private int instrumentID;//code of selected instrument icon

    public Peer() {
        //audioLatency = 0;
    }

    public Peer(String publicIp, String publicPort, String userName, String region, String country, int instrumentID, String channelName, String version,  int sampleRate, String environment) {
        this.publicIp = publicIp;
        this.publicPort = publicPort;
        this.userName = userName;
        this.country = country;
        this.region = region;
        this.instrumentID = instrumentID;
        this.channelName = channelName;
        this.environment = environment;
    //    this.audioBufferSize = audioBufferSize;
        this.sampleRate = sampleRate;
        this.version = version;
    }

    public void setPlayingInNinjam(boolean status) {
        this.playingInNinjamRoom = status;
    }

    public boolean isPlayingInNinjam() {
        return playingInNinjamRoom;
    }

    public Long getRoomID() {
        return getRoom().getId();
    }

    public int getInstrumentID() {
        return instrumentID;
    }

    public RealtimeRoom getRoom() {
        if (roomRef == null) {
            return null;
        }
        return roomRef.get();
    }

    public void setJamRoom(RealtimeRoom room) {
        if (room != null) {
            this.roomRef = Ref.create(room);
        } else {
            roomRef = null;
        }
    }

    public void setChannelName(String channelName) {
        this.channelName = channelName;
    }

    public String getChannelName() {
        return channelName;
    }

    public void updateLastUpdateTime() {
        lastUpdate = new Date();//System.currentTimeMillis();
    }

//    public Date getLastUpdate() {
//        return lastUpdate;
//    }
    /**
     * @return A quantidade de mnewilisegundos deste o último update
     */
    public long getTimeSinceLastUpdate() {
        Date now = new Date();
        if (lastUpdate == null) {
            return 0;
        }
        return now.getTime() - lastUpdate.getTime();//  System.currentTimeMillis() - lastUpdate;
    }

//    public boolean isAlive(long MAX_TIME_WITHOUT_UPDATES) {
//        return getTimeSinceLastUpdate() <= MAX_TIME_WITHOUT_UPDATES;
//    }
    public Long getId() {
        return this.id;
    }

//    public String getPrivateIp() {
//        return this.privateIp;
//    }
    public String getPublicIp() {
        return publicIp;
    }

//    public String getPrivatePort() {
//        return privatePort;
//    }
    public String getPublicPort() {
        return publicPort;
    }

    public String getUserName() {
        return userName;
    }

    public String getRegion() {
        return region;
    }

    public String getCountry() {
        return country;
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 83 * hash + (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }

    public String getVersion() {
        return version;
    }

//    public int getAudioBufferSize() {
//        return audioBufferSize;
//    }

    public String getEnvironment() {
        return environment;
    }

    public int getSampleRate() {
        return sampleRate;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Peer other = (Peer) obj;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        RealtimeRoom jamRoom = (roomRef != null) ? (roomRef.get()) : null;
        return "Peer{" + "id=" + id + ", userName=" + userName + ", jamRoom=" + jamRoom + ", region=" + region + ", country=" + country + '}';
    }

    public void setInstrumentID(int instrumentID) {
        this.instrumentID = instrumentID;
    }

}
