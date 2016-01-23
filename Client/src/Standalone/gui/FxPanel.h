#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>
#include "MainControllerStandalone.h"

class FxPanelItem;
class LocalTrackViewStandalone;

class FxPanel : public QWidget
{
    Q_OBJECT

public:
    FxPanel(LocalTrackViewStandalone *parent, Controller::MainControllerStandalone *mainController);

    virtual ~FxPanel();

    void addPlugin(Audio::Plugin *plugin);

    void removePlugins();

    inline LocalTrackViewStandalone *getLocalTrackView() const
    {
        return localTrackView;
    }

    inline QList<FxPanelItem *> getItems() const
    {
        return items;
    }

private:
    QList<FxPanelItem *> items;
    Controller::MainControllerStandalone *controller; //storing a 'casted' controller for convenience
    LocalTrackViewStandalone *localTrackView;
};

#endif // FXPANEL_H
