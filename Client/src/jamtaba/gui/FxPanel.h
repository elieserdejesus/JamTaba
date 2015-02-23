#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>
#include <QMenu>

class FxPanelItem;
class PluginGui;

class FxPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FxPanel( QWidget *parent, QMenu* fxMenu);
    ~FxPanel();
    void addPlugin(PluginGui* pluginGui);

signals:
    void editingPlugin(PluginGui* pluginGui);

protected:
    void paintEvent(QPaintEvent* );

private:
    QMenu* fxMenu;
};

#endif // FXPANEL_H
