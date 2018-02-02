#include "FxPanelItem.h"
#include "plugins/Guis.h"
#include "audio/core/Plugins.h"
#include "MainController.h"
#include "LocalTrackView.h"
#include "audio/core/PluginDescriptor.h"
#include "LocalTrackViewStandalone.h"

#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QAction>
#include <QMouseEvent>
#include <QMenu>
#include <QDesktopWidget>
#include <QStyle>

using controller::MainControllerStandalone;

FxPanelItem::FxPanelItem(LocalTrackViewStandalone *parent, MainControllerStandalone *mainController) :
    QFrame(parent),
    plugin(nullptr),
    bypassButton(new QPushButton(this)),
    label(new QLabel()),
    mainController(mainController),
    localTrackView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_contextMenu(QPoint)));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(2);

    layout->addWidget(this->label, 1);
    layout->addWidget(this->bypassButton);

    this->bypassButton->setVisible(false);
    this->bypassButton->setCheckable(true);
    this->bypassButton->setChecked(true);

    QObject::connect(this->bypassButton, SIGNAL(clicked()), this, SLOT(on_buttonClicked()));
}

bool FxPanelItem::pluginIsBypassed()
{
    return containPlugin() && plugin->isBypassed();
}

void FxPanelItem::on_buttonClicked()
{
    if (plugin) {
        this->plugin->setBypass(!this->bypassButton->isChecked());
        updateStyleSheet();
    }
}

FxPanelItem::~FxPanelItem()
{
}

void FxPanelItem::updateStyleSheet()
{
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void FxPanelItem::setPlugin(audio::Plugin *plugin)
{
    this->plugin = plugin;
    this->label->setText(plugin->getName());
    this->bypassButton->setVisible(true);
    this->bypassButton->setChecked(!plugin->isBypassed());
    updateStyleSheet();
}

void FxPanelItem::unsetPlugin()
{
    this->plugin->closeEditor();
    this->label->setText("");
    this->bypassButton->setVisible(false);

    mainController->removePlugin(this->localTrackView->getInputIndex(), plugin);

    this->plugin = nullptr;

    updateStyleSheet();
}

void FxPanelItem::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled())
        return;

    if (event->button() == Qt::LeftButton) {
        if (!containPlugin()) {
            on_contextMenu(event->pos());
        } else {
            if (plugin)
                showPluginGui(plugin);
        }
    }
}

void FxPanelItem::enterEvent(QEvent *)
{
    if (!isEnabled())
        return;

    if (!containPlugin())
        label->setText(tr("new effect..."));
}

void FxPanelItem::leaveEvent(QEvent *)
{
    if (!isEnabled())
        return;

    if (!containPlugin())
        label->setText("");
}

void FxPanelItem::showPluginsListMenu(const QPoint &p)
{
    QMenu menu;

    QList<PluginDescriptor::Category> categories;
    categories << PluginDescriptor::VST_Plugin;
#ifdef Q_OS_MAC
    categories << PluginDescriptor::AU_Plugin;
#endif
    //categories << PluginDescriptor::Native_Plugin; // native plugins are not implemented yet

    for (PluginDescriptor::Category category : categories) { // category = VST, NATIVE, AU

        QMenu *categoryMenu = &menu;
        if (categories.size() > 1) {
            QString categoryName = audio::PluginDescriptor::categoryToString(category);
            categoryMenu = new QMenu(categoryName);
            menu.addMenu(categoryMenu);
        }

        auto plugins = mainController->getPluginsDescriptors(category);

        for (const QString &manufacturer : plugins.keys()) {
            bool canCreateManufacturerMenu = !manufacturer.isEmpty() && plugins[manufacturer].size() > 1; // when the manufacturer has only one plugin this plugin is showed in the Root menu
            QMenu *parentMenu = categoryMenu;
            if (canCreateManufacturerMenu) {
                parentMenu = new QMenu(manufacturer);
                categoryMenu->addMenu(parentMenu);
            }

            for (const auto &pluginDescriptor  : plugins[manufacturer]) {
                QAction *action = parentMenu->addAction(pluginDescriptor.getName(), this, SLOT(loadSelectedPlugin()));
                action->setData(pluginDescriptor.toString());
            }
        }
    }

    menu.move(mapToGlobal(p));
    menu.exec();
}

void FxPanelItem::on_contextMenu(QPoint p)
{
    if (!containPlugin()) { // show plugins list
        showPluginsListMenu(p);
    }
    else { // show actions list if contain a plugin
        QMenu menu(this);
        menu.connect(&menu, SIGNAL(triggered(QAction *)), this, SLOT(on_actionMenuTriggered(QAction *)));
        menu.addAction(tr("bypass"));
        menu.addAction(tr("remove"));
        menu.move(mapToGlobal(p));
        menu.exec();
    }
}

void FxPanelItem::loadSelectedPlugin()
{
    auto action = qobject_cast<QAction *>(QObject::sender());

    if (!action || action->data().toString().isEmpty())
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents(); // force the cursor change

    // add a new plugin
    auto descriptor = audio::PluginDescriptor::fromString(action->data().toString());
    qint32 pluginSlotIndex = localTrackView->getPluginFreeSlotIndex();
    if (pluginSlotIndex >= 0) { // valid plugin slot (-1 will be returned when no free plugin slots are available)
        quint32 trackIndex = localTrackView->getInputIndex();
        auto plugin = mainController->addPlugin(trackIndex, pluginSlotIndex, descriptor);
        if (plugin) {
            localTrackView->addPlugin(plugin, pluginSlotIndex);
            showPluginGui(plugin);

            // if newProcessor is the first added processor, and is a virtual instrument (VSTi), and the subchannel is 'no input' then change the input selection to midi
            if (pluginSlotIndex == 0 && plugin->isVirtualInstrument()) {
                if (localTrackView->isNoInput()) {
                    localTrackView->setToMidi(); // select the first midi device, all channels
                }
            }
        }
        else {
            qCritical() << "Can´t instantiate the plugin " << descriptor.getName() << " -> " << descriptor.getPath();
        }
    }
    QApplication::restoreOverrideCursor();
}

void FxPanelItem::on_actionMenuTriggered(QAction *a)
{
    if (containPlugin()) {
        if (a->text() == tr("bypass"))
            bypassButton->click(); // simulate a click in the bypass button
        else if (a->text() == tr("remove"))
            unsetPlugin(); // set this->plugin to nullptr AND remove from mainController
    }
}

void FxPanelItem::showPluginGui(audio::Plugin *plugin)
{
    if (plugin) {
        QDesktopWidget desktop;
        int hCenter = desktop.geometry().width()/2;    // - window->width();
        int vCenter = desktop.geometry().height()/2;    // - window->height();
        plugin->openEditor(QPoint(hCenter, vCenter));
    }
}
