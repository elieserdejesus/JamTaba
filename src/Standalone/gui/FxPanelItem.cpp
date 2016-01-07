#include "FxPanelItem.h"
#include "plugins/Guis.h"
#include "audio/core/Plugins.h"
#include "MainController.h"
#include "LocalTrackView.h"
#include "audio/core/PluginDescriptor.h"
#include "LocalTrackViewStandalone.h"

#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QAction>
#include <QMouseEvent>
#include <QMenu>
#include <QDesktopWidget>

const QString FxPanelItem::NEW_EFFECT_STRING = "new effect...";

FxPanelItem::FxPanelItem(LocalTrackViewStandalone *parent, Controller::MainControllerStandalone *mainController) :
    QWidget(parent),
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

void FxPanelItem::setPlugin(Audio::Plugin *plugin)
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

void FxPanelItem::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FxPanelItem::mousePressEvent(QMouseEvent *event)
{
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
    if (!containPlugin())
        label->setText(FxPanelItem::NEW_EFFECT_STRING);
}

void FxPanelItem::leaveEvent(QEvent *)
{
    if (!containPlugin())
        label->setText("");
}

void FxPanelItem::on_contextMenu(QPoint p)
{
    if (!containPlugin()) {// show plugins list
        QMenu menu;
        QList<Audio::PluginDescriptor> plugins = mainController->getPluginsDescriptors();
        if (!plugins.isEmpty()) {
            for (const Audio::PluginDescriptor &pluginDescriptor  : plugins) {
                QAction *action = menu.addAction(pluginDescriptor.getName());
                action->setData(pluginDescriptor.toString());
            }

            QObject::connect(&menu, SIGNAL(triggered(QAction *)), this,
                             SLOT(on_fxMenuActionTriggered(QAction *)));
        } else {// no plugin found
            menu.addAction("No plugin found! Check the 'Preferences -> Vst plugins' menu.");
        }
        menu.move(mapToGlobal(p));
        menu.exec();
    } else {// show actions list if contain a plugin
        QMenu menu(this);
        menu.connect(&menu, SIGNAL(triggered(QAction *)), this,
                     SLOT(on_actionMenuTriggered(QAction *)));
        menu.addAction("bypass");
        menu.addAction("remove");
        menu.move(mapToGlobal(p));
        menu.exec();
    }
}

void FxPanelItem::on_fxMenuActionTriggered(QAction *action)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();// force the cursor change

    // add a new plugin
    Audio::PluginDescriptor descriptor = Audio::PluginDescriptor::fromString(
        action->data().toString());
    Audio::Plugin *plugin = mainController->addPlugin(this->localTrackView->getInputIndex(), descriptor);
    if (plugin) {
        this->localTrackView->addPlugin(plugin);
        showPluginGui(plugin);
    }
    QApplication::restoreOverrideCursor();
}

void FxPanelItem::on_actionMenuTriggered(QAction *a)
{
    if (containPlugin()) {
        if (a->text() == "bypass")
            bypassButton->click();// simulate a click in the bypass button
        else if (a->text() == "remove")
            unsetPlugin();// set this->plugin to nullptr AND remove from mainController
    }
}

// ++++++++++++++++++++++++++
void FxPanelItem::showPluginGui(Audio::Plugin *plugin)
{
    if (plugin) {
        QDesktopWidget desktop;
        int hCenter = desktop.geometry().width()/2;    // - window->width();
        int vCenter = desktop.geometry().height()/2;    // - window->height();
        plugin->openEditor(QPoint(hCenter, vCenter));
    }
}
