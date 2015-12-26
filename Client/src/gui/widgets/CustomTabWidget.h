#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>

class CustomTabWidget : public QTabWidget
{
public:
    CustomTabWidget(QWidget *parent);
    void setResourcesUsage(double cpuUsage, int memoryUsage);// cpu usage in percentage, memoryUsage in megabytes
private:
    static QColor RESOURCES_USAGE_BG_COLOR;
    static QColor RESOURCES_USAGE_TEXT_COLOR;

    double cpuUsage;
    int memoryUsage;
};

#endif // CUSTOMTABWIDGET_H
