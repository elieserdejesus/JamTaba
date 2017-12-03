#ifndef _MAP_WIDGET_H
#define _MAP_WIDGET_H

#include <QWidget>
#include <QMap>
#include "MapMarker.h"
#include <QMouseEvent>

struct MapMarkerComparator;

class MapWidget: public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor markerTextBackgroundColor MEMBER markerTextBackgroundColor WRITE setMarkerTextBackgroundColor)
    Q_PROPERTY(QColor markerColor MEMBER markerColor WRITE setMarkerColor)
    Q_PROPERTY(QColor markerTextColor MEMBER markerTextColor WRITE setMarkerTextColor)
    Q_PROPERTY(QColor markerLineConnectorColor MEMBER markerLineConnectorColor  WRITE setMarkerLineConnectorColor)

    friend struct MapMarkerComparator;

public:
    explicit MapWidget(QWidget *parent = 0);
    void setMarkers(const QList<MapMarker> &markers);
    static void setTilesDir(const QString &newDir);
    static void setNightMode(bool useNightMode);
    void setBlurMode(bool blurEnabled);

    void setMarkerTextBackgroundColor(const QColor &color);
    void setMarkerTextColor(const QColor &color);
    void setMarkerColor(const QColor &color);
    void setMarkerLineConnectorColor(const QColor &color);

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *event) override;
    QSize minimumSizeHint() const override;
    bool eventFilter(QObject *, QEvent *) override;

    void changeEvent(QEvent *) override;
private slots:
    void loadTiles();

private:
    static const int ZOOM;
    qreal latitude;
    qreal longitude;

    QPoint offset;
    QRect tilesRect;
    static QHash<QPoint, QPixmap> tilePixmaps; // tiles cache

    static bool usingNightMode;

    QList<MapMarker> markers;

    void invalidate();
    QRect tileRect(const QPoint &tp) const;

    void drawMapTiles(QPainter &p, const QRect &rect);
    void drawPlayersMarkers(QPainter &p);
    void drawMarker(const MapMarker &marker, QPainter &p, const QPointF &markerPosition, const QPointF &rectPosition);

    QColor markerTextBackgroundColor;
    QColor markerColor;
    QColor markerTextColor;
    QColor markerLineConnectorColor;

    void initializeCountryFont();

    QSizeF getMarkerSize(const MapMarker &marker) const;

    void setCenter(QPointF latLong);

    QPixmap loadTile(int zoomLevel, int x, int y);

    QPointF getCenterLatLong() const;

    QRectF computeMinimumRect(int ZOOM) const;

    QPointF getMarkerScreenCoordinate(const MapMarker &marker) const;

    QPointF getMinimumLatLongInMarkers() const;
    QPointF getMaximumLatLongInMarkers() const;

    struct Position
    {
        QPointF coords;
        int index;
        Position(const QPointF &coords, int index) : coords(coords), index(index){}
    };

    QList<MapWidget::Position> mapPositions;
    void updateMapPositionsCache();

    QList<MapWidget::Position> getEllipsePositions(int markersHeight, const QRectF &ellipseRect) const;
    Position findBestEllipsePositionForMarker(const MapMarker &marker, const QList<MapMarker> &markers, const QList<Position> &positions);
    QList<MapWidget::Position> getEmptyPositions(const QMap<int, QList<MapMarker>> markers, const QList<MapWidget::Position> &allPositions) const;
    bool rectIntersectsSomeMarker(const QRectF &rect, const QList<MapMarker> &markers) const;

    int getMaximumMarkerWidth();

    QFont countryFont;

    bool blurActivated;

    static QString TILES_DIR;
    static const qreal TEXT_MARGIM;
    static const int TILES_SIZE;
    static const int MARKER_POSITIONS;

};

#endif
