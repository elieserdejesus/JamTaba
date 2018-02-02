#ifndef FXPANEL_H
#define FXPANEL_H

#include <QWidget>
//#include "MainControllerStandalone.h"

class FxPanelItem;
class LocalTrackViewStandalone;

namespace controller {
class MainControllerStandalone;
}

namespace audio {
class Plugin;
}

using controller::MainControllerStandalone;
using audio::Plugin;

class FxPanel : public QWidget
{
    Q_OBJECT

public:
    FxPanel(LocalTrackViewStandalone *parent, MainControllerStandalone *mainController);

    virtual ~FxPanel();

    void addPlugin(Plugin *plugin, quint32 pluginSlotIndex);

    qint32 getPluginFreeSlotIndex() const; // return -1 if no free slots are available

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
    controller::MainControllerStandalone *controller; // storing a 'casted' controller for convenience
    LocalTrackViewStandalone *localTrackView;
};

#endif // FXPANEL_H
