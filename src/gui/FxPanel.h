#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>
#include "LocalTrackView.h"

class FxPanelItem;

namespace Controller {
    class MainController;
}

namespace Audio {
    class Plugin;
}

class FxPanel : public QWidget
{
    Q_OBJECT

public:
    FxPanel( LocalTrackView *parent, Controller::MainController* mainController);
    ~FxPanel();
    void addPlugin(Audio::Plugin* plugin);
    void removePlugins();
    //void removePlugin(Audio::Plugin* plugin);
    inline LocalTrackView* getLocalTrackView() const{return localTrackView;}
    inline QList<FxPanelItem*> getItems() const{return items;}

protected:
    void paintEvent(QPaintEvent* );
private:
    QList<FxPanelItem*> items;
    Controller::MainController* controller;
    LocalTrackView* localTrackView;
};

#endif // FXPANEL_H
