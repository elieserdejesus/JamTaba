#include "WavePeakPanel.h"

#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QGraphicsBlurEffect>

WavePeakPanel::WavePeakPanel(QWidget *parent) :
    QWidget(parent),
    maxPeaks(0),
    showingBuffering(false),
    bufferingPercentage(0),
    peaksColor(QColor(90, 90, 90)),
    loadingColor(Qt::gray),
    drawingMode(WavePeakPanel::BUILDINGS)
{
    setAutoFillBackground(false);
    recreatePeaksArray();
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
    blur->setBlurRadius(1.4);
    setGraphicsEffect(blur);
}

void WavePeakPanel::setDrawingMode(WavePeakPanelMode mode)
{
    this->drawingMode = mode;
    recreatePeaksArray();
    update();
}

void WavePeakPanel::setBufferingPercentage(uint percentage)
{
    if (percentage > 100)
        percentage = 100;
    bufferingPercentage = percentage;
    update();
}

void WavePeakPanel::setShowBuffering(bool showBuffering)
{
    if (showBuffering != showingBuffering) {
        showingBuffering = showBuffering;
        bufferingPercentage = 0;
        clearPeaks();
        setEnabled(showBuffering);
        updateGeometry();
    }
}

void WavePeakPanel::recreatePeaksArray()
{
    this->maxPeaks = computeMaxPeaks();
    peaksArray.resize(maxPeaks);
    peaksArray.clear();
}

int WavePeakPanel::computeMaxPeaks()
{
    int divider = getPeaksWidth() + getPeaksPad();
    if (divider > 0)
        return width() / divider;

    return 0;
}

void WavePeakPanel::clearPeaks()
{
    peaksArray.clear();
    update();
}

void WavePeakPanel::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    recreatePeaksArray();
}

QSize WavePeakPanel::minimumSizeHint()  const
{
    if (isEnabled())
        return QSize(0, 80);
    return QWidget::minimumSizeHint();
}

void WavePeakPanel::addPeak(float peak)
{
    if(showingBuffering) {
        qCritical() << "Adding peak while is buffering!";
        return;
    }

    if (peaksArray.size() >= maxPeaks)
        peaksArray.clear();
    peaksArray.push_back(peak);
    update();// repaint
}

void WavePeakPanel::paintSoundWave(QPainter &painter)
{
    size_t size = peaksArray.size();
    if (size <= 0) // avoid divide by zero
        return;

    qreal maxPeakHeight = height()/2.0;

    for (uint i = 0; i < size; i++) {
        float alpha = (float)i/size;

        QColor color(peaksColor); //using the color defined in stylesheet
        color.setAlpha(std::pow(alpha, 2) * 255);

        int peaksRectWidth = getPeaksWidth();
        int peaksPad = getPeaksPad();
        int xPos = i * (peaksRectWidth + peaksPad);

        int peakHeight = (int)(maxPeakHeight * peaksArray[i]);
        if (peakHeight == 0)
            peakHeight = 2;

        int yPos = maxPeakHeight - peakHeight;
        QLinearGradient gradient(xPos, yPos, xPos, yPos + peakHeight * 2);
        QColor secondaryColor(color);
        secondaryColor.setAlphaF(color.alphaF() * 0.25);

        gradient.setColorAt(0.0, secondaryColor);
        gradient.setColorAt(0.5, color);
        gradient.setColorAt(1.0, secondaryColor);
        painter.fillRect(xPos, yPos, peaksRectWidth, peakHeight * 2, gradient);
    }
}

void WavePeakPanel::paintBuildings(QPainter &painter)
{
    size_t size = peaksArray.size();
    if (size <= 0) // avoid divide by zero
        return;

    qreal maxPeakHeight = height() * 0.75;

    for (uint i = 0; i < size; i++) {
        float alpha = (float)i/size;

        QColor color(peaksColor); //using the color defined in stylesheet
        color.setAlpha(std::pow(alpha, 3) * 255);

        int peaksRectWidth = getPeaksWidth();
        int peaksPad = getPeaksPad();
        int xPos = i * (peaksRectWidth + peaksPad);

        int peakHeight = (int)(maxPeakHeight * peaksArray[i]);
        if (peakHeight == 0)
            peakHeight = 2;
        int yPos = maxPeakHeight - peakHeight;

        painter.fillRect(xPos, yPos, peaksRectWidth, peakHeight, color);

        color.setAlpha(color.alpha() * 0.35);
        painter.setPen(color);
        int mirroredHeight = peakHeight/4;
        qreal xPosMirrored = xPos + std::cos(0.4) * mirroredHeight;
        QPointF points[] = {
            QPointF(xPos, maxPeakHeight), //top left
            QPointF(xPos + peaksRectWidth, maxPeakHeight), // top right
            QPointF(xPosMirrored + peaksRectWidth, maxPeakHeight + mirroredHeight), //bottom right
            QPointF(xPosMirrored, maxPeakHeight + mirroredHeight)
        };
        painter.setBrush(color);
        painter.drawPolygon(points, 4);
    }
}

int WavePeakPanel::getPeaksPad() const
{
    if (drawingMode == WavePeakPanel::BUILDINGS)
        return 1;

    return 0;
}

int WavePeakPanel::getPeaksWidth() const
{
   return 2; // returning same value for all WavePeakPanelModes
}

void WavePeakPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (isVisible()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        if (showingBuffering || !peaksArray.empty())
            painter.fillRect(rect(), QColor(0, 0, 0, 140));

        if (!showingBuffering) {
            switch (drawingMode) {
            case WavePeakPanel::BUILDINGS:
                paintBuildings(painter);
                break;
            case WavePeakPanel::SOUND_WAVE:
                paintSoundWave(painter);
            }
        }
        else{ //showing buffering
            QPen pen;
            pen.setWidth(3);
            pen.setColor(loadingColor);
            painter.setPen(pen);

            float progress = bufferingPercentage/100.0;

            int rectSize = qMin(height(), width()) * 0.7;// 70% of width or height
            QRectF rectangle(width()/2 - rectSize/2, height()/2 - rectSize/2, rectSize, rectSize);

            //to understand these magic numbers, look drawArc method in Qt doc
            int startAngle = 0;
            int spanAngle = -progress * 360 * 16;
            painter.drawArc(rectangle, startAngle, spanAngle);
        }
    }
}
