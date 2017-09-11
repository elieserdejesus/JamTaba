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
    QPointF getLatLong() const;
    QString getPlayerName() const;
    const QImage &getFlag() const;
    QString getCountryName() const;
    QString getText() const;

private:
    QString playerName;
    QString countryName;
    QPointF latLong;
    QImage flag;
};

inline QPointF MapMarker::getLatLong() const
{
    return latLong;
}

inline const QImage &MapMarker::getFlag() const
{
    return flag;
}

inline QString MapMarker::getCountryName() const
{
    return countryName;
}

#endif // _MAP_MARKER_H
