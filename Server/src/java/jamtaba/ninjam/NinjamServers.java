package jamtaba.ninjam;

import jamtaba.ip2c.IpToCountryResolver;
import java.util.ArrayList;
import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author elieser
 */
public class NinjamServers {

    private static final Logger LOGGER = Logger.getLogger(NinjamServers.class.getName());
    
    private static final int UPDATE_PERIOD = 30000;//30 seconds
    private static Collection<NinjamServer> servers = new ArrayList<NinjamServer>();
    private static long lastUpdate = 0;

    public static Collection<NinjamServer> getServers(IpToCountryResolver ipToCountryResolver) {
        if (needUpdate()) {
            try {
                CockosWebSiteParser serversParser = new CockosWebSiteParser(ipToCountryResolver);
                LOGGER.log(Level.INFO, "updating the public servers list...");
                servers = serversParser.getPublicServers();
                lastUpdate = System.currentTimeMillis();
            } catch (Exception ex) {
                LOGGER.log(Level.SEVERE, null, ex);
            }
        }
        else{
            LOGGER.log(Level.INFO, "Returning the cached servers list!");
        }
        return servers;
    }

    private static boolean needUpdate() {
        return System.currentTimeMillis() - lastUpdate >= UPDATE_PERIOD;
    }

}
