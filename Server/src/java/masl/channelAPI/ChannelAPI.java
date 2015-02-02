package masl.channelAPI;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.io.IOUtils;
import org.apache.commons.lang.RandomStringUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.utils.URLEncodedUtils;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.json.JSONException;
import org.json.JSONObject;

public class ChannelAPI {

    private enum ReadyState {

        CONNECTING, OPEN, CLOSING, CLOSED
    };

    private String BASE_URL = "http://localhost:8888"; //Defaults to LocalHost
    private final String CHANNEL_URL = "/_ah/channel/";
    private final String PROD_TALK_URL = "https://talkgadget.google.com/talkgadget/";
    private String channelId = null;
    private String applicationKey = null;
    private String clientId = null;
    private Integer requestId = 0;
    private String sessionId = null;
    private String SID = null;
    private long messageId = 1;
    private ChannelService channelListener = new ChannelListener();
    private ReadyState readyState = ReadyState.CLOSED;
    private Integer TIMEOUT_MS = 500;
    private HttpClient httpClient = new DefaultHttpClient();
    private Thread thPoll = null;

    /**
     * Default Constructor
     */
    public ChannelAPI() {
        this.clientId = null;
        this.channelId = null;
        this.requestId = null;
        this.sessionId = null;
        this.requestId = 0;
        this.messageId = 1;
        this.applicationKey = null;
    }

    /**
     * Create A Channel, Using URL, ChannelKey and a ChannelService
     *
     * @param URL - Server Location - http://localhost:8888
     * @param channelKey - Unique Identifier for channel groups, server uses
     * this to push data to clients, can have multiple clients on the same key,
     * but only 1 channel per client
     * @param channelService - An Implementation of the ChannelService class,
     * this is where the function methods will get called when the server pushes
     * data
     * @throws IOException JSON Related
     * @throws ClientProtocolException Connection Related
     */
    public ChannelAPI(String URL, String channelKey, ChannelService channelService) throws IOException, ClientProtocolException {

        this.clientId = null;
        this.BASE_URL = URL;
        this.requestId = 0;
        this.messageId = 1;
        this.channelId = createChannel(channelKey);
        this.applicationKey = channelKey;

        if (channelListener != null) {
            this.channelListener = channelService;
        }
    }

    /**
     * Ability to join an existing Channel with a full channel token, URL, and
     * ChannelService
     *
     * @param URL - Server Location - http://localhost:8888
     * @param token - Unique token returned by the App-Engine server
     * implementation from a previously created channel
     * @param channelService - An Implementation of the ChannelService class,
     * this is where the function methods will get called when the server pushes
     * data
     */
    public void joinChannel(String URL, String token, ChannelService channelService) {
        this.clientId = null;
        this.BASE_URL = URL;
        this.channelId = token;

        this.applicationKey = this.channelId.substring(this.channelId.lastIndexOf("-") + 1);
        if (channelListener != null) {
            this.channelListener = channelService;
        }
    }

    /**
     * Create a Channel on the Server and return the channelID + Key
     *
     * @param key
     * @return String: Channel 'Token/ID'
     * @throws IOException
     * @throws ClientProtocolException
     */
    private String createChannel(String key) throws IOException, ClientProtocolException {
        String token = "";
        HttpClient staticClient = new DefaultHttpClient();
        HttpGet httpGet = new HttpGet(BASE_URL + "/token?c=" + key);
        try {
            XHR xhr = new XHR(staticClient.execute(httpGet));
            System.out.println(xhr.getResponseText());
            JSONObject json = new JSONObject(xhr.getResponseText());
            token = json.getString("token");
        } catch (JSONException e) {
            System.out.println("Error: Parsing JSON");
        }
        return token;
    }

    /**
     * Connect to the Channel Decides to use either Production Mode /
     * Development Mode based on "localhost" being found in the BASE_URL
     *
     * @throws IOException, ChannelException
     */
    public void open() throws IOException, ChannelException {
        this.readyState = ReadyState.CONNECTING;
        if (this.BASE_URL.contains("localhost")) { //Local Development Mode
            connect(sendGet(getUrl("connect")));
        } else { //Production - AppEngine Mode
            initialize();
            fetchSid();
            connect();
            longPoll();
        }
    }

