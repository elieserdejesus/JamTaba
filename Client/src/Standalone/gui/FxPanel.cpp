#include "FxPanel.h"
#include "FxPanelItem.h"
#include "LocalTrackViewStandalone.h"

#include <QVBoxLayout>
#include <QPainter>

using namespace Controller;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel::FxPanel(LocalTrackViewStandalone *parent, MainControllerStandalone *mainController) :
    QWidget(parent),
    controller(mainController),
    localTrackView(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->setSpacing(2);

    for (int i = 0; i < 4; i++) {
        FxPanelItem *item = new FxPanelItem(localTrackView, mainController);
        items.append(item);
        mainLayout->addWidget(item);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::removePlugins()
{
    QList<FxPanelItem *> items = findChildren<FxPanelItem *>();
    for (FxPanelItem *item : items) {
        if (item->containPlugin())
            item->unsetPlugin();
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::addPlugin(Audio::Plugin *plugin)
{
    QList<FxPanelItem *> items = findChildren<FxPanelItem *>();
    for (FxPanelItem *item : items) {// find the first free slot to put the new plugin
        if (!item->containPlugin()) {
            item->setPlugin(plugin);
            return;
        }
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel::~FxPanel()
{
    // delete ui;
}


