#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>

class FxPanelItem;
class StandaloneLocalTrackView;

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
    FxPanel(StandaloneLocalTrackView *parent, Controller::MainController *mainController);

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
    Controller::MainController *controller;
    StandaloneLocalTrackView *localTrackView;
};

#endif // FXPANEL_H
