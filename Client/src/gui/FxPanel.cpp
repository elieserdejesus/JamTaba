#include "FxPanel.h"
//#include "ui_fxpanel.h"
#include "FxPanelItem.h"
#include "plugins/guis.h"
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QLayout>
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel::FxPanel(LocalTrackView *parent, Controller::MainController *mainController)
    :
    QWidget(parent),
    controller(mainController),
    localTrackView(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->setSpacing(2);

    //QWidget* contentPane = new QWidget(this);

    //QScrollArea* scrollArea = new QScrollArea(this);
    //scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //QVBoxLayout* contentLayout = new QVBoxLayout( contentPane);
    //contentLayout->setContentsMargins(QMargins(2, 2, 2, 2));
    //contentLayout->setSpacing(2);
    //contentLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    //scrollArea->setWidget(contentPane);
    //scrollArea->setWidgetResizable(true);
    //mainLayout->addWidget(scrollArea);

    for(int i=0; i < 4; i++){
        FxPanelItem* item = new FxPanelItem(localTrackView, mainController);
        items.append(item);
        //contentLayout->addWidget(item);
        mainLayout->addWidget(item);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::removePlugin(){
    QList<FxPanelItem*> items = findChildren<FxPanelItem*>();
    for(FxPanelItem* item : items){//find the first free slot to put the new plugin
        if(item->containPlugin()){
            item->unsetPlugin();

            return;
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::addPlugin(Audio::Plugin * plugin){
    QList<FxPanelItem*> items = findChildren<FxPanelItem*>();
    for(FxPanelItem* item : items){//find the first free slot to put the new plugin
        if(!item->containPlugin()){
            item->setPlugin(plugin);
            return;
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FxPanel::~FxPanel()
{
    //delete ui;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FxPanel::paintEvent(QPaintEvent* ){
    //default code to use stylesheets
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
