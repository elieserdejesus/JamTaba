#include "MapWidget.h"
#include <QtCore>
#include <QtWidgets>
#include <QDebug>
#include <cmath>
#include <QEvent>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QString MapWidget::TILES_DIR = ":/tiles/map/";
const int MapWidget::TILES_SIZE = 256; // tile size in pixels
const qreal MapWidget::TEXT_MARGIM = 3;
const int MapWidget::MARKER_POSITIONS = 8;
bool MapWidget::usingNightMode = false;
const int MapWidget::ZOOM = 1; // fixed zoom level

QHash<QPoint, QPixmap> MapWidget::tilePixmaps;

uint qHash(const QPoint& p)
{
    return p.x() * 17 ^ p.y();
}

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal tx = (lng + 180.0) / 360.0;
    qreal divider = cos(lat * M_PI / 180.0);
    if (divider != 0) {
        qreal ty = (1.0 - log(tan(lat * M_PI / 180.0) +
                          1.0 / divider) / M_PI) / 2.0;
        return QPointF(tx * zn, ty * zn);
    }

    return QPointF(0, 0);
}

QPointF tileForCoordinate(QPointF latLong, int zoom)
{
    return tileForCoordinate(latLong.x(), latLong.y(), zoom);
}

qreal getDistance(const QPointF &p1, const QPointF &p2)
{
    qreal x = qAbs(p1.x() - p2.x());
    qreal y = qAbs(p1.y() - p2.y());
    return std::sqrt(x * x + y * y);
}

MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent)
{

    loadTiles();
    setCenter(QPointF(0, 0));
    installEventFilter(this);
}

void MapWidget::setTilesDir(const QString &newDir)
{
    MapWidget::TILES_DIR = newDir;
}

QPointF MapWidget::getCenterLatLong() const
{
    if (markers.isEmpty())
        return QPointF(0, 0);

    if (markers.count() == 1)
        return markers.first().getLatLong();

    double minLatitude = 1000.0; //storing min and max lat and long to computer the center point in the map
    double minLongitude = 1000.0;
    double maxLatitude = -1000.0;
    double maxLongitude = -1000.0;

    for (const MapMarker &marker : markers) {
        QPointF latLong = marker.getLatLong();
        qreal latitude = latLong.x();
        qreal longitude = latLong.y();
        if (latitude < minLatitude)
            minLatitude = latitude;

        if (latitude > maxLatitude)
            maxLatitude = latitude;

        if (longitude < minLongitude)
            minLongitude= longitude;

        if (longitude > maxLongitude)
            maxLongitude= longitude;
    }

    qreal centerLatitude = (maxLatitude-minLatitude)/2.0 + minLatitude;
    qreal centerLongitude = (maxLongitude-minLongitude)/2.0 + minLongitude;

    return QPointF(centerLatitude, centerLongitude);
}

QPointF MapWidget::getMinimumLatLongInMarkers() const
{
    if (markers.isEmpty())
        return QPointF(0, 0);

    if (markers.size() == 1)
        return markers.first().getLatLong();

    qreal minLatitude = 1000;
    qreal minLongitude = 1000;
    for (const MapMarker &marker : markers) {
        qreal latitude = marker.getLatLong().x();
        qreal longitude = marker.getLatLong().y();
        if (latitude < minLatitude)
            minLatitude = latitude;

        if (longitude < minLongitude)
            minLongitude = longitude;
    }

    return QPointF(minLatitude, minLongitude);
}

QPointF MapWidget::getMaximumLatLongInMarkers() const
{
    if (markers.isEmpty())
        return QPointF(0, 0);

    if (markers.size() == 1)
        return markers.first().getLatLong();

    qreal maxLatitude = -1000;
    qreal maxLongitude = -1000;
    for (const MapMarker &marker : markers) {
        qreal latitude = marker.getLatLong().x();
        qreal longitude = marker.getLatLong().y();
        if (latitude > maxLatitude)
            maxLatitude = latitude;

        if (longitude > maxLongitude)
            maxLongitude = longitude;
    }

    return QPointF(maxLatitude, maxLongitude);
}

