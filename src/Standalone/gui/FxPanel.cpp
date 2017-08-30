#include "FxPanel.h"
#include "FxPanelItem.h"
#include "LocalTrackViewStandalone.h"

#include <QVBoxLayout>
#include <QPainter>

using namespace Controller;

FxPanel::FxPanel(LocalTrackViewStandalone *parent, MainControllerStandalone *mainController) :
    QWidget(parent),
    controller(mainController),
    localTrackView(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->setSpacing(1);

    for (int i = 0; i < 4; i++) {
        FxPanelItem *item = new FxPanelItem(localTrackView, mainController);
        items.append(item);
        mainLayout->addWidget(item);
    }
}

void FxPanel::removePlugins()
{
    QList<FxPanelItem *> items = findChildren<FxPanelItem *>();
    for (FxPanelItem *item : items) {
        if (item->containPlugin())
            item->unsetPlugin();
    }
}

qint32 FxPanel::getPluginFreeSlotIndex() const
{
    QList<FxPanelItem *> items = findChildren<FxPanelItem *>();
    int slotIndex = 0;
    for (FxPanelItem *item : items) {
        if (!item->containPlugin())
            return slotIndex;
        slotIndex++;
    }
    return -1; // no free slot
}

void FxPanel::addPlugin(Audio::Plugin *plugin, quint32 pluginSlotIndex)
{
    QList<FxPanelItem *> items = findChildren<FxPanelItem *>();
    if (pluginSlotIndex < (quint32)items.count()) {
        FxPanelItem *fxPanelItem = items.at(pluginSlotIndex);
        if (!fxPanelItem->containPlugin())
            fxPanelItem->setPlugin(plugin);
        else
            qCritical() << "Can't add " << plugin->getName() << " in slot index " << pluginSlotIndex;
    }
}

FxPanel::~FxPanel()
{
    // delete ui;
}


