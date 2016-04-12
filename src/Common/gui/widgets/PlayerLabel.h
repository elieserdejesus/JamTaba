#ifndef _PLAYER_LABEL_
#define _PLAYER_LABEL_

#include <QLabel>

namespace Login {
class UserInfo;
}

namespace Geo {
class Location;
}

class PlayerLabel : public QLabel
{
    Q_OBJECT

public:
    PlayerLabel(QWidget* parent, const Login::UserInfo &userInfo, const Geo::Location &location);
    inline QString getUserIP() const { return userIP; }
    void setLocation(const Geo::Location &newLocation);
private:
    static QString getImgTag(const Geo::Location &location);
    static QString getToolTipText(const Geo::Location &location, const QString &userName);
    static QString getCountryNameTag(const Geo::Location &location);
    QString userIP;
};

#endif
