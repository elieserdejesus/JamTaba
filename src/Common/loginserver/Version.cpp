#include "Version.h"
#include <QtDebug>

using namespace loginserver;

Version Version::fromString(const QString &string)
{
    QStringList versionParts = QString(string).replace(QRegExp("[^0-9\\.]"), "").split(QRegExp("\\."));
    if(versionParts.length() != 3){
        qWarning() << "version string with wrong format: " << string << "  versionParts: " << versionParts.length();
        return Version();
    }

    int major = versionParts[0].toInt();
    int minor = versionParts[1].toInt();
    int build = versionParts[2].toInt();

    return Version(major, minor, build);
}


bool Version::isNewerThan(const Version &other) const
{
    if(majorVersion > other.majorVersion){
        return true;
    }
    else{
        if(majorVersion == other.majorVersion){
            if(minorVersion > other.minorVersion){
                return true;
            }
            else{
                if(minorVersion == other.minorVersion){
                    return buildNumber > other.buildNumber;
                }
            }
        }
    }
    return false;
}

Version::Version()
{
    majorVersion = 0;
    minorVersion = 0;
    buildNumber = 0;
}

Version::Version(int major, int minor, int build)
{
    majorVersion = major;
    minorVersion = minor;
    buildNumber = build;
}

QString Version::toString() const
{
    return QString("Version{ major=%1, minor=%2, buildNumber=%3 }")
            .arg(majorVersion)
            .arg(minorVersion)
            .arg(buildNumber);
}

