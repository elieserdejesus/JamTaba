package jamtaba.ip2c;

/**
 * @author zeh
 */
public interface IpToCountryResolver {

    String getCountry(String ip);

    public static final IpToCountryResolver NULL_RESOLVER = new IpToCountryResolver() {

        public String getCountry(String ip) {
            return "";
        }
    };

}
