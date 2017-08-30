#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>

class CustomTabWidget : public QTabWidget
{

public:
    explicit CustomTabWidget(QWidget *parent);
    void setResourcesUsage(int memoryUsage); // cpu usage in percentage, memoryUsage in megabytes
    protected:
        void paintEvent(QPaintEvent *event);
private:
    static QColor RESOURCES_USAGE_BG_COLOR;
    static QColor RESOURCES_USAGE_TEXT_COLOR;

    // double cpuUsage;
    int memoryUsage;
};

#endif // CUSTOMTABWIDGET_H
