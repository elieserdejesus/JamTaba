#pragma once

#include "TrackView.h"
#include <QWidget>
#include <QMenu>

class FxPanel;
class PluginGui;

class LocalTrackView : public TrackView
{
    Q_OBJECT
public:
    LocalTrackView(QWidget *parent = 0);
    ~LocalTrackView();
    void initializeFxPanel(QMenu* fxMenu);
    void addPlugin(PluginGui* pluginGui);
signals:
    void editingPlugin(PluginGui* pluginGui);
    void removingPlugin(PluginGui* pluginGui);
private:
    FxPanel* fxPanel;
};

