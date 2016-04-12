#ifndef _PLAYER_LABEL_
#define _PLAYER_LABEL_

#include <QLabel>
#include "loginserver/LoginService.h"

namespace Geo {
class Location;
}

class PlayerLabel : public QLabel
{
    Q_OBJECT

public:
    PlayerLabel(QWidget* parent, const Login::UserInfo &userInfo, const Geo::Location &location);
    inline QString getUserIP() const { return userInfo.getIp(); }
    void setLocation(const Geo::Location &newLocation);

private:
    static QString getImgTag(const Geo::Location &location);
    static QString getToolTipText(const Geo::Location &location, const QString &userName);
    static QString getCountryNameTag(const Geo::Location &location);
    void updateText(const Geo::Location &location);
    Login::UserInfo userInfo;
};

#endif