    /**
     * Sets up the initial connection, passes in the token
     */
    private void initialize() throws ChannelException {

        JSONObject xpc = new JSONObject();
        try {
            xpc.put("cn", RandomStringUtils.random(10, true, false));
            xpc.put("tp", "null");
            xpc.put("lpu", this.PROD_TALK_URL + "xpc_blank");
            xpc.put("ppu", this.BASE_URL + this.CHANNEL_URL + "xpc_blank");

        } catch (JSONException e1) {

        }

        List<NameValuePair> params = new ArrayList<NameValuePair>();
        params.add(new BasicNameValuePair("token", this.channelId));
        params.add(new BasicNameValuePair("xpc", xpc.toString()));

        String initUri = this.PROD_TALK_URL + "d?" + URLEncodedUtils.format(params, "UTF-8");

        HttpGet httpGet = new HttpGet(initUri);
        try {
            HttpResponse resp = this.httpClient.execute(httpGet);
            if (resp.getStatusLine().getStatusCode() > 299) {
                throw new ChannelException("Initialize failed: " + resp.getStatusLine());
            }

            String html = IOUtils.toString(resp.getEntity().getContent(), "UTF-8");
            consume(resp.getEntity());

            Pattern p = Pattern.compile("chat\\.WcsDataClient\\(([^\\)]+)\\)",
                    Pattern.CASE_INSENSITIVE | Pattern.MULTILINE);
            Matcher m = p.matcher(html);
            if (m.find()) {
                String fields = m.group(1);
                p = Pattern.compile("\"([^\"]*?)\"[\\s,]*", Pattern.MULTILINE);
                m = p.matcher(fields);

                for (int i = 0; i < 7; i++) {
                    if (!m.find()) {
                        throw new ChannelException("Expected iteration #" + i + " to find something.");
                    }
                    if (i == 2) {
                        this.clientId = m.group(1);
                    } else if (i == 3) {
                        this.sessionId = m.group(1);
                    } else if (i == 6) {
                        if (!this.channelId.equals(m.group(1))) {
                            throw new ChannelException("Tokens do not match!");
                        }
                    }
                }
            }
        } catch (IOException e) {
            throw new ChannelException(e);
        }
    }

    /**
     * Fetches and parses the SID, which is a kind of session ID.
     */
    private void fetchSid() throws ChannelException {

        String uri = getBindString(new BasicNameValuePair("CVER", "1"));

        HttpPost httpPost = new HttpPost(uri);

        List<NameValuePair> data = new ArrayList<NameValuePair>();
        data.add(new BasicNameValuePair("count", "0"));
        try {
            httpPost.setEntity(new UrlEncodedFormEntity(data));
        } catch (UnsupportedEncodingException e) {

        }

        TalkMessageParser parser = null;
        try {
            HttpResponse resp = this.httpClient.execute(httpPost);
            parser = new TalkMessageParser(resp);
            TalkMessage msg = parser.getMessage();

            TalkMessage.TalkMessageEntry entry = msg.getEntries().get(0);
            entry = entry.getMessageValue().getEntries().get(1);
            List<TalkMessage.TalkMessageEntry> entries = entry.getMessageValue().getEntries();
            if (!entries.get(0).getStringValue().equals("c")) {
                throw new InvalidMessageException("Expected first value to be 'c', found: "
                        + entries.get(0).getStringValue());
            }

            this.SID = entries.get(1).getStringValue();
        } catch (ClientProtocolException e) {
            throw new ChannelException(e);
        } catch (IOException e) {
            throw new ChannelException(e);
        } catch (InvalidMessageException e) {
            throw new ChannelException(e);
        } finally {
            if (parser != null) {
                parser.close();
            }
        }
    }

    /**
     * We need to make this "connect" request to set up the binding.
     */
    private void connect() throws ChannelException {
        String uri = getBindString(new BasicNameValuePair("AID", Long.toString(this.messageId)),
                new BasicNameValuePair("CVER", "1"));

        List<NameValuePair> params = new ArrayList<NameValuePair>();
        params.add(new BasicNameValuePair("count", "1"));
        params.add(new BasicNameValuePair("ofs", "0"));
        params.add(new BasicNameValuePair("req0_m", "[\"connect-add-client\"]"));
        params.add(new BasicNameValuePair("req0_c", this.clientId));
        params.add(new BasicNameValuePair("req0__sc", "c"));

        HttpEntity entity = null;
        try {
            entity = new UrlEncodedFormEntity(params);
        } catch (UnsupportedEncodingException e) {

        }

        HttpPost httpPost = new HttpPost(uri);
        httpPost.setEntity(entity);
        try {
            HttpResponse resp = this.httpClient.execute(httpPost);
            consume(resp.getEntity());
        } catch (ClientProtocolException e) {
            throw new ChannelException(e);
        } catch (IOException e) {
            throw new ChannelException(e);
        }

        this.channelListener.onOpen();
    }

