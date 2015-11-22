
package jamtaba.ninjam;

import jamtaba.ninjam.public_servers.ServersParser;
import jamtaba.ip2c.IpToCountryResolver;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author elieser
 */
public class NinjamServers {
    
    private static final int UPDATE_PERIOD = 10000;//10 seconds
    private static Collection<NinjamServer> servers = new ArrayList<NinjamServer>();
    private static long lastUpdate=  0;
    
    public static Collection<NinjamServer> getServers(IpToCountryResolver ipToCountryResolver){
        if(needUpdate()){
           try{
            servers = ServersParser.getPublicServers(ipToCountryResolver);
           }
           catch(IOException ex){
               Logger.getLogger(NinjamServers.class.getName()).log(Level.SEVERE, ex.getMessage(), ex);
           }
        }
        lastUpdate = System.currentTimeMillis();
        return servers;
    }
    
    private static boolean needUpdate(){
        return System.currentTimeMillis() - lastUpdate > UPDATE_PERIOD;
    }
    
    
}
