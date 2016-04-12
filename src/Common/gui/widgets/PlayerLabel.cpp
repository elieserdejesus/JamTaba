#include "PlayerLabel.h"
#include "geo/IpToLocationResolver.h"
#include "loginserver/LoginService.h"

PlayerLabel::PlayerLabel(QWidget* parent, const Login::UserInfo &userInfo, const Geo::Location &location)
    : QLabel(parent),
      userInfo(userInfo)
{
    setTextFormat(Qt::RichText);
    updateText(location);
}

void PlayerLabel::updateText(const Geo::Location &location)
{
    QString imgHTML = getImgTag(location);
    QString countryNameHTML = getCountryNameTag(location);
    QString userName = userInfo.getName();

    setText(imgHTML + userName + countryNameHTML);
}

void PlayerLabel::setLocation(const Geo::Location &newLocation)
{
    updateText(newLocation);
}

QString PlayerLabel::getCountryNameTag(const Geo::Location &location)
{
    if (!location.isUnknown())
        return " <i>(" + location.getCountryName() + ")</i>";

    return "";
}

QString PlayerLabel::getToolTipText(const Geo::Location &location, const QString &userName)
{
    if (location.isUnknown()) {
        return tr("%1  location is not available at moment!").arg(userName);
    }

    return "";
}

QString PlayerLabel::getImgTag(const Geo::Location &location)
{
    if (!location.isUnknown()) {
        QString countryCode = location.getCountryCode().toLower();
        return "<img src=:/flags/flags/" + countryCode + ".png> ";
    }

    return "<img src=:/images/warning.png> ";
}
