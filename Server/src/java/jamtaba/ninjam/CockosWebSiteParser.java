package jamtaba.ninjam;

import jamtaba.ip2c.IpToCountryResolver;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 *
 * @author Elieser
 */
public class CockosWebSiteParser {

    private static final Logger LOGGER = Logger.getLogger(CockosWebSiteParser.class.getName());

    private final IpToCountryResolver ipToCountryResolver;

    public CockosWebSiteParser(IpToCountryResolver ipToCountryResolver) {
        this.ipToCountryResolver = ipToCountryResolver;
    }

    public List<NinjamServer> getPublicServers() throws Exception {
        List<NinjamServer> publicServers = new ArrayList<NinjamServer>();
        String serversContent = getRawContentString();
        Matcher matcher = SERVER_CONTENT_PATTERN.matcher(serversContent);
        while (matcher.find()) {//iterate in all servers
            String serverString = matcher.group();
            if(!serverString.contains("Server down: could not connect")){//skip down servers
                NinjamServer server = parseServer(serverString);
                if (server != null) {
                    publicServers.add(server);
                }
            }
        }
        return publicServers;
    }

    private NinjamServer parseServer(String rawServerData) {
        Matcher serverNameMatcher = SERVER_NAME_PATTERN.matcher(rawServerData);
        if (serverNameMatcher.find()) {
            String serverName = serverNameMatcher.group(1);
            int serverPort = Integer.parseInt(serverNameMatcher.group(3));
            Matcher bpiBpmMatcher = BPM_BPI_PATTERN.matcher(rawServerData);
            if (bpiBpmMatcher.find(serverNameMatcher.end())) {
                int bpm = Integer.parseInt(bpiBpmMatcher.group(1));
                short bpi = Short.parseShort(bpiBpmMatcher.group(2));
                Matcher usersCountMatcher = USERS_COUNT_PATTERN.matcher(rawServerData);
                if (usersCountMatcher.find(serverNameMatcher.end())) {
                    int serverMaxUsers = Integer.parseInt(usersCountMatcher.group(1));
                    String serverStreamUrl = getServerStreamUrl(serverName, serverPort);
                    NinjamServer server = new NinjamServer(serverName, serverPort, serverMaxUsers, bpm, bpi, serverStreamUrl);
                    String usersListRawString = rawServerData.substring(usersCountMatcher.end()).replaceAll("</?[uU][lL]>", "");
                    List<NinjamUser> users = parseUsersList(usersListRawString);
                    for (NinjamUser user : users) {
                        server.addUser(user);
                    }
                    return server;
                } else {
                    LOGGER.log(Level.SEVERE, "Returning Null server because the usersCountMatcher fails: {0}", rawServerData);
                }
            } else {
                LOGGER.log(Level.SEVERE, "Returning Null server because the bpiBpmMatcher fails: {0}", rawServerData);
            }
        } else {
            LOGGER.log(Level.SEVERE, "Returning Null server because the serverNameMatcher fails: {0}", rawServerData);
        }
        return null;
    }

    private String getRawContentString() throws Exception {
        String rawContent = getUrlContent(new URL(COCKOS_URL));
        String delimiter = "<BR><BR>";
        int index = rawContent.indexOf(delimiter);
        if (index >= 0) {
            int beginIndex = rawContent.indexOf(delimiter, index + 1) + delimiter.length();
            int endIndex = rawContent.indexOf(delimiter, beginIndex + 1);
            if (beginIndex >= 0 && endIndex > beginIndex) {
                return rawContent.substring(beginIndex, endIndex);
            }
        } else {
            LOGGER.log(Level.SEVERE, "Returning an empty string because the delimiter {0} was not founded rawString: {1}", new Object[]{delimiter, rawContent});
        }
        return "";
    }

    private List<NinjamUser> parseUsersList(String usersListRawString) {
        String usersStrings[] = usersListRawString.split("<BR>");
        List<NinjamUser> users = new ArrayList<NinjamUser>();
        for (String userString : usersStrings) {
            if (!userString.isEmpty()) {
                users.add(parseUser(userString));
            }
        }
        return users;
    }

