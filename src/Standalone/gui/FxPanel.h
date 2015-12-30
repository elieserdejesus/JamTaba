#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>
#include "MainControllerStandalone.h"

class FxPanelItem;
class StandaloneLocalTrackView;

class FxPanel : public QWidget
{
    Q_OBJECT

public:
    FxPanel(StandaloneLocalTrackView *parent, Controller::StandaloneMainController *mainController);

    virtual ~FxPanel();

    void addPlugin(Audio::Plugin *plugin);

    void removePlugins();

    inline StandaloneLocalTrackView *getLocalTrackView() const
    {
        return localTrackView;
    }

    inline QList<FxPanelItem *> getItems() const
    {
        return items;
    }

protected:
    void paintEvent(QPaintEvent *);

private:
    QList<FxPanelItem *> items;
    Controller::StandaloneMainController *controller; //storing a 'casted' controller for convenience
    StandaloneLocalTrackView *localTrackView;
};

#endif // FXPANEL_H
