#pragma once

#include "BaseTrackView.h"
#include <QWidget>
#include <QMenu>

class FxPanel;

namespace Audio {
class Plugin;
}

namespace Controller {
    class MainController;
}

class LocalTrackView : public BaseTrackView
{
    Q_OBJECT
public:
    LocalTrackView(QWidget *parent, Controller::MainController* mainController);
    ~LocalTrackView();
    void initializeFxPanel(QMenu* fxMenu);
    void addPlugin(Audio::Plugin* plugin);
signals:
    void editingPlugin(Audio::Plugin* plugi);
    void removingPlugin(Audio::Plugin* plugin);
private:
    FxPanel* fxPanel;
};

