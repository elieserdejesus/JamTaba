#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>
#include <QMenu>

class FxPanelItem;

namespace Audio {
    class Plugin;
}

class FxPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FxPanel( QWidget *parent, QMenu* fxMenu);
    ~FxPanel();
    void addPlugin(Audio::Plugin* plugin);
    void removePlugin(Audio::Plugin* plugin);
signals:
    void editingPlugin(Audio::Plugin* plugin);
    void pluginRemoved(Audio::Plugin* plugin);
protected:
    void paintEvent(QPaintEvent* );

private:
    QMenu* fxMenu;
};

#endif // FXPANEL_H
