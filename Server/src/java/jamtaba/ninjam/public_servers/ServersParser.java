package jamtaba.ninjam.public_servers;

import jamtaba.ip2c.IpToCountryResolver;
import jamtaba.ninjam.NinjaMServer;
import jamtaba.ninjam.NinjaMUser;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

/**
 * @author zeh
 */
public class ServersParser {

    private static final Logger LOGGER = Logger.getLogger(ServersParser.class.getName());
    private static final String URL = "http://ninbot.com/app/servers.php";
    private static final JSONParser parser = new JSONParser();
    
  
    public static Collection<NinjaMServer> getPublicServers(IpToCountryResolver ipToCountryResolver) throws IOException {
        try {
            Map<String, AutoSongServer> autoSongServers = AutoSongParser.getPublicServers();
            //+++++++++++++++
            URL url = new URL(URL);
            JSONObject root = (JSONObject) parser.parse(new BufferedReader(new InputStreamReader(url.openStream())));
            JSONArray serversArray = (JSONArray) root.get("servers");
            Collection<NinjaMServer> servers = new ArrayList<NinjaMServer>();
            for (int i = 0; i < serversArray.size(); i++) {
                JSONObject serverObject = (JSONObject) serversArray.get(i);
                String nameParts[] = ((String) serverObject.get("name")).split(":");
                String serverName = nameParts[0];
                int serverPort = Integer.parseInt(nameParts[1]);
                String uniqueName = serverName + ":" + serverPort;
                if (autoSongServers.containsKey(uniqueName)) {
                    String streamUrl = (String) serverObject.get("stream");
                    JSONArray usersArray = (JSONArray) serverObject.get("users");
                    AutoSongServer autoSongServer = autoSongServers.get(uniqueName);
                    NinjaMServer server = new NinjaMServer(serverName, serverPort, autoSongServer.getMaxUsers(), autoSongServer.getBpm(), autoSongServer.getBpi(), streamUrl);

                    //users
                    for (int j = 0; j < usersArray.size(); j++) {
                        JSONObject userObject = (JSONObject) usersArray.get(j);
                        String userIp = NinjaMUser.getSanitizedIp( (String) userObject.get("ip"));
                        String userName = (String) userObject.get("name");
                        String userCountryCode = ipToCountryResolver.getCountry(userIp);
                        server.addUser(new NinjaMUser(userName, userIp, userCountryCode));
                    }
                    servers.add(server);
                }
            }
            return servers;
        } catch (ParseException pEx) {
            LOGGER.log(Level.SEVERE, pEx.getMessage(), pEx);
        }
        return Collections.EMPTY_LIST;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static class AutoSongParser {

        private static final String PUBLIC_SERVERS_URL = "http://autosong.ninjam.com/serverlist.php";

        public static Map<String, AutoSongServer> getPublicServers() throws IOException {
            String urlContent = getUrlContent(PUBLIC_SERVERS_URL).replace("END", "");
            String serversInfos[] = urlContent.split("SERVER");
            Map<String, AutoSongServer> servers = new HashMap<String, AutoSongServer>();
            for (String infos : serversInfos) {
                if (!infos.isEmpty()) {
                    String parts[] = getTreePartServerInfos(infos);
                    String hostInfos[] = parts[0].split(":");
                    int port = Integer.parseInt(hostInfos[1]);
                    String host = hostInfos[0];
                    boolean serverDown = parts[1].contains("Server down");
                    if (!serverDown) {
                        short maxUsers = Short.parseShort(parts[2].split(":")[0].split("/")[1]);
                        short bpi = Short.parseShort(parts[1].split("/")[1]);
                        int bpm = Integer.parseInt(parts[1].split("/")[0].replace(" BPM", ""));
                        AutoSongServer server = new AutoSongServer(host, port, maxUsers, bpi, bpm);
                        servers.put(server.getName() + ":" + server.getPort(), server);
                    }
                }
            }
            return servers;
        }

        private static String[] getTreePartServerInfos(String serverInfos) {
            String parts[] = new String[3];
            int start = 0;
            int end = 0;
            int offset = 0;
            for (int i = 0; i < parts.length; i++) {
                start = serverInfos.indexOf("\"", offset) + 1;
                end = serverInfos.indexOf("\"", start);
                parts[i] = serverInfos.substring(start, end);
                offset = end + 1;
            }
            return parts;
        }

        private static String getUrlContent(String url) throws MalformedURLException, IOException {
            URL theUrl = new URL(url);
            Scanner scanner = new Scanner(new BufferedInputStream(theUrl.openStream()));
            StringBuilder builder = new StringBuilder();
            while (scanner.hasNext()) {
                builder.append(scanner.nextLine());
            }
            return builder.toString().replace("END", "");
        }
    }

    static class AutoSongServer {

        private final String name;
        private final short maxUser;
        private final int port;
        private final short bpi;
        private final int bpm;

        public AutoSongServer(String name, int port, short maxUser, short bpi, int bpm) {
            this.name = name;
            this.port = port;
            this.maxUser = maxUser;
            this.bpi = bpi;
            this.bpm = bpm;
        }

        public short getBpi() {
            return bpi;
        }

        public int getBpm() {
            return bpm;
        }

        public short getMaxUsers() {
            return maxUser;
        }

        public String getName() {
            return name;
        }

        public int getPort() {
            return port;
        }

    }

    public static void main(String args[]) throws IOException {
//        Collection<NinjaMServer> servers = ServersParser.getPublicServers();
//        for (NinjaMServer server : servers) {
//            System.out.println(server.getHostName() + ":" + server.getPort() + "  " + server.getUsers().size() + "/" + server.getMaxUsers() + " " + usersLine(server.getUsers()));
//        }
    }
    
//    private static String usersLine(List<NinjaMUser> users){
//        String line  = "{";
//        for (int i = 0; i < users.size(); i++) {
//            line += users.get(i).getName();
//            if(i < users.size() - 1){
//                line += ", ";
//            }
//        }
//        line += "}";
//        return line;
//    }

}
