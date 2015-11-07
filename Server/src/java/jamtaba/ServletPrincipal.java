package jamtaba;

import com.googlecode.objectify.ObjectifyService;
import jamtaba.command.AbstractCommand;
import java.io.PrintStream;
import java.util.*;
import javax.servlet.ServletException;
import javax.servlet.http.*;
import jamtaba.command.Command;
import jamtaba.command.CommandFactory;
import jamtaba.command.DisconnectFromServer;
import jamtaba.ip2c.Ip2CResolver;
import jamtaba.ip2c.IpToCountryResolver;
import jamtaba.ninjam.NinjamServer;
import jamtaba.ninjam.NinjamServers;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ServletPrincipal extends HttpServlet {

    private static final Logger LOGGER = Logger.getLogger(ServletPrincipal.class.getName());

    private static final long serialVersionUID = 1L;
    private static final int STATIC_REAL_TIME_ROOMS = 5;
    private static IpToCountryResolver ipToCountryResolver;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    @Override
    public synchronized void init() throws ServletException {
        super.init();

        try {
            ipToCountryResolver = new Ip2CResolver(getServletContext());//estou usando o servletContext para carregar o arquivo ip-to-country.bin
        } catch (IOException ioException) {
            LOGGER.log(Level.SEVERE, ioException.getMessage(), ioException);
            ipToCountryResolver = IpToCountryResolver.NULL_RESOLVER;
        }

        ObjectifyService.register(Peer.class);
        ObjectifyService.register(RealtimeRoom.class);
        ObjectifyService.register(Version.class);
        
        DbUtils.tryCreateCurrenVersion();//create current version objet in data store if necessary
        
        //+++++++++ CREATE THE STATIC (created by the system) ROOMS
        Collection<RealtimeRoom> rooms = DbUtils.loadRooms();
        if (rooms.isEmpty()) {
            List<RealtimeRoom> jamRooms = new ArrayList<RealtimeRoom>(STATIC_REAL_TIME_ROOMS);
            for (int i = 0; i < STATIC_REAL_TIME_ROOMS; i++) {
                jamRooms.add(new RealtimeRoom("Room " + (i + 1), true));//a sala é estática
            }
            //cria a sala de espera e a sala onde os peers que entram em salas do ninjam ficam
            RealtimeRoom waitintRoom = new RealtimeRoom("Waiting Room", RealtimeRoom.WAITING_ROOM_ID);

            DbUtils.createWaitingRoom(waitintRoom);
            DbUtils.save(jamRooms);
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    @Override
    public void doPost(final HttpServletRequest req, final HttpServletResponse resp) throws ServletException {
        Command command = CommandFactory.getCommand(req);
        try {
            configureResponse(resp);
            //++++++++++++++++++
            //printRequestParameters(req, System.out);
            //++++++++++++++++++++
            if (command.requiredDataIsAvailable(req)) {
                command.execute(req, resp);

                if(!(command instanceof DisconnectFromServer)){
                    //build the response
                    Collection<RealtimeRoom> jamRooms = DbUtils.loadRooms();
                    Collection<NinjamServer> ninjamServers = NinjamServers.getServers(ipToCountryResolver);
                    //++++++++++++++++++
                    Peer connectedPeer = AbstractCommand.getConnectedPeer(req);
                    resp.getWriter().print(VsJsonUtils.getJsonToResponse(jamRooms, ninjamServers, connectedPeer));
                }
            }
        } catch (Exception e) {
            throw new RuntimeException("Error executing " + command.getClass().getName(), e);
        }
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    private void printRequestParameters(HttpServletRequest req, PrintStream out) {
        out.println("");
        out.println("parametros recebidos:");
        Set keys = req.getParameterMap().keySet();
        for (Object key : keys) {
            out.println(key + " => " + req.getParameter(key.toString()));
        }
        out.println("");
        out.println("");
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    private void configureResponse(HttpServletResponse resp) {
        resp.setContentType("text/html; charset=utf-8");
        resp.setHeader("Cache-Control", "private, no-store, no-cache, must-revalidate");
        resp.setHeader("Pragma", "no-cache");
        resp.setDateHeader("Expires", 0);
        //resp.setHeader("Content-Encoding" , "compress");
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    @Override
//    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException {
//        try {
//            PrintWriter out = resp.getWriter();
//            Collection<NinjaMServer> servers = NinjamServers.getServers(ipToCountryResolver);
//            out.println("<html><body><ul>");
//            for (NinjaMServer server : servers) {
//                out.println("<li>");
//                    out.println(server.getHostName() + ": " + server.getPort());
//                    out.println("<ol>");
//                    for (NinjaMUser user : server.getUsers()) {
//                        out.println("<li>");
//                        out.println(user.getName() + " " + user.getIp() + " " + user.getCountryCode());
//                        out.println("</li>");
//                    }
//                    out.println("</ol>");
//                out.println("</li>");
//            }
//            out.println("</ul></body></html>");
//        } catch (Exception e) {
//            throw new ServletException(e);
//        }
//    }

}
