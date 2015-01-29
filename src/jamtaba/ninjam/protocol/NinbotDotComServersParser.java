package jninjam.protocol;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import jninjam.NinjaMServer;
import jninjam.NinjaMUser;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

/**
 * @author zeh
 */
public class NinbotDotComServersParser implements NinjamPublicServersParser {

    private static final Logger LOGGER = Logger.getLogger(NinbotDotComServersParser.class.getName());
    private static final String URL = "http://ninbot.com/app/servers.php";
    private static final JSONParser parser = new JSONParser();

    @Override
    public Collection<NinjaMServer> getPublicServers() throws IOException {
        try {
            URL url = new URL(URL);
            JSONObject root = (JSONObject)parser.parse(new BufferedReader(new InputStreamReader(url.openStream())));
            JSONArray serversArray = (JSONArray)root.get("servers");
            Collection<NinjaMServer> servers = new ArrayList<NinjaMServer>();
            for (int i = 0; i < serversArray.size(); i++) {
                JSONObject serverObject = (JSONObject)serversArray.get(i);
                String nameParts[] = ((String)serverObject.get("name")).split(":");
                NinjaMServer server = NinjaMServer.getServer(nameParts[0], Integer.valueOf( nameParts[1]));
                String streamUrl = (String)serverObject.get("stream");
                server.setStreamUrl(streamUrl);
                JSONArray usersArray = (JSONArray)serverObject.get("users");
                
                Set<NinjaMUser> onlineUsers = new HashSet<NinjaMUser>();
                for (int j = 0; j < usersArray.size(); j++) {
                    JSONObject userObject = (JSONObject)usersArray.get(j);
                    String userIp = (String)userObject.get("ip");
                    String userName = (String)userObject.get("name");
                    String fullName = userName;
                    if(!userIp.isEmpty()){
                        fullName += "@" + userIp;
                    }
                    //server.addUser( NinjaMUser.getUser(fullName));
                    onlineUsers.add(NinjaMUser.getUser(fullName));
                    server.refreshUserList(onlineUsers);
                }
                servers.add(server);
            }
            return servers;
        } catch (ParseException pEx) {
            LOGGER.log(Level.SEVERE, pEx.getMessage(), pEx);
        }
        return Collections.EMPTY_LIST;
    }

    @Override
    public void shutdown() {
        //throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    public static void main(String args[]) throws IOException {
        NinbotDotComServersParser parser = new NinbotDotComServersParser();
        Collection<NinjaMServer> servers = parser.getPublicServers();
        for (NinjaMServer server : servers) {
            System.out.println(server);
            System.out.println("--");
        }
    }

}
