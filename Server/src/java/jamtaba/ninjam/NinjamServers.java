
package jamtaba.ninjam;

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
    
    private static final int UPDATE_PERIOD = 30000;//30 seconds
    private static Collection<NinjaMServer> servers = new ArrayList<NinjaMServer>();
    private static long lastUpdate=  0;
    
    public static Collection<NinjaMServer> getServers(IpToCountryResolver ipToCountryResolver){
        if(needUpdate()){
           try{
            servers = ServersParser.getPublicServers(ipToCountryResolver);
           }
           catch(IOException ex){
               Logger.getLogger(NinjamServers.class.getName()).log(Level.SEVERE, ex.getMessage(), ex);
           }
        }
        return servers;
    }
    
    private static boolean needUpdate(){
        return System.currentTimeMillis() - lastUpdate > UPDATE_PERIOD;
    }
    
    
}
