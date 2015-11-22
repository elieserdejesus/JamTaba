package jamtaba.ninjam;

import java.util.Random;

/**
 * @author elieser
 */
public class NinjamUser {

    private final static Random random = new Random(System.currentTimeMillis());
    
    private final String name;
    private final String ip;
    private final String fullName;
    private final String countryCode;

    public NinjamUser(String name, String ip, String coutryCode) {
        this.fullName = !ip.isEmpty() ? (name + "@" + ip) : name;
        this.name = name;
        this.ip = getSanitizedIp(ip);
        this.countryCode = coutryCode;
    }
    
    public static String getSanitizedIp(String rawIp){
        String cleanIp = rawIp;
        if(cleanIp == null || cleanIp.endsWith(".com")){
            return "";
        }
        cleanIp = cleanIp.trim();
        if(cleanIp == null || cleanIp.isEmpty()){
            return "";
        }
        if(cleanIp.endsWith("x")){
            int rndValue = 128;//returning a fixed value to allow caching in Jamtaba client. random.nextInt(180) + 20;// [20-200]
            cleanIp = cleanIp.replace("x", String.valueOf(rndValue));
        }
        return cleanIp;
    }

    public String getCountryCode() {
        return countryCode;
    }

    public String getIp() {
        return ip;
    }

    public String getName() {
        return name;
    }

    public String getFullName() {
        return fullName;
    }

}
