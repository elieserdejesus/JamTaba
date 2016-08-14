#ifndef _MAP_WIDGET_H
#define _MAP_WIDGET_H

#include <QWidget>
#include <QMap>
#include "MapMarker.h"

struct MapMarkerComparator;

class MapWidget: public QWidget
{
    Q_OBJECT

    friend struct MapMarkerComparator;

public:
    MapWidget(QWidget *parent = 0);
    void setMarkers(const QList<MapMarker> &markers);
    void setMarkersVisibility(bool showMarkers);
    static void setTilesDir(const QString &newDir);

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *event) override;
    QSize minimumSizeHint() const override;

private slots:
    void loadTiles();

private:
    int zoom;
    qreal latitude;
    qreal longitude;

    QPoint offset;
    QRect tilesRect;
    static QMap<int, QHash<QPoint, QPixmap>> tilePixmaps; // tiles cache, one QHash per zoom level

    QList<MapMarker> markers;

    bool showingMarkers;

    void invalidate();
    QRect tileRect(const QPoint &tp) const;

    void drawMapTiles(QPainter &p, const QRect &rect);
    void drawPlayersList(QPainter &p);
    void drawPlayersMarkers(QPainter &p, bool showCountryDetailsInMarkers);
    void drawMarker(const MapMarker &marker, QPainter &p, const QPointF &markerPosition, const QPointF &rectPosition, bool showCountryDetails) const;

    QPainterPath getMarkerPainterPath(const MapMarker &marker, const QPointF &markerPosition, const QPointF &rectPosition, bool showCountryDetails) const;
    QRectF getMarkerRect(const MapMarker &marker, const QPointF &anchor, bool showCountryDetails) const;

    void setCenter(QPointF latLong);

    QPixmap loadTile(int zoomLevel, int x, int y);

    QPointF getCenterLatLong() const;

    QRectF computeMinimumRect(int zoom) const;

    QPointF getMarkerScreenPosition(const MapMarker &marker) const;

    QPointF getMinimumLatLongInMarkers() const;
    QPointF getMaximumLatLongInMarkers() const;

    void drawMarkersRegion(QPainter &p, QList<MapMarker> &markers, qreal initialAngle, bool showCountryDetailsInMarkers, bool shiftRectsToLeft) const;

    int calculateBestZoomLevel() const;

    void autoAdjustZoomLevel();

    static QString TILES_DIR;
    static const qreal TEXT_MARGIM;
    static const int TILES_SIZE;
    static const int MARKER_POSITIONS;

};

#endif
