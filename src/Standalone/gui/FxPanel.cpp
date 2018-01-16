#include "FxPanel.h"
#include "FxPanelItem.h"
#include "LocalTrackViewStandalone.h"

#include <QVBoxLayout>
#include <QPainter>

using namespace controller;

FxPanel::FxPanel(LocalTrackViewStandalone *parent, MainControllerStandalone *mainController) :
    QWidget(parent),
    controller(mainController),
    localTrackView(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->setSpacing(1);

    for (int i = 0; i < 4; i++) {
        auto item = new FxPanelItem(localTrackView, mainController);
        items.append(item);
        mainLayout->addWidget(item);
    }
}

void FxPanel::removePlugins()
{
    auto items = findChildren<FxPanelItem *>();
    for (auto item : items) {
        if (item->containPlugin())
            item->unsetPlugin();
    }
}

qint32 FxPanel::getPluginFreeSlotIndex() const
{
    auto items = findChildren<FxPanelItem *>();
    int slotIndex = 0;
    for (auto item : items) {
        if (!item->containPlugin())
            return slotIndex;
        slotIndex++;
    }
    return -1; // no free slot
}

void FxPanel::addPlugin(audio::Plugin *plugin, quint32 pluginSlotIndex)
{
    auto items = findChildren<FxPanelItem *>();
    if (pluginSlotIndex < (quint32)items.count()) {
        auto fxPanelItem = items.at(pluginSlotIndex);
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


