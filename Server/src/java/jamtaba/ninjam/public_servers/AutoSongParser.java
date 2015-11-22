package jamtaba.ninjam.public_servers;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class AutoSongParser {
 
    private static final String PUBLIC_SERVERS_URL = "http://autosong.ninjam.com/serverlist.php";

    public static Map<String, AutoSongServer> getPublicServers() throws IOException {
        //SERVER "ninbot.com:2050" "120 BPM/16" "5/8:Audialite,karmakameleon,notMe,T-rex,theo_h"
        String urlContent = getUrlContent(PUBLIC_SERVERS_URL).replace("END", "");
        String[] serversInfos = urlContent.split("SERVER");
        Map<String, AutoSongServer> servers = new HashMap<String, AutoSongServer>();
        for (String serverInfos : serversInfos) {
            if (!serverInfos.isEmpty()) {
                String[] parts = getTreePartServerInfos(serverInfos);
                String[] hostInfos = parts[0].split(":");
                int port = Integer.parseInt(hostInfos[1]);
                String host = hostInfos[0];
                boolean serverDown = parts[1].contains("Server down");
                if (!serverDown) {
                    short maxUsers = Short.parseShort(parts[2].split(":")[0].split("/")[1]);
                    short bpi = Short.parseShort(parts[1].split("/")[1]);
                    int bpm = Integer.parseInt(parts[1].split("/")[0].replace(" BPM", ""));
                    AutoSongServer server = new AutoSongServer(host, port, maxUsers, bpi, bpm);
                    servers.put(server.getName() + ":" + server.getPort(), server);
                    String[] usersList = parts[2].split(":")[1].split(",");
                    for (String userName : usersList) {
                        server.addUser(userName);
                    }
                }
            }
        }
        return servers;
    }

    /***
     * @param serverInfos
     * @return The first array element contains server name and port. The secong array element contains
     * current server bpm and BPI values. The third array element contains users count and users names.
     */
    private static String[] getTreePartServerInfos(String serverInfos) {
        String[] parts = new String[3];
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
    
    static class AutoSongServer {

        private final String name;
        private final short maxUser;
        private final int port;
        private final short bpi;
        private final int bpm;
        private final List<String> users = new ArrayList<String>();

        public AutoSongServer(String name, int port, short maxUser, short bpi, int bpm) {
            this.name = name;
            this.port = port;
            this.maxUser = maxUser;
            this.bpi = bpi;
            this.bpm = bpm;
        }
        
        public void addUser(String userName){
            this.users.add(userName);
        }
        
        public List<String> getUsers(){
            return users;
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
    
}
