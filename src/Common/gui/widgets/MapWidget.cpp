#include "MapWidget.h"
#include <QtCore>
#include <QtWidgets>
#include <QDebug>
#include <cmath>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QString MapWidget::TILES_DIR = ":/tiles/map/";
const int MapWidget::TILES_SIZE = 256; // tile size in pixels
const qreal MapWidget::TEXT_MARGIM = 3;
const int MapWidget::MARKER_POSITIONS = 8;

QMap<int, QHash<QPoint, QPixmap>> MapWidget::tilePixmaps;

// some consts used to translate map tiles to lat,long
const qreal deg2rad = M_PI/180.0;
const qreal A_EARTH = 6378137;
const qreal flattening = 1.0/298.257223563;
const qreal NAV_E2 = (2.0-flattening)*flattening;

uint qHash(const QPoint& p)
{
    return p.x() * 17 ^ p.y();
}

QPointF tileForCoordinate(qreal lat, qreal lng, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal tx = (lng + 180.0) / 360.0;
    qreal ty = (1.0 - log(tan(lat * M_PI / 180.0) +
                          1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0;
    return QPointF(tx * zn, ty * zn);
}

QPointF tileForCoordinate(QPointF latLong, int zoom)
{
    return tileForCoordinate(latLong.x(), latLong.y(), zoom);
}

qreal longitudeFromTile(qreal tx, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal lat = tx / zn * 360.0 - 180.0;
    return lat;
}

qreal latitudeFromTile(qreal ty, int zoom)
{
    qreal zn = static_cast<qreal>(1 << zoom);
    qreal n = M_PI - 2 * M_PI * ty / zn;
    qreal lng = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
    return lng;
}


MapWidget::MapWidget(QWidget *parent)
    : QWidget(parent),
      zoom(1),
      showingMarkers(true)
{

    loadTiles();
    setCenter(QPointF(0, 0));
}

void MapWidget::setMarkersVisibility(bool showMarkers)
{
    if (showingMarkers != showMarkers) {
        showingMarkers = showMarkers;
    }
    update();
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

void MapWidget::clearMarkerPositions()
{
    markersPositions.clear();
    for (int position = 0; position < MARKER_POSITIONS; ++position) {
        markersPositions.insert(position, false); // position is not used
    }
}

void MapWidget::setMarkers(const QList<MapMarker> &newMarkers)
{
    markers.clear();
    markers.append(newMarkers);

    autoAdjustZoomLevel();

    setCenter(getCenterLatLong());
}

int MapWidget::calculateBestZoomLevel() const
{
    // auto adjust zoom
    int zoomLevel = 1; // 1 is the max zoom level
    QRectF rect = computeMinimumRect(zoomLevel);
    while ((qAbs(rect.width()) > width() || qAbs(rect.height()) > height()) && zoomLevel > 0) {
        zoomLevel--;
        rect = computeMinimumRect(zoomLevel); // trying another zoom level
    }
    return zoomLevel;
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
    QRectF minRect = computeMinimumRect(zoom);
    hint.setHeight(qMax(240.0, minRect.height()));
    return hint;
}

void MapWidget::invalidate()
{
    if (width() <= 0 || height() <= 0)
        return;

    QPointF center = tileForCoordinate(latitude, longitude, zoom);
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
    int totalTiles = std::pow(2, zoom);
    for (int x = 0; x < totalTiles; ++x) {
        for (int y = 0; y < totalTiles; ++y) {
            QPoint tp(x, y);
            if (!tilePixmaps[zoom].contains(tp)) {
                tilePixmaps[zoom].insert(tp, loadTile(zoom, x, y));
            }
        }
    }
}

void MapWidget::resizeEvent(QResizeEvent *)
{
    autoAdjustZoomLevel();
    setCenter(getCenterLatLong());
}

void MapWidget::autoAdjustZoomLevel()
{
    int bestZoom = calculateBestZoomLevel();
    if (bestZoom != zoom) {
        zoom = bestZoom;
        loadTiles(); // load the tiles for the new zoom level
    }
}

QPixmap MapWidget::loadTile(int zoomLevel, int x, int y)
{
    QString path(MapWidget::TILES_DIR + "%1/%2/%3.png");
    path = path.arg(zoomLevel).arg(x).arg(y);
    QFile imageFile(path);
    if (!imageFile.exists()) {
        qCritical() << "Tile no found to zoom:" << zoomLevel << " x:" << x << " y:" << y;
        return QPixmap();
    }

    return QPixmap(path);
}

void MapWidget::drawMapTiles(QPainter &p, const QRect &rect)
{
    int tiles = std::pow(2, zoom);
    for (int x = 0; x <= tilesRect.width(); ++x) {
        for (int y = 0; y <= tilesRect.height(); ++y) {
            QPoint tp(x + tilesRect.left(), y + tilesRect.top());
            QRect box = tileRect(tp);
            if (rect.intersects(box)) {
                tp.setX((tp.x() + tiles) % tiles);
                tp.setY((tp.y() + tiles) % tiles);
                if (tilePixmaps[zoom].contains(tp)) {
                    p.drawPixmap(box, tilePixmaps[zoom].value(tp));
                }
            }
        }
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p;
    p.begin(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    drawMapTiles(p, event->rect());

    bool showCountryDetailsInMarkers = zoom < 3;
    if (showingMarkers)
        drawPlayersMarkers(p, showCountryDetailsInMarkers);

    if (!showCountryDetailsInMarkers || !showingMarkers)
        drawPlayersList(p);

    p.end();
}

QPointF MapWidget::getMarkerScreenPosition(const MapMarker &marker) const
{
    QPointF center = tileForCoordinate(latitude, longitude, zoom);
    qreal hCenter = width()/2.0;
    qreal vCenter = height()/2.0;

    QPointF latLong = marker.getLatLong();
    QPointF tile = tileForCoordinate(latLong.x(), latLong.y(), zoom);
    qreal x = hCenter - ((center.x() - tile.x()) * TILES_SIZE);
    qreal y = vCenter - ((center.y() - tile.y()) * TILES_SIZE);
    return QPointF(x, y);
}

qreal distance(const QPointF &p1, const QPointF &p2)
{
    qreal x = qAbs(p1.x() - p2.x());
    qreal y = qAbs(p1.y() - p2.y());
    return std::sqrt(x * x + y * y);
}

QPointF MapWidget::getBestMarkerRectPosition(const QPointF &screenPosition)
{
    if (markers.isEmpty() || markers.size() == 1) {
        return QPointF(screenPosition.x() + 10, screenPosition.y() + 10);
    }

    qreal hRadius = width()/2.0 * 0.8;
    qreal vRadius = height()/2.0 * 0.8;
    qreal hCenter = width()/2.0;
    qreal vCenter = height()/2.0;
    qreal angleIncrement = (2 * M_PI)/MARKER_POSITIONS; // dividing the circle in 8 pies
    qreal angle = 0.0;

    qreal minDistance = hRadius;
    QPointF bestPosition = screenPosition;
    int bestPositionIndex = 0;
    for (int i = 1; i < MARKER_POSITIONS; ++i) {
        angle += angleIncrement;
        QPointF candidate(hCenter + (hRadius * std::cos(angle)), vCenter + (vRadius * std::sin(angle)));
        qreal dist = distance(candidate, screenPosition);
        if (dist < minDistance && !markersPositions[i]) { // slot[i] is free to use?
            minDistance = dist;
            bestPosition = candidate;
            bestPositionIndex = i;
        }
    }
    markersPositions.insert(bestPositionIndex, true); // this slot is used
    return bestPosition;
}

void MapWidget::drawPlayersMarkers(QPainter &p, bool showCountryDetailsInMarkers)
{
    clearMarkerPositions();
    for (MapMarker &marker : markers) {
        QPointF screenPosition = getMarkerScreenPosition(marker);
        QPointF rectPosition = getBestMarkerRectPosition(screenPosition);
        drawMarker(marker, p, screenPosition, rectPosition, showCountryDetailsInMarkers);
    }
}

void MapWidget::drawMarker(const MapMarker &marker, QPainter &painter, const QPointF &markerPosition, const QPointF &rectPosition, bool showCountryDetails) const
{
    QFontMetrics fMetrics = fontMetrics();
    QString text = marker.getText(showCountryDetails);
    qreal textWidth = fMetrics.width(text);

    //drawing the dark transparent background
    static QColor bgColor(0, 0, 0, 120);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(bgColor));
    painter.drawPath(getMarkerPainterPath(marker, markerPosition, rectPosition, showCountryDetails));

    //drawing the player red circle
    static qreal circleRadius = 2.5;
    static QColor redColor(255, 0, 0, 180);
    painter.setBrush(redColor);
    painter.drawEllipse(markerPosition, circleRadius, circleRadius);

    //draw the player name text
    painter.setPen(Qt::white);
    qreal textY = rectPosition.y() + TEXT_MARGIM;
    qreal textX = rectPosition.x() + TEXT_MARGIM;
    painter.drawText(textX, textY, text);

    qreal imageX = textX + textWidth + TEXT_MARGIM;
    qreal imageY = rectPosition.y() - fMetrics.height()/2.0;
    painter.drawImage(QPointF(imageX, imageY), marker.getFlag());
}

QPainterPath MapWidget::getMarkerPainterPath(const MapMarker &marker, const QPointF &markerPosition, const QPointF &rectPosition, bool showCountryDetails) const
{
    QRectF markerRect = getMarkerRect(marker, rectPosition, showCountryDetails);
    qreal rectCenter = markerRect.center().x();

    QPainterPath painterPath;

    painterPath.addRoundRect(markerRect, 15);

    painterPath.moveTo(markerPosition);
    qreal triangleY = (rectPosition.y() < markerPosition.y()) ? markerRect.bottom() : markerRect.top();
    painterPath.lineTo(rectCenter - 4.0, triangleY);
    painterPath.lineTo(rectCenter + 4.0, triangleY);

    return painterPath;
}

QRectF MapWidget::getMarkerRect(const MapMarker &marker, const QPointF &anchor, bool showCountryDetails) const
{
    QFontMetrics fMetrics = fontMetrics();
    QString text = marker.getText(showCountryDetails);
    const QImage &flag = marker.getFlag();
    qreal rectWidth = fMetrics.width(text) + TEXT_MARGIM * 3 + flag.width();
    qreal rectX = 0;
    qreal height = fMetrics.height() + (TEXT_MARGIM * 2);
    qreal rectY = -height/2;
    QRectF rect(rectX, rectY, rectWidth, height);
    rect.translate(anchor.x(), anchor.y());
    return rect;
}

void MapWidget::drawPlayersList(QPainter &p)
{
    if (markers.isEmpty())
        return;

    p.setTransform(QTransform());//reset transform

    QFontMetrics metrics = fontMetrics();
    qreal fontHeight = metrics.height();

    //compute the background rectangle width
    qreal maxWidth = 0;
    bool showCountryDetails = zoom < 3;
    for(const MapMarker &mark : markers) {
        QString userString = mark.getText(showCountryDetails);
        qreal width = metrics.width(userString);
        if (width > maxWidth)
            maxWidth = width;
    }

    //draw the background rect
    static const qreal margim = 5;
    QRectF backgroundRect(margim, margim, maxWidth + margim * 2, fontHeight * markers.count() + margim * 2);
    static const QColor bgColor(255, 255, 255, 160);
    p.setPen(Qt::NoPen);
    p.setBrush(bgColor);
    p.drawRect(backgroundRect);

    //draw the players names
    int y = fontHeight + margim;
    int x = margim * 2;
    p.setPen(Qt::black);
    for(const MapMarker &mark : markers) {
        QString userString = mark.getText(showCountryDetails);
        p.drawText(x, y, userString);
        y += fontHeight;
    }

}

QRect MapWidget::tileRect(const QPoint &tp) const
{
    QPoint t = tp - tilesRect.topLeft();
    int x = t.x() * TILES_SIZE + offset.x();
    int y = t.y() * TILES_SIZE + offset.y();
    return QRect(x, y, TILES_SIZE, TILES_SIZE);
}
