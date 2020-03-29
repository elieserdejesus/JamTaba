#ifndef _JAMTABA_VERSION_
#define _JAMTABA_VERSION_

#include <QStringList>

namespace loginserver {

class Version {

private:
    int majorVersion;
    int minorVersion;
    int buildNumber;

    Version();
    Version(int major, int minor, int build);

public:
    static Version fromString(const QString &string);
    bool isNewerThan(const Version &other) const;
    QString toString() const;

};

} // namespace

#endif
