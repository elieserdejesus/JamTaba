package jamtaba;

import com.googlecode.objectify.ObjectifyService;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.util.*;
import javax.servlet.ServletException;
import javax.servlet.http.*;
import jamtaba.command.Command;
import jamtaba.command.CommandFactory;

public class ServletPrincipal extends HttpServlet {

    private static final long serialVersionUID = 1L;
    private static final int STATIC_REAL_TIME_ROOMS = 5;
    //private static final DebugLog DebugLog = new DebugLog();
    //private static final DaoFactory daoFactory = new JdoDaoFactory();// new MemoryDaoFactory();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    @Override
    public synchronized void init() throws ServletException {
        super.init();

        ObjectifyService.register(Peer.class);
        ObjectifyService.register(RealtimeRoom.class);

        //+++++++++ CREATE THE STATIC ROOMS, ONE FOR EACH LOADED STYLE ++++
        //++++++++++++
        Collection<RealtimeRoom> rooms = DbUtils.loadRooms();
        if (rooms.isEmpty()) {
            List<RealtimeRoom> jamRooms = new ArrayList<RealtimeRoom>(STATIC_REAL_TIME_ROOMS);
            for (int i = 0; i < STATIC_REAL_TIME_ROOMS; i++) {
                jamRooms.add( new RealtimeRoom("Room " + (i + 1), true));//a sala é estática
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
//       Room roomTeste = new Room("testando bug duplicação", 0, 2, Collections.EMPTY_LIST, false);
//       DbUtils.save(roomTeste);
//       Room loadedRoom = DbUtils.getRoomByID(roomTeste.getId());
//       Peer p = new Peer("", "", "peer do bug", "", "", 5);
//       DbUtils.save(p);
//       loadedRoom.addPeer(p);
//       DbUtils.save(loadedRoom);
//       DbUtils.save(p);

        //doPost(req, resp);
       //http://localhost:8080/vs?cmd=CONNECT&publicIp=teste&publicPort=34&audioLatency=5&userName=elieser
       //http://localhost:8080/vs?cmd=NEW&roomName=teste&roomStyleCode=0&maxUsers=2
        
//        try{
//            Room waitRoom = DbUtils.getWaitingRoom();
//            Peer p = new Peer("ip", "porta", "teste", "sc", "br", 5);
//            DbUtils.save(p, false);
//            waitRoom.addPeer(p);
//            DbUtils.save(waitRoom, false);
//            
//            showData(resp.getWriter());
//        }
//        catch(IOException e){
//            throw new ServletException(e);
//        }
//}
    private void showData(PrintWriter out) {
        //showJamRooms(out);
        //out.print("<hr/>");
        //showPeers(out);
    }

    private void showPeers(PrintWriter out) {
//
//        IPeerDAO peerDAO = daoFactory.getPeerDao();
//        Collection<Peer> peers = peerDAO.getAll();
//        out.print("<html>");
//        out.print("<head>");
//        out.print("<style>"
//                + "table tbody tr:nth-child(odd) {"
//                + "background:lightgray;"
//                + "}"
//                + "</style>");
//        out.print("</head>");
//        out.print("<body>");
//        out.print("<table border=1 cellpadding=4>");
//        out.print("<tr>");
//        out.print("<th> ID");
//        out.print("<th> country");
//        out.print("<th> region");
//        out.print("<th> userName");
//        out.print("<th> roomID");
//        out.print("</tr>");
//        for (Peer peer : peers) {
//            out.print("<tr>");
//            out.print("<td>" + peer.getId() + "</td>");
//            out.print("<td>" + peer.getCountry() + "</td>");
//            out.print("<td>" + peer.getRegion() + "</td>");
//            out.print("<td>" + peer.getUserName() + "</td>");
//            out.print("<td>" + peer.getRoom() + "</td>");
//            out.print("</tr>");
//        }
//        out.print("</table>");
//        out.print("</doby>");
//        out.print("</html>");
    }

    private void showJamRooms(PrintWriter out) {
        Collection<RealtimeRoom> rooms = DbUtils.loadRooms();
        out.print("<html>");
        out.print("<head>");
        out.print("<style>"
                + "table tbody tr:nth-child(odd) {"
                + "background:lightgray;"
                + "}"
                + "</style>");
        out.print("</head>");
        out.print("<body>");
        out.print("<table border=1 cellpadding=4>");
        out.print("<tr>");
        out.print("<th> ID");
        out.print("<th> Name");
        out.print("<th> Style");
        out.print("<th> Max Users");
        out.print("<th> Peers");
        out.print("<th> ownerID");
        out.print("<th> empty");
        out.print("<th> full");
        out.print("<th> static");
        out.print("</tr>");
        for (RealtimeRoom jamRoom : rooms) {
            out.print("<tr>");
            out.print("<td>" + jamRoom.getId() + "</td>");
            out.print("<td>" + jamRoom.getName() + "</td>");
            //out.print("<td>" + jamRoom.getStyleCode() + "</td>");
            out.print("<td>" + jamRoom.getMaxUsers() + "</td>");
            out.print("<td>" + jamRoom.getPeers().size() + "</td>");
            //out.print("<td>" + ((jamRoom.hasOwner()) ? (jamRoom.getOwner().getUserName()) : ("sem owner"))+ "</td>");
            out.print("<td>" + jamRoom.isEmpty() + "</td>");
            out.print("<td>" + jamRoom.isFull() + "</td>");
            out.print("<td>" + jamRoom.isStatic() + "</td>");
            out.print("</tr>");
        }
        out.print("</table>");
        out.print("</doby>");
        out.print("</html>");
    }
}
