#include "LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include "ui_TrackGroupView.h"

#include "../MainController.h"
#include "Highligther.h"

#include <QPushButton>
#include <QMenu>

LocalTrackGroupView::LocalTrackGroupView(int index)
    :index(index)
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

void LocalTrackGroupView::on_toolButtonClicked()
{
    QMenu menu;
    QAction* addSubchannelAction = menu.addAction(QIcon(":/images/more.png"), "Add subchannel");
    QObject::connect(addSubchannelAction, SIGNAL(triggered()), this, SLOT(onAddSubChannelClicked()));
    addSubchannelAction->setEnabled(trackViews.size() < MAX_SUB_CHANNELS);
    if(trackViews.size() > 1 ){
        menu.addSeparator();
        for (int i = 2; i <= trackViews.size(); ++i) {
            QAction* action = menu.addAction(QIcon(":/images/less.png"), "Remove subchannel " + QString::number(i));
            action->setData( i-1 );//use track view index as user data
        }
    }
    QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_toolButtonActionTriggered(QAction*)));
    QObject::connect(&menu, SIGNAL(hovered(QAction*)), this, SLOT(on_toolButtonActionHovered(QAction*)));
    menu.move( mapToGlobal( toolButton->pos() + QPoint(toolButton->width(), 0) ));
    menu.exec();
}

void LocalTrackGroupView::onAddSubChannelClicked(){
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

void LocalTrackGroupView::on_toolButtonActionHovered(QAction *action){
    if(action->data().isValid()){//only remove actions contains data
        int trackIndex = action->data().toInt();
        if(trackIndex >= 0 && trackIndex < trackViews.size()){
            Highligther::getInstance()->highlight(trackViews.at(trackIndex));
        }
    }
}

void LocalTrackGroupView::on_toolButtonActionTriggered(QAction *action){
    if(action->data().isValid()){//only remove actions contains data
        int trackIndex = action->data().toInt();
        if(trackIndex >= 0 && trackIndex < trackViews.size()){
            removeTrackView(trackIndex);
        }
    }
}
