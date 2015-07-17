#include "fxpanelitem.h"
#include "plugins/guis.h"
#include "../audio/core/plugins.h"
#include "../MainController.h"
#include "../gui/LocalTrackView.h"
#include "../audio/core/plugins.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QAction>
#include <QMouseEvent>
#include <QMenu>
#include <QDesktopWidget>


const QString FxPanelItem::NEW_EFFECT_STRING = "new effect...";

FxPanelItem::FxPanelItem(LocalTrackView *parent, Controller::MainController *mainController)
    :QLabel(parent),
      plugin(nullptr),
      button(new QPushButton(this)),
      mainController(mainController),
      localTrackView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_contextMenu(QPoint)));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->setContentsMargins(1, 1, 1, 1);
    layout->addWidget(this->button);

    this->button->setVisible(false);
    this->button->setCheckable(true);
    this->button->setChecked(true);
    QObject::connect( this->button, SIGNAL(clicked()), this, SLOT(on_buttonClicked()) );

    //this->actionsMenu = createActionsMenu();
}

//void FxPanelItem::setFxMenu(QMenu* fxMenu){
//    this->fxMenu = fxMenu;
//}

bool FxPanelItem::pluginIsBypassed(){
    return containPlugin() && plugin->isBypassed();
}

void FxPanelItem::on_buttonClicked() {
    if(plugin){
        this->plugin->setBypass(!this->button->isChecked());

        //update style sheet
        style()->unpolish(this);
        style()->polish(this);
        update();
    }
}

FxPanelItem::~FxPanelItem()
{

}

void FxPanelItem::setPlugin(Audio::Plugin* plugin){
    this->plugin = plugin;
    this->setText( plugin->getName());
    this->button->setVisible(true);
    this->button->setChecked(!plugin->isBypassed());
}

void FxPanelItem::unsetPlugin(){
    this->plugin = nullptr;
    this->setText("");
    this->button->setVisible(false);

    //update style sheet
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void FxPanelItem::paintEvent(QPaintEvent* ev){
    QLabel::paintEvent(ev);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FxPanelItem::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if(!containPlugin()){
            on_contextMenu(event->pos());
        }
        else{
            if(plugin){
                showPluginGui(plugin);
            }
        }
    }
}

void FxPanelItem::enterEvent(QEvent *){
    if(!containPlugin()){
        setText(FxPanelItem::NEW_EFFECT_STRING);
    }
}

void FxPanelItem::leaveEvent(QEvent *){
    if(!containPlugin()){
        setText("");
    }
}

void FxPanelItem::on_contextMenu(QPoint p){
    if(!containPlugin()){//show plugins list
        QMenu menu;
        QList<Audio::PluginDescriptor> plugins = mainController->getPluginsDescriptors();
        for(const Audio::PluginDescriptor& pluginDescriptor  : plugins){
            QAction* action = menu.addAction(pluginDescriptor.getName());
            action->setData(pluginDescriptor.toString());
        }

        QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_fxMenuActionTriggered(QAction*)));
        menu.move(mapToGlobal(p));
        menu.exec();
    }
    else{//show actions list if contain a plugin
        QMenu menu(this);
        menu.connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_actionMenuTriggered(QAction*)));
        menu.addAction("bypass");
        menu.addAction("remove");
        menu.move(mapToGlobal(p));
        menu.exec();
    }
}

void FxPanelItem::on_fxMenuActionTriggered(QAction* action){
    //add a new plugin
    Audio::PluginDescriptor descriptor = Audio::PluginDescriptor::fromString( action->data().toString());
    Audio::Plugin* plugin = mainController->addPlugin(this->localTrackView->getInputIndex(), descriptor);

    this->localTrackView->addPlugin(plugin);
    showPluginGui(plugin);
}

void FxPanelItem::on_actionMenuTriggered(QAction* a){
    if(containPlugin()){
        if( a->text() == "bypass"){
            button->click();//simulate a click in the bypass button
        }
        else if(a->text() == "remove"){
            Audio::Plugin* plugin = this->plugin;
            unsetPlugin();//set this->plugin to nullptr

            if(plugin->hasEditorWindow()){
                Audio::PluginWindow* window = plugin->getEditor();
                if(window){
                    window->close();
                }
            }
            mainController->removePlugin(this->localTrackView->getInputIndex(), plugin);
        }
    }
}

//++++++++++++++++++++++++++
void FxPanelItem::showPluginGui(Audio::Plugin *plugin){
    if(plugin && plugin->hasEditorWindow()){
        Audio::PluginWindow* window = plugin->getEditor();

        if(!window->isVisible()){
            window->show();//show to generate a window handle, VST plugins use this handle to draw plugin GUI
            QDesktopWidget desktop;

            int hCenter = desktop.geometry().width()/2;// - window->width();
            int vCenter = desktop.geometry().height()/2;// - window->height();
            plugin->openEditor(QPoint(hCenter, vCenter));
        }
        else{
            window->activateWindow();
        }
    }
}