    /**
     * Gets the URL to the "/bind" endpoint.
     */
    private String getBindString(NameValuePair... extraParams) {
        List<NameValuePair> params = new ArrayList<NameValuePair>();
        params.add(new BasicNameValuePair("token", this.channelId));
        params.add(new BasicNameValuePair("gsessionid", this.sessionId));
        params.add(new BasicNameValuePair("clid", this.clientId));
        params.add(new BasicNameValuePair("prop", "data"));
        params.add(new BasicNameValuePair("zx", RandomStringUtils.random(12, true, false)));
        params.add(new BasicNameValuePair("t", "1"));
        if (this.SID != null && this.SID != "") {
            params.add(new BasicNameValuePair("SID", this.SID));
        }
        for (int i = 0; i < extraParams.length; i++) {
            params.add(extraParams[i]);
        }

        params.add(new BasicNameValuePair("RID", Integer.toString(this.requestId)));
        this.requestId++;

        return this.PROD_TALK_URL + "dch/bind?VER=8&" + URLEncodedUtils.format(params, "UTF-8");
    }

    /**
     * Grabbing Data "Production" Path
     */
    private void longPoll() {
        if (this.thPoll != null) {
            return;
        }

        this.thPoll = new Thread(new Runnable() {
            private TalkMessageParser repoll() {
                String bindString = getBindString(new BasicNameValuePair("CI", "0"),
                        new BasicNameValuePair("AID", Long.toString(messageId)),
                        new BasicNameValuePair("TYPE", "xmlhttp"),
                        new BasicNameValuePair("RID", "rpc"));

                HttpGet httpGet = new HttpGet(bindString);
                HttpResponse resp = null;
                try {
                    resp = httpClient.execute(httpGet);
                    return new TalkMessageParser(resp);
                } catch (ClientProtocolException e) {
                } catch (IOException e) {
                } catch (ChannelException e) {
                }

                return null;
            }

            @Override
            public void run() {
                TalkMessageParser parser = null;
                while (readyState.equals(ReadyState.OPEN)) {
                    if (parser == null) {
                        parser = repoll();
                        if (parser == null) {
                            try {
                                Thread.sleep(2500);
                            } catch (InterruptedException e) {
                            }
                        }
                    }
                    try {
                        TalkMessage msg = parser.getMessage();
                        if (msg == null) {
                            parser.close();
                            parser = null;
                        } else {
                            handleMessage(msg);
                        }
                    } catch (ChannelException e) {
                        channelListener.onError(500, e.getMessage());

                        return;
                    }
                }
            }
        });

        readyState = ReadyState.OPEN;
        this.thPoll.start();
    }

    /**
     * Used each time we receive a message on the Production side, filters
     * garbage data from actual data
     */
    private void handleMessage(TalkMessage msg) {
        try {
            List<TalkMessage.TalkMessageEntry> entries = msg.getEntries();
            msg = entries.get(0).getMessageValue();

            entries = msg.getEntries();
            this.messageId = entries.get(0).getNumberValue();

            msg = entries.get(1).getMessageValue();
            entries = msg.getEntries();

            if (entries.get(0).getKind() == TalkMessage.MessageEntryKind.ME_STRING && entries.get(0).getStringValue().equals("c")) {
                msg = entries.get(1).getMessageValue();
                entries = msg.getEntries();

                String thisSessionID = entries.get(0).getStringValue();
                if (!thisSessionID.equals(this.sessionId)) {
                    this.sessionId = thisSessionID;
                }

                msg = entries.get(1).getMessageValue();
                entries = msg.getEntries();

                if (entries.get(0).getStringValue().equalsIgnoreCase("ae")) {
                    String msgValue = entries.get(1).getStringValue();
                    this.channelListener.onMessage(msgValue);
                }
            }
        } catch (InvalidMessageException e) {
            e.printStackTrace();
        }
    }

