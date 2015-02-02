package jamtaba;

import static jamtaba.command.RefreshCommand.MAX_TIME_WITHOUT_UPDATES;

/**
 * @author zeh
 */
public class PeerCleaner {

    private static long lastInactivePeerCleaning = 0;
    private static final long INACTIVE_PEER_CLEANING_PERIOD = 30*1000;

    public static void cleanInactivePeers() {
        if (canDeleteInactivePeers()) {
            DbUtils.deleteInactivePeers(MAX_TIME_WITHOUT_UPDATES);
            lastInactivePeerCleaning = System.currentTimeMillis();
        }
    }

    private static boolean canDeleteInactivePeers() {
        return System.currentTimeMillis() - lastInactivePeerCleaning >= INACTIVE_PEER_CLEANING_PERIOD;
    }
}
