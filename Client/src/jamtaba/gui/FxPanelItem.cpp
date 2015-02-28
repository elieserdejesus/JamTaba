#include "fxpanelitem.h"
#include "plugins/guis.h"
#include "../audio/core/plugins.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QAction>

const QString FxPanelItem::NEW_EFFECT_STRING = "new effect...";

FxPanelItem::FxPanelItem(QWidget *parent, QMenu *fxMenu)
    :QLabel(parent),
      fxMenu(fxMenu),
      pluginGui(nullptr)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_contextMenu(QPoint)));

    this->button = new QPushButton(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addStretch();
    layout->setContentsMargins(1, 1, 1, 1);
    layout->addWidget(this->button);

    this->button->setVisible(false);
    this->button->setCheckable(true);
    this->button->setChecked(true);
    QObject::connect( this->button, SIGNAL(clicked()), this, SLOT(on_buttonClicked()) );

    this->actionsMenu = createActionsMenu();
}

bool FxPanelItem::pluginIsBypassed(){
    return containPlugin() && pluginGui->getPlugin()->isBypassed();
}

void FxPanelItem::on_buttonClicked() {
    if(pluginGui){
        this->pluginGui->getPlugin()->setBypass(!this->button->isChecked());

        //update style sheet
        style()->unpolish(this);
        style()->polish(this);
        update();
    }
}

FxPanelItem::~FxPanelItem()
{

}

void FxPanelItem::setPlugin(PluginGui *pluginGui){
    this->pluginGui = pluginGui;
    this->setText( pluginGui->getPluginName());
    this->button->setVisible(true);
    this->button->setChecked(true);
}

void FxPanelItem::unsetPlugin(){
    this->pluginGui = nullptr;
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
            if(pluginGui){
                emit editingPlugin(pluginGui);
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
        if(fxMenu){
            fxMenu->popup(mapToGlobal(p));
        }
    }
    else{//show actions list
        if(actionsMenu){
            actionsMenu->popup(mapToGlobal(p));
        }
    }
}

void FxPanelItem::on_actionMenuTriggered(QAction* a){
    if(containPlugin()){
        if( a->text() == "bypass"){
            button->click();//simulate a click in the bypass button
        }
        else if(a->text() == "remove"){
            PluginGui* pluginGui = this->pluginGui;
            unsetPlugin();//set this->pluginGui to nullptr
            emit pluginRemoved(pluginGui);
        }
    }
}

QMenu* FxPanelItem::createActionsMenu(){
    QMenu* menu = new QMenu(this);
    menu->connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(on_actionMenuTriggered(QAction*)));
    menu->addAction("bypass");
    menu->addAction("remove");
    return menu;
}