    /**
     * Helper class for parsing talk messages. Again, this protocol has been
     * reverse-engineered so it doesn't have a lot of error checking and is
     * generally fairly lenient.
     */
    private static class TalkMessageParser {

        private HttpResponse mHttpResponse;
        private BufferedReader mReader;

        public TalkMessageParser(HttpResponse resp) throws ChannelException {
            try {
                mHttpResponse = resp;
                InputStream ins = resp.getEntity().getContent();
                mReader = new BufferedReader(new InputStreamReader(ins));
            } catch (IllegalStateException e) {
                throw new ChannelException(e);
            } catch (IOException e) {
                throw new ChannelException(e);
            }
        }

        public TalkMessage getMessage() throws ChannelException {
            String submission = readSubmission();
            if (submission == null) {
                return null;
            }

            TalkMessage msg = new TalkMessage();

            try {
                msg.parse(new BufferedReader(new StringReader(submission)));
            } catch (InvalidMessageException e) {
                throw new ChannelException(e);
            }

            return msg;
        }

        public void close() {
            try {
                mReader.close();
            } catch (IOException e) {
            }

            if (mHttpResponse != null) {
                consume(mHttpResponse.getEntity());
            }
        }

        private String readSubmission() throws ChannelException {
            try {
                String line = mReader.readLine();
                if (line == null) {
                    return null;
                }

                int numChars = Integer.parseInt(line);
                char[] chars = new char[numChars];
                int total = 0;
                while (total < numChars) {
                    int numRead = mReader.read(chars, total, numChars - total);
                    total += numRead;
                }
                return new String(chars);
            } catch (IOException e) {
                throw new ChannelException(e);
            } catch (NumberFormatException e) {
                throw new ChannelException("Submission was not in expected format.", e);
            }
        }
    }

    /**
     * A "talk" message is a data structure containing lists of strings,
     * integers and (recursive) talk messages.
     */
    private static class TalkMessage {

        public enum MessageEntryKind {

            ME_STRING,
            ME_NUMBER,
            ME_EMPTY,
            ME_TALKMESSAGE
        }

        private ArrayList<TalkMessageEntry> mEntries;

        public TalkMessage() {
            mEntries = new ArrayList<TalkMessageEntry>();
        }

        private TalkMessage(ArrayList<TalkMessageEntry> entries) {
            mEntries = entries;
        }

        public List<TalkMessageEntry> getEntries() {
            return mEntries;
        }

        public void parse(BufferedReader reader) throws InvalidMessageException {
            try {
                if (skipWhitespace(reader) != '[') {
                    throw new InvalidMessageException("Expected initial [");
                }

                mEntries = parseMessage(reader);
            } catch (IOException e) {
                throw new InvalidMessageException(e);
            }
        }

        @Override
        public String toString() {
            String str = "[";
            for (TalkMessageEntry entry : mEntries) {
                if (str != "[") {
                    str += ",";
                }
                str += entry.toString();
            }
            return str + "]";
        }

        private static ArrayList<TalkMessageEntry> parseMessage(BufferedReader reader)
                throws InvalidMessageException, IOException {
            ArrayList<TalkMessageEntry> entries = new ArrayList<TalkMessageEntry>();

            int ch = skipWhitespace(reader);
            while (ch != ']') {
                if (ch < 0) {
                    throw new InvalidMessageException("Unexpected end-of-message.");
                }

                if (ch == '[') {
                    ArrayList<TalkMessageEntry> childEntries = parseMessage(reader);
                    entries.add(new TalkMessageEntry(MessageEntryKind.ME_TALKMESSAGE,
                            new TalkMessage(childEntries)));
                } else if (ch == '\"' || ch == '\'') {
                    String stringValue = parseStringValue(reader, (char) ch);
                    entries.add(new TalkMessageEntry(MessageEntryKind.ME_STRING, stringValue));
                } else if (ch == ',') {
                    // blank entry
                    entries.add(new TalkMessageEntry(MessageEntryKind.ME_EMPTY, null));
                } else {
                    // we assume it's a number
                    long numValue = parseNumberValue(reader, (char) ch);
                    entries.add(new TalkMessageEntry(MessageEntryKind.ME_NUMBER, numValue));
                }

                //We expect a comma next, or the end of the message
                if (ch != ',') {
                    ch = skipWhitespace(reader);
                }

                if (ch != ',' && ch != ']') {
                    throw new InvalidMessageException("Expected , or ], found " + ((char) ch));
                } else if (ch == ',') {
                    ch = skipWhitespace(reader);
                }
            }

            return entries;
        }

