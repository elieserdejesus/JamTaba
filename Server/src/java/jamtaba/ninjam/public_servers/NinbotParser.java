/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package jamtaba.ninjam.public_servers;

import jamtaba.ip2c.IpToCountryResolver;
import jamtaba.ninjam.NinjamServer;
import jamtaba.ninjam.NinjamUser;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

/**
 *
 * @author Elieser
 */
public class NinbotParser {

    private static final String URL = "http://ninbot.com/app/servers.php";
    private static final JSONParser parser = new JSONParser();

    public static List<NinbotServer> getPublicServers(IpToCountryResolver ipToCountryResolver) throws Exception {

        URL url = new URL(URL);
        JSONObject root = (JSONObject) parser.parse(new BufferedReader(new InputStreamReader(url.openStream())));
        JSONArray serversArray = (JSONArray) root.get("servers");
        List<NinbotServer> serversList = new ArrayList<NinbotServer>();
        for (int i = 0; i < serversArray.size(); i++) {
            JSONObject serverObject = (JSONObject) serversArray.get(i);
            String nameParts[] = ((String) serverObject.get("name")).split(":");
            String serverName = nameParts[0];
            int serverPort = Integer.parseInt(nameParts[1]);
            //String uniqueName = serverName + ":" + serverPort;
            //if (autoSongServers.containsKey(uniqueName)) {
            String streamUrl = (String) serverObject.get("stream");

                //AutoSongParser.AutoSongServer autoSongServer = autoSongServers.get(uniqueName);
            //NinjaMServer server = new NinjaMServer(serverName, serverPort, autoSongServer.getMaxUsers(), autoSongServer.getBpm(), autoSongServer.getBpi(), streamUrl);
            NinbotServer ninbotServer = new NinbotServer(serverName, serverPort, streamUrl);
            //users
            JSONArray usersArray = (JSONArray) serverObject.get("users");
            for (int j = 0; j < usersArray.size(); j++) {
                JSONObject userObject = (JSONObject) usersArray.get(j);
                String userIp = NinjamUser.getSanitizedIp((String) userObject.get("ip"));
                String userName = (String) userObject.get("name");
                String userCountryCode = ipToCountryResolver.getCountry(userIp);
                ninbotServer.addUser(new NinbotUser(userName, userIp, userCountryCode));
            }
            serversList.add(ninbotServer);
            //}
        }
        return serversList;
    }

    static class NinbotUser {

        private final String name;
        private final String ip;
        private final String countryCode;

        public NinbotUser(String name, String ip, String countryCode) {
            this.name = name;
            this.ip = ip;
            this.countryCode = countryCode;
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
        
        

    }

    static class NinbotServer {

        private final String serverName;
        private final int serverPort;
        private final String streamUrl;
        private final List<NinbotUser> users = new ArrayList<NinbotUser>();

        public NinbotServer(String server, int port, String streamUrl) {
            this.serverName = server;
            this.serverPort = port;
            this.streamUrl = streamUrl;
        }

        public List<NinbotUser> getUsers() {
            return users;
        }

        public void addUser(NinbotUser user) {
            this.users.add(user);
        }

        public String getName() {
            return serverName;
        }

        public int getPort() {
            return serverPort;
        }

        public String getStreamUrl() {
            return streamUrl;
        }

    }

}
