package masl.channel;

import java.io.IOException;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.appengine.api.channel.ChannelMessage;
import com.google.appengine.api.channel.ChannelService;
import com.google.appengine.api.channel.ChannelServiceFactory;

@SuppressWarnings("serial")
public class ChatServlet extends HttpServlet {
  @Override
  public void doPost(HttpServletRequest req, HttpServletResponse resp) throws IOException {
    String channelKey = req.getParameter("channelKey");
    String message = req.getParameter("message");
    
    //Send a message based on the 'channelKey' any channel with this key will receive the message
    ChannelService channelService = ChannelServiceFactory.getChannelService();
    
	channelService.sendMessage(new ChannelMessage(channelKey, message));
  }
}