package jamtaba;

import com.googlecode.objectify.annotation.Cache;
import com.googlecode.objectify.annotation.Entity;
import com.googlecode.objectify.annotation.Id;

@Cache
@Entity
public class Version {
    
    @Id
    private long ID = 1;
    
    private int majorVersion;
    private int minorVersion;
    private int buildNumber;

    public static Version fromString(String string) {
        String[] versionParts = string.split("\\.");
        if(versionParts.length != 3){
            throw new IllegalArgumentException("version string with wrong format: " + string + "  versionParts: " + versionParts.length);
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

    public Version() {
        this.majorVersion = 0;
        this.minorVersion = 0;
        this.buildNumber = 0;
    }

    private Version(int major, int minor, int build) {
        this.majorVersion = major;
        this.minorVersion = minor;
        this.buildNumber = build;
    }
    
    public static void main(String args[]){
        Version v = Version.fromString("2.0.0");
        System.out.println(v);
    }

    @Override
    public String toString() {
        return "Version{" + "majorVersion=" + majorVersion + ", minorVersion=" + minorVersion + ", buildNumber=" + buildNumber + '}';
    }
    
}
