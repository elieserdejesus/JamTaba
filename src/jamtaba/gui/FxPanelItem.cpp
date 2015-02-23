#include "fxpanelitem.h"
#include "plugins/guis.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>


FxPanelItem::FxPanelItem(QWidget *parent, QMenu *fxMenu)
    :QLabel(parent),
      menu(fxMenu),
      pluginGui(nullptr)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_contextMenu(QPoint)));
    //setMinimumHeight(14);
}

FxPanelItem::~FxPanelItem()
{

}

void FxPanelItem::setPlugin(PluginGui *pluginGui){
    this->pluginGui = pluginGui;
    this->setText( pluginGui->getPluginName());
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
        setText("new effect ...");
    }
}

void FxPanelItem::leaveEvent(QEvent *){
    if(!containPlugin()){
        setText("");
    }
}

void FxPanelItem::on_contextMenu(QPoint p){
    if(menu && !containPlugin()){
        menu->popup(mapToGlobal(p));
    }
}


