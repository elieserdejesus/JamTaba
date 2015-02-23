#ifndef PLUGINWINDOW_H
#define PLUGINWINDOW_H

#include <QDialog>

class PluginGui;

class PluginWindow : public QDialog
{
    Q_OBJECT

public:
    PluginWindow(PluginGui* pluginGui, QWidget* parent);
    ~PluginWindow();
private:
    PluginGui* pluginGui;
};

#endif // PLUGINWINDOW_H
