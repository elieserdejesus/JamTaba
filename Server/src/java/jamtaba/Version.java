package jamtaba;

public class Version {
    private final int majorVersion;
    private final int minorVersion;
    private final int buildNumber;

    public static Version fromString(String string) {
        String[] versionParts = string.split(".");
        if(versionParts.length != 3){
            throw new IllegalArgumentException("version string with wrong format: " + string);
        }
        int major = Integer.parseInt(versionParts[0]);
        int minor = Integer.parseInt(versionParts[1]);
        int build = Integer.parseInt(versionParts[2]);
        return new Version(major, minor, build);
    }

    public boolean isCompatibleWith(Version other){
        return majorVersion == other.getMajorVersion() && minorVersion == other.getMinorVersion();
    }
    
    public boolean isNewer(Version other){
        if(majorVersion > other.getMajorVersion()){
            return true;
        }
        else{
            if(majorVersion == other.getMajorVersion()){
                if(minorVersion > other.getMinorVersion()){
                    return true;
                }
                else{
                    if(minorVersion == other.getMinorVersion()){
                        return buildNumber > other.getBuildNumber();
                    }
                }
            }
        }
        return false;
    }
    
    public int getBuildNumber() {
        return buildNumber;
    }

    public int getMajorVersion() {
        return majorVersion;
    }

    public int getMinorVersion() {
        return minorVersion;
    }

    private Version(int major, int minor, int build) {
        this.majorVersion = major;
        this.minorVersion = minor;
        this.buildNumber = build;
    }
    
    
    
}