QRectF MapWidget::computeMinimumRect(int zoom) const
{
    QPointF center = tileForCoordinate(latitude, longitude, zoom);
    QPointF min = tileForCoordinate(getMinimumLatLongInMarkers(), zoom);
    QPointF max = tileForCoordinate(getMaximumLatLongInMarkers(), zoom);
    qreal hCenter = width()/2.0;
    qreal vCenter = height()/2.0;
    qreal left = hCenter - ((center.x() - min.x()) * TILES_SIZE);
    qreal top = vCenter - ((center.y() - min.y()) * TILES_SIZE);
    qreal right = hCenter - ((center.x() - max.x()) * TILES_SIZE);
    qreal bottom = vCenter - ((center.y() - max.y()) * TILES_SIZE);
    return QRectF(QPointF(left, top), QPointF(right, bottom));
}

void MapWidget::setMarkers(const QList<MapMarker> &newMarkers)
{
    markers.clear();
    markers.append(newMarkers);

    setCenter(getCenterLatLong());
}

void MapWidget::setCenter(QPointF latLong)
{
    this->latitude = latLong.x();
    this->longitude = latLong.y();
    invalidate();
}

QSize MapWidget::minimumSizeHint() const
{
    QSize hint = QWidget::minimumSizeHint();
    QRectF minRect = computeMinimumRect(ZOOM);
    hint.setHeight(qMax(240.0, minRect.height()));
    return hint;
}

void MapWidget::invalidate()
{
    if (width() <= 0 || height() <= 0)
        return;

    QPointF center = tileForCoordinate(latitude, longitude, ZOOM);
    qreal tileX = center.x();
    qreal tileY = center.y();

    // top-left corner of the center tile
    int xp = width() / 2 - (tileX - floor(tileX)) * TILES_SIZE;
    int yp = height() / 2 - (tileY - floor(tileY)) * TILES_SIZE;

    // first tile vertical and horizontal
    int xa = (xp + TILES_SIZE - 1) / TILES_SIZE;
    int ya = (yp + TILES_SIZE - 1) / TILES_SIZE;
    int xs = static_cast<int>(tileX) - xa;
    int ys = static_cast<int>(tileY) - ya;

    // offset for top-left tile
    offset = QPoint(xp - xa * TILES_SIZE, yp - ya * TILES_SIZE);

    // last tile vertical and horizontal
    int xe = static_cast<int>(tileX) + (width() - xp - 1) / TILES_SIZE;
    int ye = static_cast<int>(tileY) + (height() - yp - 1) / TILES_SIZE;

    // build a rect
    tilesRect = QRect(xs, ys, xe - xs + 1, ye - ys + 1);

    update();
}

void MapWidget::loadTiles()
{
    int totalTiles = std::pow(2, ZOOM);
    for (int x = 0; x < totalTiles; ++x) {
        for (int y = 0; y < totalTiles; ++y) {
            QPoint tp(x, y);
            if (!tilePixmaps.contains(tp)) {
                tilePixmaps.insert(tp, loadTile(ZOOM, x, y));
            }
        }
    }
}

void MapWidget::updateMapPositionsCache()
{
    static const int markersHeight = fontMetrics().height() + TEXT_MARGIM * 2;
    qreal ellipseX = TEXT_MARGIM;
    qreal ellipseY = TEXT_MARGIM + markersHeight/2;
    qreal ellipseWidth = width() - getMaximumMarkerWidth() - TEXT_MARGIM * 2;
    qreal ellipseHeight = height() - TEXT_MARGIM * 2 - markersHeight;
    QRectF ellipseRect(ellipseX, ellipseY, ellipseWidth, ellipseHeight);

    mapPositions.clear();
    mapPositions.append(getEllipsePositions(markersHeight, ellipseRect));
}

