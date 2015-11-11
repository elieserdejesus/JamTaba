#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>

class CustomTabWidget : public QTabWidget
{
public:
    CustomTabWidget(QWidget* parent);

    //cpu usage in percentage, memoryUsage in megabytes
    void setResourcesUsage(int cpuUsage, int memoryUsage);
protected:
    void paintEvent(QPaintEvent *event);
private:
    static QColor RESOURCES_USAGE_BG_COLOR;
    static QColor RESOURCES_USAGE_TEXT_COLOR;

    int cpuUsage;
    int memoryUsage;
};

#endif // CUSTOMTABWIDGET_H
