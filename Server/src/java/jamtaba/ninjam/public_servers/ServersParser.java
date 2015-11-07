package jamtaba.ninjam.public_servers;

import jamtaba.ip2c.IpToCountryResolver;
import jamtaba.ninjam.NinjamServer;
import jamtaba.ninjam.NinjamUser;
import jamtaba.ninjam.public_servers.AutoSongParser.AutoSongServer;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
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

    public static Collection<NinjamServer> getPublicServers(IpToCountryResolver ipToCountryResolver) throws IOException {
        List<NinjamServer> servers = new ArrayList<NinjamServer>();
        try {
            Map<String, AutoSongServer> autoSongServers = AutoSongParser.getPublicServers();
            boolean ninbotParsingFail = false;
            try {//try request servers from ninbot
                List<NinbotParser.NinbotServer> ninbotServers = NinbotParser.getPublicServers(ipToCountryResolver);
                for (NinbotParser.NinbotServer ninbotServer : ninbotServers) {
                    String uniqueName = ninbotServer.getName() + ":" + ninbotServer.getPort();
                    if (autoSongServers.containsKey(uniqueName)) {
                        AutoSongServer autoSongServer = autoSongServers.get(uniqueName);
                        NinjamServer server = new NinjamServer(ninbotServer.getName(), ninbotServer.getPort(), autoSongServer.getMaxUsers(), autoSongServer.getBpm(), autoSongServer.getBpi(), ninbotServer.getStreamUrl());

                        //users
                        for (NinbotParser.NinbotUser user : ninbotServer.getUsers()) {
                            server.addUser(new NinjamUser(user.getName(), user.getIp(), user.getCountryCode()));
                        }
                        servers.add(server);
                    }
                }
            } catch (Exception e) {
                LOGGER.log(Level.SEVERE, e.getMessage(), e);
                ninbotParsingFail = true;
            }
            
            if(ninbotParsingFail){
                for (AutoSongServer autoSongServer : autoSongServers.values()) {
                    NinjamServer server = new NinjamServer(autoSongServer.getName(), autoSongServer.getPort(), autoSongServer.getMaxUsers(), autoSongServer.getBpm(), autoSongServer.getBpi(), "");
                    for (String userName : autoSongServer.getUsers()) {
                        server.addUser(new NinjamUser(userName, "", ""));//we don't have ip or country code in auto song, but is better than nothing
                    }
                    servers.add(server);
                }
            }
        } catch (Exception pEx) {
            LOGGER.log(Level.SEVERE, pEx.getMessage(), pEx);
        }
        return servers;
    }

    public static void main(String args[]) throws IOException {
        Collection<NinjamServer> servers = ServersParser.getPublicServers(IpToCountryResolver.NULL_RESOLVER);
        for (NinjamServer server : servers) {
            System.out.println(server.getHostName() + ":" + server.getPort() + "  " + server.getUsers().size() + "/" + server.getMaxUsers() + " " + usersLine(server.getUsers()));
        }
    }

    private static String usersLine(List<NinjamUser> users) {
        String line = "{ ";
        for (int i = 0; i < users.size(); i++) {
            line += users.get(i).getName() + "(" + users.get(i).getIp() + ", " + users.get(i).getCountryCode()+")";
            if (i < users.size() - 1) {
                line += ", ";
            }
        }
        line += " }";
        return line;
    }

}
