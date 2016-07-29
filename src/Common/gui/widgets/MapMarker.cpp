#include "MapMarker.h"

MapMarker::MapMarker(const QString &playerName, const QString &countryName, const QPointF &latLong, const QImage &flag) :
    name(playerName),
    countryName(countryName),
    latLong(latLong),
    flag(flag)
{

}

QString MapMarker::getText(bool showCountryDetails) const
{
    QString text = name;
    if (showCountryDetails)
        text += " (" + countryName + ")";
    return text;
}
