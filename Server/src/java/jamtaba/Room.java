package jamtaba;

import com.googlecode.objectify.Ref;
import com.googlecode.objectify.annotation.Cache;
import com.googlecode.objectify.annotation.Entity;
import com.googlecode.objectify.annotation.Id;
import com.googlecode.objectify.annotation.Load;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.logging.Logger;

@Cache
@Entity
public class Room {

    private static final Logger LOGGER = Logger.getLogger(Room.class.getName());
    
    public static final Long WAITING_ROOM_ID = 1L;
    
    //public static final Long NINJAM_ROOM_ID = -100L;
    
    public static final Integer DEFAULT_MAX_USERS = 4;

    @Id
    private Long id;

    private String name;

    //private Integer styleCode;

    private Integer maxUsers;

    @Load
    private List<Ref<Peer>> peersRefs = new ArrayList<Ref<Peer>>();
    //este campo não pode ser final, Objectify não trabalha com campos final

    private boolean staticRoom = false;//salas estáticas não são deletadas nem alteradas pelos usuários

    //private boolean waitingRoom = false;

//    @Load
//    //@Parent
//    private Ref<Peer> ownerRef;

    public Room() {
        this.staticRoom = true;
    }

    public Room(String name, boolean staticRoom) {
        this(name,  DEFAULT_MAX_USERS, Collections.EMPTY_LIST, staticRoom);
    }

    public Room(String name, long id) {
        this(name, -1, Collections.EMPTY_LIST, false);
        this.id = id;
        this.staticRoom = true;
    }

    public Room(String name, int maxUsers, List<Peer> peers, boolean staticRoom) {
        this.name = name;
        //this.styleCode = styleCode;
        this.maxUsers = maxUsers;
        this.staticRoom = staticRoom;
        initializePeers(peers);
    }
   
    public boolean isWaitingRoom() {
        return id == WAITING_ROOM_ID;
    }

//    public JamRoom(String name, boolean staticRoom) {
//        this(name, "Todos os estilos", staticRoom);
//    }
//    public void setStyleCode(Integer styleCode) {
//        this.styleCode = styleCode;
//    }

    public boolean isStatic() {
        return staticRoom;
    }

//    public boolean hasOwner() {
//        try{
//            return ownerRef != null && ownerRef.get() != null;
//        }
//        catch(NotFoundException e){
//            LOGGER.log(Level.SEVERE, e.getMessage(), e);
//        }
//        return false;
//    }

//    public void setPeers(Collection<Peer> peers) {
//        this.peers.clear();
//        for (Peer peer : peers) {
//            this.peers.add(Ref.create(peer));
//        }
//    }
    public synchronized void setMaxUsers(int newMaxUsersValue) {
        if (newMaxUsersValue > peersRefs.size()) {
            maxUsers = newMaxUsersValue;
        }
    }

    public synchronized List<Peer> getPeers() {
        List<Peer> peers = new ArrayList<Peer>();
        List<Ref> deadRefs = new ArrayList<Ref>();
        for (Ref<Peer> ref : peersRefs) {
            Peer p = ref.get();
            if(p != null){
                peers.add(p);
            }
            else{
                deadRefs.add(ref);
            }
        }
        peersRefs.removeAll(deadRefs);
        return peers;
    }

//    public synchronized boolean peerIsOwner(Long peerID) {
//        if (!hasOwner()) {
//            return false;
//        }
//        return getOwner().getId().equals(peerID);
//    }

    private void initializePeers(List<Peer> peersToAdd) {
        for (Peer peer : peersToAdd) {
            addPeer(peer);
        }
    }

    public Integer getMaxUsers() {
        return maxUsers;
    }

    public synchronized boolean addPeer(Peer p) {
        if (!isFull()) {
            peersRefs.add(Ref.create(p));
            p.setJamRoom(this);
//            if (!isStatic() && !hasOwner()) {
//                setOwner(p);
//            }
            return true;
        }
        return false;
    }

    public synchronized void removePeer(Long peerID) {
        for (Ref<Peer> peerRef : peersRefs) {
            Peer peer = peerRef.get();
            if (peer != null && peer.getId().equals(peerID)) {
//                if (!isStatic() && hasOwner() && peer.getId().equals(getOwner().getId())) {//se o owner está saindo
//                    if (!isEmpty()) {
//                        setOwner(peersRefs.get(0).get());//o primeiro da lista passa a ser o novo owner
//                    } else {
//                        setOwner(null);
//                    }
//                }
                peersRefs.remove(peerRef);
                peer.setJamRoom(null);
                break;
            }
        }
//        if (isEmpty() && !isStatic()) {
//            setOwner(null);
//        }
    }

//    private void setOwner(Peer peer) {
//        if (peer == null) {
//            ownerRef = null;
//        } else {
//            ownerRef = Ref.create(peer);
//        }
//    }

    public boolean isEmpty() {
        return peersRefs.isEmpty();
    }

    public boolean isFull() {
        return !isWaitingRoom() && peersRefs.size() == maxUsers;
    }

    public Long getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String n) {
        this.name = n;
    }

//    public Integer getStyleCode() {
//        return styleCode;
//    }

//    public Peer getOwner() {
//        if (ownerRef != null) {
//            try{
//                return ownerRef.safe();
//            }
//            catch(NotFoundException e){
//                LOGGER.log(Level.SEVERE, e.getMessage(), e);
//            }
//        }
//        return null;
//    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 67 * hash + (this.id != null ? this.id.hashCode() : 0);
        hash = 67 * hash + (this.name != null ? this.name.hashCode() : 0);
        //hash = 67 * hash + (this.styleCode != null ? this.styleCode.hashCode() : 0);
        hash = 67 * hash + (this.staticRoom ? 1 : 0);
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Room other = (Room) obj;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    

    
}
