#include "LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include "ui_TrackGroupView.h"

#include "../MainController.h"
#include "Highligther.h"

#include "MainWindow.h"

#include <QPushButton>
#include <QMenu>

LocalTrackGroupView::LocalTrackGroupView(int index, MainWindow *mainFrame)
    :index(index), mainFrame(mainFrame), peakMeterOnly(false)
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

void LocalTrackGroupView::closePluginsWindows(){
    QList<LocalTrackView*> trackViews = getTracks();
    foreach (LocalTrackView* trackView, trackViews) {
        trackView->closeAllPlugins();
    }
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
    //PRESETS-----------------------------
    QAction* addPresetActionLoad = menu.addAction(QIcon(":/images/preset-load.png"), "Load preset");
    addPresetActionLoad->setDisabled(false);// so we can merge to master without confusion for the user until it works

    QAction* addPresetActionSave = menu.addAction(QIcon(":/images/preset-save.png"), "Save preset");
    addPresetActionSave->setDisabled(true);// so we can merge to master without confusion for the user until it works

    menu.addSeparator();
    //-------------------------------------
    //CHANNELS
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

    if(!mainFrame->isRunningAsVstPlugin()){//subchannels are disabled in VST Plugin
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
    }

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
    emit trackAdded();
}

void LocalTrackGroupView::setToWide(){
    if(trackViews.count() <= 1){//don't allow 2 wide subchannels
        foreach (BaseTrackView* trackView, this->trackViews) {
            trackView->setToWide();
        }
    }
}

void LocalTrackGroupView::setToNarrow(){
    foreach (BaseTrackView* trackView, this->trackViews) {
        trackView->setToNarrow();
    }
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
       emit trackRemoved();
    }
}

void LocalTrackGroupView::detachMainControllerInSubchannels(){
    foreach (BaseTrackView* view, trackViews) {
        (dynamic_cast<LocalTrackView*>(view))->detachMainController();
    }
}

void LocalTrackGroupView::on_removeChannelClicked(){
    mainFrame->removeChannelsGroup(mainFrame->getChannelGroupsCount()-1);
}
//+++++++++++++++++++++++++++++
void LocalTrackGroupView::setPeakMeterMode(bool peakMeterOnly){
    if(this->peakMeterOnly != peakMeterOnly){
        this->peakMeterOnly = peakMeterOnly;
        this->ui->topPanel->setVisible(!this->peakMeterOnly);
        foreach (BaseTrackView* baseView, trackViews) {
            dynamic_cast<LocalTrackView*> (baseView)->setPeakMetersOnlyMode(peakMeterOnly, mainFrame->isRunningInMiniMode() );
        }
        updateGeometry();
        adjustSize();
    }
}

void LocalTrackGroupView::togglePeakMeterOnlyMode(){
    setPeakMeterMode(!this->peakMeterOnly);
}