    private NinjamUser parseUser(String userString) {
        String userName = userString.substring(0, userString.indexOf("<font"));//user names are sufixed by <font .... For example: HammerOn<font... 
        Matcher ipMatcher = USER_IP_PATTERN.matcher(userString);
        String userIP = "";
        if (ipMatcher.find(userName.length())) {
            userIP = NinjamUser.getSanitizedIp(ipMatcher.group());
        }
        String countryCode = ipToCountryResolver.getCountry(userIP);
        return new NinjamUser(userName, userIP, countryCode);
    }

    private String getUrlContent(URL url) throws Exception {
        InputStream in = url.openStream();
        StringBuilder sb = new StringBuilder();
        BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String read;
        while ((read = br.readLine()) != null) {
            sb.append(read);
        }
        br.close();
        return sb.toString();
    }

    public static void main(String args[]) throws Exception {
        CockosWebSiteParser serversParser = new CockosWebSiteParser(IpToCountryResolver.NULL_RESOLVER);
        List<NinjamServer> publicServers = serversParser.getPublicServers();
        for (NinjamServer publicServer : publicServers) {
            System.out.println(publicServer.getUniqueName() + " "
                    + publicServer.getBpm() + " bpm "
                    + publicServer.getBpi() + " bpi "
                    + "max users: " + publicServer.getMaxUsers()
                    + " stream url: " + publicServer.getStreamUrl()
            );
            for (NinjamUser user : publicServer.getUsers()) {
                System.out.println("\t" + user.getFullName() + " (" + user.getCountryCode() + ")");
            }
            System.out.println("");
        }
    }

    private static final String COCKOS_URL = "http://autosong.ninjam.com/server-list.php";
    private static final Pattern SERVER_NAME_PATTERN = Pattern.compile("(([a-z]+[\\.]?)+):([0-9]+)");
    private static final Pattern SERVER_CONTENT_PATTERN = Pattern.compile(SERVER_NAME_PATTERN.pattern() + ".*?</ul>");//every server section end with a lower case </ul>
    private static final Pattern BPM_BPI_PATTERN = Pattern.compile("([0-9]{2,3}) BPM/([0-9]{1,2}) BPI");//95 BPM/16 BPI
    private static final Pattern USERS_COUNT_PATTERN = Pattern.compile("[0-9]{1,2}/([0-9]{1,2}) Users:");//0/8 Users:
    private static final Pattern USER_IP_PATTERN = Pattern.compile("@([0-9]{1,3}\\.){3}x");//@195.132.200.x
    private static final Map<String, Map<Integer, String>> SERVERS_STREAMS = new HashMap<String, Map<Integer, String>>();

    static {
        SERVERS_STREAMS.put("ninjamer.com", new HashMap<Integer, String>());
        SERVERS_STREAMS.put("ninbot.com", new HashMap<Integer, String>());
        SERVERS_STREAMS.put("mutantlab.com", new HashMap<Integer, String>());
        SERVERS_STREAMS.put("ninjamers.servebeer.com", new HashMap<Integer, String>());

        SERVERS_STREAMS.get("ninbot.com").put(2049, "http://ninbot.com:8000/2049");
        SERVERS_STREAMS.get("ninbot.com").put(2050, "http://ninbot.com:8000/2050");
        SERVERS_STREAMS.get("ninbot.com").put(2051, "http://ninbot.com:8000/2051");
        SERVERS_STREAMS.get("ninbot.com").put(2052, "http://ninbot.com:8000/2052");

        SERVERS_STREAMS.get("ninjamer.com").put(2049, "http://ninjamer.com:15000/jambot1");
        SERVERS_STREAMS.get("ninjamer.com").put(2050, "http://ninjamer.com:15000/jambot2");
        SERVERS_STREAMS.get("ninjamer.com").put(2051, "http://ninjamer.com:15000/jambot3");
        SERVERS_STREAMS.get("ninjamer.com").put(2052, "http://ninjamer.com:15000/jambot4");

        SERVERS_STREAMS.get("mutantlab.com").put(2049, "http://mutantlab.com:8010/mj");

    }

    private static String getServerStreamUrl(String server, int port) {
        if (SERVERS_STREAMS.containsKey(server)) {
            if (SERVERS_STREAMS.get(server).containsKey(port)) {
                return SERVERS_STREAMS.get(server).get(port);
            }
        }
        return "";
    }

}
