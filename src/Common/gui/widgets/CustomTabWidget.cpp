#include "CustomTabWidget.h"
#include <QStyleOption>
#include <QPainter>

QColor CustomTabWidget::RESOURCES_USAGE_BG_COLOR = QColor(0, 0, 0, 60);
QColor CustomTabWidget::RESOURCES_USAGE_TEXT_COLOR = QColor(255, 255, 255, 160);

CustomTabWidget::CustomTabWidget(QWidget *parent) :
    QTabWidget(parent),
    cpuUsage(0),
    memoryUsage(0)
{
}

void CustomTabWidget::setResourcesUsage(double cpuUsage, int memoryUsage)
{
    this->cpuUsage = cpuUsage;
    this->memoryUsage = memoryUsage;
    repaint();
}

void CustomTabWidget::paintEvent(QPaintEvent *e){
    QTabWidget::paintEvent(e);

    QPainter painter(this);
    //draw the cpu/memory usage background
    QString string = "CPU:" + QString::number(cpuUsage, 'f', 0) + "% MEM:" + QString::number(memoryUsage) + "%";
    const int H_MARGIM = 3;
    const int V_MARGIM = 2;
    const int ROUND = 3;
    int rectWidth = painter.fontMetrics().width(string) + H_MARGIM * 2;
    int rectHeight = tabBar()->height() - V_MARGIM * 2;
    int x = width() - rectWidth;

    //fill the brackground
    painter.setBrush(RESOURCES_USAGE_BG_COLOR);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(x, V_MARGIM, rectWidth, rectHeight , ROUND, ROUND);

    //draw the text
    painter.setPen(RESOURCES_USAGE_TEXT_COLOR);
    int textY = rectHeight - painter.fontMetrics().descent();
    painter.drawText(x + H_MARGIM, textY, string);

}
