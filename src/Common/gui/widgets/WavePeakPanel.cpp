#include "WavePeakPanel.h"

#include <QPainter>
#include <QDebug>
#include <cmath>
#include <QGraphicsBlurEffect>

const int WavePeakPanel::peaksRectWidth = 1;
const int WavePeakPanel::peaksPad = 0;

WavePeakPanel::WavePeakPanel(QWidget *parent) :
    QWidget(parent),
    maxPeaks(0),
    showingBuffering(false),
    bufferingPercentage(0),
    peaksColor(QColor(90, 90, 90)),
    loadingColor(Qt::gray)
{
    setAutoFillBackground(false);
    recreatePeaksArray();
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
    blur->setBlurRadius(1.65);
    setGraphicsEffect(blur);
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
    return width() / (peaksRectWidth + peaksPad);
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

void WavePeakPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if (isVisible()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (!showingBuffering) {
            size_t size = peaksArray.size();
            for (uint i = 0; i < size; i++) {
                float alpha = (float)i/size;

                QColor color(peaksColor); //using the color defined in stylesheet
                color.setAlpha(std::pow(alpha, 4) * 255);

                int xPos = i * (peaksRectWidth + peaksPad);
                drawPeak(&painter, xPos, peaksArray[i], color);
            }
        }
        else{ //showing buffering

            painter.fillRect(rect(), QColor(0, 0, 0, 80));

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

void WavePeakPanel::drawPeak(QPainter *g, int x, float peak, const QColor &color)
{
    int peakHeight = (int)(height() * peak);
    if (peakHeight == 0)
        peakHeight = 2;
    int y = (height() - peakHeight) / 2;

    g->fillRect(x, y, peaksRectWidth, peakHeight, color);
}