        private static String parseStringValue(BufferedReader reader, char quote)
                throws IOException {
            String str = "";
            for (int ch = reader.read(); ch > 0 && ch != quote; ch = reader.read()) {
                if (ch == '\\') {
                    ch = reader.read();
                    if (ch < 0) {
                        break;
                    }
                }
                str += (char) ch;
            }

            return str;
        }

        private static long parseNumberValue(BufferedReader reader, char firstChar)
                throws IOException {
            String str = "";
            for (int ch = firstChar; ch > 0 && Character.isDigit(ch); ch = reader.read()) {
                str += (char) ch;
                reader.mark(1);
            }
            reader.reset();

            return Long.parseLong(str);
        }

        private static int skipWhitespace(BufferedReader reader) throws IOException {
            int ch = reader.read();
            while (ch >= 0) {
                if (!Character.isWhitespace(ch)) {
                    return ch;
                }
                ch = reader.read();
            }
            return -1;
        }

        public static class TalkMessageEntry {

            MessageEntryKind mKind;
            Object mValue;

            public TalkMessageEntry(MessageEntryKind kind, Object value) {
                mKind = kind;
                mValue = value;
            }

            public MessageEntryKind getKind() {
                return mKind;
            }

            public String getStringValue() throws InvalidMessageException {
                if (mKind == MessageEntryKind.ME_STRING) {
                    return (String) mValue;
                } else {
                    throw new InvalidMessageException("String value expected, found: " + mKind + " (" + mValue + ")");
                }
            }

            public long getNumberValue() throws InvalidMessageException {
                if (mKind == MessageEntryKind.ME_NUMBER) {
                    return (Long) mValue;
                } else {
                    throw new InvalidMessageException("Number value expected, found: " + mKind + " (" + mValue + ")");
                }
            }

            public TalkMessage getMessageValue() throws InvalidMessageException {
                if (mKind == MessageEntryKind.ME_TALKMESSAGE) {
                    return (TalkMessage) mValue;
                } else {
                    throw new InvalidMessageException("TalkMessage value expected, found: " + mKind + " (" + mValue + ")");
                }
            }

            @Override
            public String toString() {
                if (mKind == MessageEntryKind.ME_EMPTY) {
                    return "";
                } else if (mKind == MessageEntryKind.ME_STRING) {
                    return "\"" + mValue.toString() + "\"";
                } else {
                    return mValue.toString();
                }
            }
        }
    }

    /**
     * This exception will be thrown any time we have an issue parsing a talk
     * message. Probably this means they've changed the protocol on us.
     */
    public static class InvalidMessageException extends Exception {

        private static final long serialVersionUID = 1L;

        public InvalidMessageException(String msg) {
            super(msg);
        }

        public InvalidMessageException(Throwable e) {
            super(e);
        }
    }

    /**
     * Close the Channel, Channel is gone from the server now, a new Channel is
     * required if you wish to reconnect, you can't re-use the old one
     *
     * @throws IOException
     */
    public void close() throws IOException {
        this.readyState = ReadyState.CLOSING;
        disconnect(sendGet(getUrl("disconnect")));
    }

    /**
     * A helper method that consumes an HttpEntity so that the HttpClient can be
     * reused. If you're not planning to run on Android, you can use the
     * non-deprecated EntityUtils.consume() method instead.
     */
    @SuppressWarnings("deprecation")
    private static void consume(HttpEntity entity) {
        //Grab Everything
        try {
            if (entity != null) {
                entity.consumeContent();
            }
        } catch (IOException e) {
            //Don't Worry About
        }
    }

    /**
     * Development getUrl
     *
     * @param command
     * @return
     * @throws IOException
     */
    private String getUrl(String command) throws IOException {
        String url = BASE_URL + CHANNEL_URL + "dev?command=" + command + "&channel=";

        url += URLEncoder.encode(this.channelId, "UTF-8");
        if (this.clientId != null) {
            url += "&client=" + URLEncoder.encode(this.clientId, "UTF-8");
        }
        return url;
    }

