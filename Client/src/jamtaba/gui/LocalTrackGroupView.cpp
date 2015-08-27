#include "LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include "ui_TrackGroupView.h"

#include "../MainController.h"
#include "Highligther.h"

#include "MainFrame.h"

#include <QPushButton>
#include <QMenu>

LocalTrackGroupView::LocalTrackGroupView(int index, MainFrame *mainFrame)
    :index(index), mainFrame(mainFrame)
{
    toolButton = new QPushButton();
    toolButton->setObjectName("toolButton");
    toolButton->setText("");
    toolButton->setToolTip("Add or remove channels...");
    this->ui->topPanel->layout()->addWidget(toolButton);

    QObject::connect(toolButton, SIGNAL(clicked()), this, SLOT(on_toolButtonClicked()));
    QObject::connect(this->ui->groupNameField, SIGNAL(editingFinished()), this, SIGNAL(nameChanged()) );
}

LocalTrackGroupView::~LocalTrackGroupView()
{

}


QList<LocalTrackView*> LocalTrackGroupView::getTracks() const{
    QList<LocalTrackView*> tracks;
    foreach (BaseTrackView* baseView, trackViews) {
        tracks.append( dynamic_cast<LocalTrackView*>( baseView));
    }
    return tracks;
}

void LocalTrackGroupView::refreshInputSelectionName(int inputTrackIndex){
    QList<LocalTrackView*> tracks = getTracks();
    foreach (LocalTrackView* trackView, tracks) {
        if(trackView->getInputIndex() == inputTrackIndex){
            trackView->refreshInputSelectionName();
        }
    }
}

void LocalTrackGroupView::refreshInputSelectionNames(){
    QList<LocalTrackView*> tracks = getTracks();
    foreach (LocalTrackView* trackView, tracks) {
        trackView->refreshInputSelectionName();
    }
}

void LocalTrackGroupView::on_addChannelClicked(){
    mainFrame->addChannelsGroup("new channel");
}

//NOT WORKING
bool LocalTrackGroupView::eventFilter(QObject *target, QEvent *event){
    qWarning() << event->type();
    if (event->type() == QEvent::Leave) {
        Highligther::getInstance()->stopHighlight();
    }
    return TrackGroupView::eventFilter(target, event);
}

void LocalTrackGroupView::on_toolButtonClicked(){
    QMenu menu;

    QAction* addChannelAction = menu.addAction(QIcon(":/images/more.png"), "Add channel");
    QObject::connect(addChannelAction, SIGNAL(triggered()), this, SLOT(on_addChannelClicked()));
    addChannelAction->setEnabled(mainFrame->getChannelGroupsCount() < 2);//at this moment users can't create more channels
    if(mainFrame->getChannelGroupsCount() > 1){
        //menu.addSeparator();
        for (int i = 2; i <= mainFrame->getChannelGroupsCount(); ++i) {
            QString channelName = mainFrame->getChannelGroupName(i-1);
            QAction* action = menu.addAction(QIcon(":/images/less.png"), "Remove channel \"" + channelName + "\"");
            action->setData( i-1 );//use channel index as action data
            QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_removeChannelClicked()));
            QObject::connect(action, SIGNAL(hovered()), this, SLOT(on_removeChannelHovered()));
            action->installEventFilter(this);
        }
    }

    menu.addSeparator();

    QAction* addSubchannelAction = menu.addAction(QIcon(":/images/more.png"), "Add subchannel");
    QObject::connect(addSubchannelAction, SIGNAL(triggered()), this, SLOT(on_addSubChannelClicked()));
    addSubchannelAction->setEnabled(trackViews.size() < MAX_SUB_CHANNELS);
    if(trackViews.size() > 1 ){
        //menu.addSeparator();
        for (int i = 2; i <= trackViews.size(); ++i) {
            QAction* action = menu.addAction(QIcon(":/images/less.png"), "Remove subchannel " + QString::number(i));
            action->setData( i-1 );//use track view index as user data
            QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(on_removeSubChannelClicked()));
            QObject::connect(action, SIGNAL(hovered()), this, SLOT(on_removeSubchannelHovered()));
            action->installEventFilter(this);
        }
    }
    //QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_RemoveSubChannelTriggered(QAction*)));
    //QObject::connect(&menu, SIGNAL(hovered(QAction*)), this, SLOT(on_removeSubchannelHovered(QAction*)));
    menu.move( mapToGlobal( toolButton->pos() + QPoint(toolButton->width(), 0) ));
    menu.exec();
}

void LocalTrackGroupView::on_addSubChannelClicked(){
    if(!trackViews.isEmpty()){
        LocalTrackView* trackView = new LocalTrackView( trackViews.at(0)->getMainController(), getIndex());
        addTrackView(trackView);
        trackView->getMainController()->setInputTrackToNoInput(trackView->getInputIndex());
    }
}

void LocalTrackGroupView::addTrackView(BaseTrackView *trackView){
    if(trackViews.size() >= MAX_SUB_CHANNELS){
        return;
    }
    TrackGroupView::addTrackView(trackView);
}

void LocalTrackGroupView::on_removeChannelHovered(){
    int channelGroupIndex = 1;//just the second channel can be highlighted at moment
    if(channelGroupIndex >= 0 && channelGroupIndex < mainFrame->getChannelGroupsCount()){
        mainFrame->highlightChannelGroup(channelGroupIndex);
    }
}

void LocalTrackGroupView::on_removeSubchannelHovered(){
    int subChannelIndex = 1;//just the second subchannel can be highlighted at moment
    if(subChannelIndex >= 0 && subChannelIndex < trackViews.size()){
        Highligther::getInstance()->highlight(trackViews.at(subChannelIndex));
    }
}

void LocalTrackGroupView::on_removeSubChannelClicked(){
    if(trackViews.size() > 1){//can't remove the default/first subchannel
       removeTrackView(1);//always remove the second channel
    }
}

void LocalTrackGroupView::on_removeChannelClicked(){
    mainFrame->removeChannelsGroup(mainFrame->getChannelGroupsCount()-1);
}
