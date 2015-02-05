package jamtaba.ip2c;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletContext;
import net.firefang.ip2c.Country;
import net.firefang.ip2c.IP2Country;

/**
 * @author zeh
 */
public class Ip2CResolver implements IpToCountryResolver {

    private static final Logger LOGGER = Logger.getLogger(Ip2CResolver.class.getName());
    private static Ip2CResolver instance;

    private final IP2Country ip2c;

    public Ip2CResolver(ServletContext servletContext) throws IOException {
        try {
            URL resource = servletContext.getResource("/WEB-INF/ip-to-country.bin");
            File file = new File(resource.toURI());
            ip2c = new IP2Country(file.toString(), IP2Country.MEMORY_CACHE);
        } catch (URISyntaxException Ex) {
            throw new IOException(Ex);
        }
    }

    @Override
    public String getCountry(String ip) {
        if(ip == null || ip.isEmpty()){
            return "";
        }
        try {
            Country c = ip2c.getCountry(ip);
            if (c != null) {
                return c.get2cStr();
            }
        } catch (IOException ex) {
            LOGGER.severe("tentando resolver o ip \"" + ip +"\"");
            LOGGER.log(Level.SEVERE, ex.getMessage(), ex);
        }
        return "UNKNOWN";
    }
}