void MapWidget::resizeEvent(QResizeEvent *)
{
    if (!markers.isEmpty())
        updateMapPositionsCache();

    setCenter(getCenterLatLong());
}

QPixmap MapWidget::loadTile(int zoomLevel, int x, int y)
{
    QString path(MapWidget::TILES_DIR + "%1/%2/%3.png");
    path = path.arg(zoomLevel).arg(x).arg(y);
    QFile imageFile(path);
    if (!imageFile.exists()) {
        qCritical() << "Tile not found to zoom:" << zoomLevel << " x:" << x << " y:" << y;
        return QPixmap();
    }

    return QPixmap(path);
}

void MapWidget::drawMapTiles(QPainter &p, const QRect &rect)
{
    int tiles = std::pow(2, ZOOM);
    for (int x = 0; x <= tilesRect.width(); ++x) {
        for (int y = 0; y <= tilesRect.height(); ++y) {
            QPoint tp(x + tilesRect.left(), y + tilesRect.top());
            QRect box = tileRect(tp);
            if (rect.intersects(box)) {
                tp.setX((tp.x() + tiles) % tiles);
                tp.setY((tp.y() + tiles) % tiles);
                if (tilePixmaps.contains(tp)) {
                    p.drawPixmap(box, tilePixmaps.value(tp));
                }
            }
        }
    }

    if (MapWidget::usingNightMode) {
        QPainter::CompositionMode compositionMode = p.compositionMode();
        p.setCompositionMode(QPainter::CompositionMode_Difference);
        p.fillRect(rect, Qt::white);
        p.setCompositionMode(compositionMode);
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p;
    p.begin(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    drawMapTiles(p, event->rect());

    drawPlayersMarkers(p);

    p.end();

}

void MapWidget::setNightMode(bool useNightMode)
{
    MapWidget::usingNightMode = useNightMode;
}

QPointF MapWidget::getMarkerScreenCoordinate(const MapMarker &marker) const
{
    QPointF center = tileForCoordinate(latitude, longitude, ZOOM);
    qreal hCenter = width()/2.0;
    qreal vCenter = height()/2.0;

    QPointF latLong = marker.getLatLong();
    QPointF tile = tileForCoordinate(latLong.x(), latLong.y(), ZOOM);
    qreal x = hCenter - ((center.x() - tile.x()) * TILES_SIZE);
    qreal y = vCenter - ((center.y() - tile.y()) * TILES_SIZE);
    return QPointF(x, y);
}

int MapWidget::getMaximumMarkerWidth() const
{
    int maxWidth = 0;
    QPointF anchor(0, 0);
    for (const MapMarker &marker : markers) {
        int markerWidth = getMarkerRect(marker, anchor).width();
        if (markerWidth > maxWidth)
            maxWidth = markerWidth;
    }
    return maxWidth;
}

QList<MapWidget::Position> MapWidget::getEmptyPositions( const QMap<int, QList<MapMarker>> markers, const QList<MapWidget::Position> &allPositions) const
{
    QList<MapWidget::Position> emptyPositions;
    for (const MapWidget::Position &position : allPositions) {
        if (markers[position.index].isEmpty())
            emptyPositions.append(position);
    }
    return emptyPositions;
}

void MapWidget::drawPlayersMarkers(QPainter &p)
{
    if (mapPositions.isEmpty() && !markers.isEmpty())
        updateMapPositionsCache();

    QMap<int, QList<MapMarker>> map;
    for (const MapMarker &marker : markers) {
        Position position= findBestEllipsePositionForMarker(marker, markers, mapPositions);
        map[position.index].append(marker);
    }

    for (int i : map.keys()) {
        while (map[i].size() > 1) {
            const MapMarker &marker = map[i].takeLast();
            QList<MapWidget::Position> emptyPositions = getEmptyPositions(map, mapPositions);
            MapWidget::Position newEmptyPosition = findBestEllipsePositionForMarker(marker, markers, emptyPositions);
            if (newEmptyPosition.index != i) { // avoid append the Position in same index and create an infinite loop
                if (map[newEmptyPosition.index].isEmpty()) // new position is really empty?
                    map[newEmptyPosition.index].append(marker);
                else
                    qCritical() << " new marker position is not empty: position " << newEmptyPosition.index;
            }
            else{
                qCritical() << "Warning! The newPosition.index is not really a new position!";
            }
        }
    }

    //finally drawing the markers
    for (int positionIndex : map.keys()) {
        if (!map[positionIndex].isEmpty()) {
            const MapMarker &marker = map[positionIndex].first();
            if (positionIndex >= 0 && positionIndex < mapPositions.size()) {
                QPointF rectPosition = mapPositions.at(positionIndex).coords;
                QPointF markerPosition = getMarkerScreenCoordinate(marker);
                drawMarker(marker, p, markerPosition, rectPosition);
            }
        }
    }
}

bool MapWidget::rectIntersectsSomeMarker(const QRectF &rect, const QList<MapMarker> &markers) const
{
    QRectF r = rect.adjusted(-10, -10, 10, 10);
    for (const MapMarker &marker : markers) {
        if (r.contains(getMarkerScreenCoordinate(marker)))
            return true;
    }
    return false;
}

MapWidget::Position MapWidget::findBestEllipsePositionForMarker(const MapMarker &marker, const QList<MapMarker> &markers, const QList<MapWidget::Position> &positions) const
{

    if (positions.isEmpty())
        return MapWidget::Position(QPointF(0, 0), 0);

    int bestPositionIndex = 0;
    QPointF markerPosition = getMarkerScreenCoordinate(marker);
    qreal minDistance = 1000.0;
    QPointF position = positions.first().coords;
    for (int i = 1; i < positions.size(); ++i) {
        const QPointF &ellipsePosition = positions.at(i).coords;
        qreal distance = getDistance(markerPosition, ellipsePosition);
        if (distance < minDistance) {
            QRectF markerRect = getMarkerRect(marker, ellipsePosition);
            if (!rectIntersectsSomeMarker(markerRect, markers)) { // avoid a rect intersecting markers circles
                minDistance = distance;
                position = ellipsePosition;
                bestPositionIndex = positions.at(i).index;
            }
        }
    }
    return MapWidget::Position(position, bestPositionIndex);
}

QList<MapWidget::Position> MapWidget::getEllipsePositions(int markersHeight, const QRectF &ellipseRect) const
{
    qreal hRadius = ellipseRect.width()/2.0;
    qreal vRadius = ellipseRect.height()/2.0;
    qreal elipseHCenter = ellipseRect.x() + hRadius;
    qreal elipseVCenter = ellipseRect.y() + vRadius;

    QList<MapWidget::Position> positions;

    qreal angle = -M_PI/2.0; //start angle
    int index = 0;
    Q_ASSERT(markersHeight > 0);
    const int maxPositions = height()/markersHeight * 2.0;
    static const qreal MAX_ANGLE = M_PI * 1.5;
    static const qreal STEP = 0.1;
    while (index < maxPositions && angle < MAX_ANGLE) {
        qreal x = elipseHCenter + (std::cos(angle) * hRadius);
        qreal y = elipseVCenter + (std::sin(angle) * vRadius);
        positions.append(MapWidget::Position(QPointF(x, y), index++));
        qreal tempY;
        do{
            angle += STEP;
            tempY = elipseVCenter + (std::sin(angle) * vRadius);
        }
        while (qAbs(tempY - y) <= markersHeight && angle < MAX_ANGLE);
    }

    // the last position is too close to the first position?
    if (positions.size() >= 2) {
        const MapWidget::Position &first = positions.first();
        const MapWidget::Position &last = positions.last();
        if (qAbs(last.coords.y() - first.coords.y()) < markersHeight)
            positions.removeLast(); // discard the last
    }

    return positions;
}

struct MapMarkerComparator
{
    QPointF anchor;
    const MapWidget *mapWidget;
    MapMarkerComparator(const MapWidget *mapWidget, const QPointF &anchor) : mapWidget(mapWidget), anchor(anchor){}
    bool operator()(const MapMarker &m1, const MapMarker &m2)
    {
        qreal deltaM1 = getDistance(mapWidget->getMarkerScreenCoordinate(m1), anchor);
        qreal deltaM2 = getDistance(mapWidget->getMarkerScreenCoordinate(m2), anchor);
        return deltaM1 < deltaM2;
    }
};

QColor MapWidget::getMarkerTextBackgroundColor()
{
    if (!MapWidget::usingNightMode)
        return QColor(0, 0, 0, 120);

    return QColor(255, 255, 255, 120);
}

QColor MapWidget::getMarkerColor()
{
    if (!MapWidget::usingNightMode)
        return Qt::red;

    return Qt::white;
}

QColor MapWidget::getMarkerTextColor()
{
    if (!MapWidget::usingNightMode)
        return Qt::white;

    return Qt::black;
}

void MapWidget::drawMarker(const MapMarker &marker, QPainter &painter, const QPointF &markerPosition, const QPointF &rectPosition) const
{
    QString text = marker.getPlayerName();

    QRectF markerRect = getMarkerRect(marker, rectPosition);

    QColor bgColor = getMarkerTextBackgroundColor();
    painter.setBrush(QBrush(bgColor));

    // drawing the line connector
    painter.setPen(bgColor);
    painter.setClipping(true);
    painter.setClipRegion(QRegion(rect()).subtracted(markerRect.toRect()));
    painter.drawLine(markerRect.center(), markerPosition);

    // drawing the transparent background
    painter.setClipping(false);
    painter.setPen(Qt::NoPen);
    painter.drawRect(markerRect);

    // drawing the player marker
    const static qreal markerSize = 1.6;
    painter.setBrush(getMarkerColor());
    painter.drawEllipse(markerPosition, markerSize, markerSize);

    //draw the player name text
    painter.setPen(getMarkerTextColor());
    qreal textY = rectPosition.y() + TEXT_MARGIM + fontMetrics().descent()/2.0;
    qreal textX = rectPosition.x() + TEXT_MARGIM;
    painter.drawText(textX, textY, text);
}

QRectF MapWidget::getMarkerRect(const MapMarker &marker, const QPointF &anchor) const
{
    QFontMetrics fMetrics = fontMetrics();
    QString text = marker.getPlayerName();
    qreal rectWidth = fMetrics.width(text) + TEXT_MARGIM * 2;
    qreal height = fMetrics.height() + TEXT_MARGIM;
    QRectF rect(anchor.x(), anchor.y() - height/2.0, rectWidth, height);
    return rect;
}

QRect MapWidget::tileRect(const QPoint &tp) const
{
    QPoint t = tp - tilesRect.topLeft();
    int x = t.x() * TILES_SIZE + offset.x();
    int y = t.y() * TILES_SIZE + offset.y();
    return QRect(x, y, TILES_SIZE, TILES_SIZE);
}

bool MapWidget::eventFilter(QObject *object, QEvent *ev)
{
    if (ev->type() == QEvent::ToolTip) {
        QHelpEvent *event = static_cast<QHelpEvent *>(ev);
        QPoint mousePosition = event->pos();

        //check if mouse is intercepting some marker and show country name as tooltip
        for (const MapMarker &marker : markers) {
            QPointF markerPosition = getMarkerScreenCoordinate(marker);
            QRectF markerRect(markerPosition.x() - 2.5, markerPosition.y() - 2.5, 5, 5);
            if (markerRect.contains(QPointF(mousePosition))) {
                QToolTip::showText(event->globalPos(), marker.getCountryName());
                return true;
            }else {
                QToolTip::hideText();
                event->ignore();
            }
        }
    }
    return QWidget::eventFilter(object, ev);
}

