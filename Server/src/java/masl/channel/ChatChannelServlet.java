package masl.channel;

import com.google.appengine.api.channel.ChannelService;
import com.google.appengine.api.channel.ChannelServiceFactory;

import java.io.IOException;
import javax.servlet.http.*;


@SuppressWarnings("serial")
public class ChatChannelServlet extends HttpServlet {
	private static ChannelService channelService = ChannelServiceFactory.getChannelService();

	@Override
	public void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws IOException {
		String channelKey = req.getParameter("c");

		// Create a Channel using the 'channelKey' we received from the client
		String token = channelService.createChannel(channelKey);

		// Send the client the 'token' + the 'channelKey' this way the client
		// can start using the new channel
		resp.setContentType("text/html");
		StringBuffer sb = new StringBuffer();
		sb.append("{\"channelKey\":\"");
		sb.append(channelKey);
		sb.append("\",\"token\":\"");
		sb.append(token);
		sb.append("\"}");
		resp.getWriter().write(sb.toString());
	}
}