    ;

    /**
     * If you successfully connect to the Channel, start pulling data.
     * If you fail to connect, the error is printed to the terminal and
     * the connection is closed.
     * @param xhr
     */
    private void connect(XHR xhr) {
        if (xhr.isSuccess()) {
            this.clientId = xhr.getResponseText();
            this.readyState = ReadyState.OPEN;
            this.channelListener.onOpen();
            poll();
        } else {
            this.readyState = ReadyState.CLOSING;
            this.channelListener.onError(xhr.getStatus(), xhr.getStatusText());
            this.readyState = ReadyState.CLOSED;
            this.channelListener.onClose();
        }
    }

    /**
     * Closing the channel
     *
     * @param xhr
     */
    private void disconnect(XHR xhr) {
        this.readyState = ReadyState.CLOSED;
        this.channelListener.onClose();
    }

    /**
     *
     * @param xhr
     */
    private void forwardMessage(XHR xhr) {
        if (xhr.isSuccess()) {
            String data = StringUtils.chomp(xhr.getResponseText());
            if (!StringUtils.isEmpty(data)) {
                this.channelListener.onMessage(data);
            }
            poll();
        } else {
            this.channelListener.onError(xhr.getStatus(), xhr.getStatusText());
        }
    }

    /**
     * Grabbing Data "DEV"
     */
    private void poll() {
        if (thPoll == null) {
            thPoll = new Thread(new Runnable() {

                @Override
                public void run() {
                    XHR xhr = null;
                    try {
                        Thread.sleep(TIMEOUT_MS);
                        xhr = sendGet(getUrl("poll"));
                        thPoll = null;
                        forwardMessage(xhr);
                    } catch (Exception e) {
                        thPoll = null;
                    }
                }

            });
            thPoll.start();
        }
    }

    /**
     * Send Message On, If there is an error notify the channelListener!
     *
     * @param xhr
     */
    private void forwardSendComplete(XHR xhr) {
        if (!xhr.isSuccess()) {
            this.channelListener.onError(xhr.getStatus(), xhr.getStatusText());
        }
    }

    /**
     * Used to send a message to the server
     *
     * @param message
     * @param urlPattern - where the server should look for the message. ex:
     * "/chat"
     * @return true
     * @throws IOException
     */
    public boolean send(String message, String urlPattern) throws IOException {
        if (this.readyState != ReadyState.OPEN) {
            return false;
        }
        String url = BASE_URL + urlPattern;
        List<NameValuePair> params = new ArrayList<NameValuePair>();
        params.add(new BasicNameValuePair("channelKey", this.applicationKey));
        params.add(new BasicNameValuePair("message", message));
        forwardSendComplete(sendPost(url, params));
        return true;
    }

    /**
     * Send an HTTPPOST, convenience function
     *
     * @param url
     * @param params
     * @return XHR, nice responses from httpRequests
     * @throws IOException
     */
    private XHR sendPost(String url, List<NameValuePair> params) throws IOException {
        HttpClient sendClient = new DefaultHttpClient();
        UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params, "UTF-8");
        HttpPost httpPost = new HttpPost(url);
        httpPost.setEntity(entity);
        return new XHR(sendClient.execute(httpPost));
    }

    /**
     * Send an HTTPGET, convenience function
     *
     * @param url
     * @return XHR, nice responses from httpRequests
     * @throws IOException
     */
    private XHR sendGet(String url) throws IOException {
        HttpGet httpGet = new HttpGet(url);
        return new XHR(httpClient.execute(httpGet));
    }

    /**
     * Set a new ChannelListener
     *
     * @param channelListener
     */
    public void setChannelListener(ChannelService channelListener) {
        if (channelListener != null) {
            this.channelListener = channelListener;
        }
    }

    /**
     * This exception is thrown in case of errors.
     */
    public static class ChannelException extends Exception {

        private static final long serialVersionUID = 1L;

        public ChannelException() {
        }

        public ChannelException(Throwable cause) {
            super(cause);
        }

        public ChannelException(String message) {
            super(message);
        }

        public ChannelException(String message, Throwable cause) {
            super(message, cause);
        }
    }
}
