package masl.channelAPI;

public interface ChannelService {
        /**
         * Set this to a function called when the socket is ready to receive messages.
         */
		void onOpen();
		
		/**
		 * Set this to a function called when the socket receives a message. 
		 * The function is passed one parameter: a message object. 
		 * The data field of this object is the string passed to the send_message method on the server.
		 * @param message
		 */
        void onMessage(String message);
        
        /**
         * Close the socket. The socket cannot be used again after calling close; the server must create a new socket.
         */
        void onClose();
        
        /**
         * Set this property to a function that called when the socket is closed. When the socket is closed, 
         * it cannot be reopened. Use the open() method on a goog.appengine.Channel object to create a new socket.
         * @param errorCode
         * @param description
         */
        void onError(Integer errorCode, String description);
};
