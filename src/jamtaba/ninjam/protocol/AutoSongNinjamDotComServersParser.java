package jninjam.protocol;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Scanner;
import jninjam.NinjaMServer;
import jninjam.NinjaMUser;

/**
 *
 * @author zeh
 */
public class AutoSongNinjamDotComServersParser implements NinjamPublicServersParser {

    private static final String PUBLIC_SERVERS_URL = "http://autosong.ninjam.com/serverlist.php";

    private boolean skipUsers = false;//parser only servers info

    public AutoSongNinjamDotComServersParser() {
    }

    public AutoSongNinjamDotComServersParser(boolean skipUsers) {
        this.skipUsers = skipUsers;
    }

    @Override
    public Collection<NinjaMServer> getPublicServers() throws IOException {
        String urlContent = getUrlContent(PUBLIC_SERVERS_URL);
        String serversInfos[] = urlContent.split("SERVER");
        List<NinjaMServer> servers = new ArrayList<NinjaMServer>();
        for (String infos : serversInfos) {
            if (!infos.isEmpty()) {
                String parts[] = getTreePartServerInfos(infos);
                String hostInfos[] = parts[0].split(":");
                int port = Integer.parseInt(hostInfos[1]);
                String host = hostInfos[0];
                NinjaMServer server = NinjaMServer.getServer(host, port);
                boolean serverDown = parts[1].contains("Server down");
                if (!serverDown) {
                    int maxUsers = Integer.parseInt(parts[2].split(":")[0].split("/")[1]);
                    if (!skipUsers) {
                        String users[] = parts[2].split(":")[1].split(",");
                        for (String userName : users) {
                            String userFullName = userName + "@" + host;
                            server.addUser(NinjaMUser.getUser(userFullName));
                        }
                    }
                    server.setMaxUsers(maxUsers);
                    server.setIsActive(true);
                } else {
                    server.setIsActive(false);
                    server.setMaxUsers(0);
                }
                servers.add(server);
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

    @Override
    public void shutdown() {
        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    public static void main(String args[]) throws IOException{
        AutoSongNinjamDotComServersParser parser = new AutoSongNinjamDotComServersParser();
        for (NinjaMServer ninjaMServer : parser.getPublicServers()) {
            System.out.println(ninjaMServer);
            for (NinjaMUser ninjaMUser : ninjaMServer.getUsers()) {
                System.out.println("\t" + ninjaMUser);
            }
        }
    }
    
}
