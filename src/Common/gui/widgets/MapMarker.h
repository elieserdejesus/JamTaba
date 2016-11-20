#ifndef _MAP_MARKER_H
#define _MAP_MARKER_H

#include <QString>
#include <QPainterPath>
#include <QPointF>
#include <QImage>

class MapMarker
{

public:
    MapMarker(const QString &playerName, const QString &countryName, const QPointF &latLong, const QImage &flag);
    inline QPointF getLatLong() const { return latLong; }
    QString getPlayerName() const;
    inline const QImage &getFlag() const { return flag; }
    inline QString getCountryName() const { return countryName; }
    QString getText() const;

private:
    QString playerName;
    QString countryName;
    QPointF latLong;
    QImage flag;

};

#endif // _MAP_MARKER_H
