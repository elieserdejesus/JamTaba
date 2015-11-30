#include "LocalTrackGroupView.h"
#include "LocalTrackView.h"
#include "ui_TrackGroupView.h"

#include "../MainController.h"
#include "Highligther.h"

#include "MainWindow.h"

#include <QPushButton>
#include <QMenu>
#include <QMouseEvent> //for menu
#include <QInputDialog>//for menu


LocalTrackGroupView::LocalTrackGroupView(int channelIndex, MainWindow *mainFrame)
    :index(channelIndex), mainFrame(mainFrame), peakMeterOnly(false), preparingToTransmit(false)
{
    toolButton = createToolButton();
    this->ui->topPanel->layout()->addWidget(toolButton);
    //this->installEventFilter(this);

    xmitButton = createXmitButton();
    this->layout()->addWidget(xmitButton);

    QObject::connect(toolButton, SIGNAL(clicked()), this, SLOT(on_toolButtonClicked()));
    QObject::connect(this->ui->groupNameField, SIGNAL(editingFinished()), this, SIGNAL(nameChanged()) );
    QObject::connect(xmitButton, SIGNAL(toggled(bool)), this, SLOT(on_xmitButtonClicked(bool)));

    xmitButton->setChecked(true);
}

LocalTrackGroupView::~LocalTrackGroupView()
{

}

void LocalTrackGroupView::setPreparingStatus(bool preparing){
    this->preparingToTransmit = preparing;
    xmitButton->setEnabled(!preparing);
    if(!isShowingPeakMeterOnly()){
        xmitButton->setText(preparing ? "Preparing" : "Transmiting" );
    }

    xmitButton->setProperty("preparing", QVariant(preparing));//change the button property to change stylesheet
    style()->unpolish(xmitButton); //force the computation of the new stylesheet for "preparing" state in QPushButton
    style()->polish(xmitButton);
}

void LocalTrackGroupView::on_xmitButtonClicked(bool checked){
    if(!preparingToTransmit){//users can't change xmit when is preparing
        setUnlightStatus(!checked);
        mainFrame->setTransmitingStatus(getChannelIndex(), checked);
    }
}

QPushButton* LocalTrackGroupView::createXmitButton(){
    QPushButton* toolButton = new QPushButton();
    toolButton->setObjectName("xmitButton");
    toolButton->setCheckable(true);
    toolButton->setText("Transmit");
    toolButton->setToolTip("Enable/disable your audio transmission for others");
    toolButton->setAccessibleDescription(toolTip());
    return toolButton;
}

QPushButton* LocalTrackGroupView::createToolButton(){
    QPushButton* toolButton = new QPushButton();
    toolButton->setObjectName("toolButton");
    toolButton->setText("");
    toolButton->setToolTip("Add or remove channels...");
    toolButton->setAccessibleDescription(toolTip());
    return toolButton;
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
bool LocalTrackGroupView::eventFilter(QObject *target, QEvent *event)
{
    bool val= QObject::eventFilter(target, event);

    QMenu * menu = dynamic_cast<QMenu*>(target);
    if(menu && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent * ev = (QMouseEvent *)event;
        if(ev)
        {
            if(ev->button() == Qt::RightButton)
            {
              on_RemovePresetClicked();
                ev->ignore();
                return true; // yes we filter the event
            }
        }
    }
    return val;
    /*qWarning() << event->type();
    if (event->type() == QEvent::Leave) {
        Highligther::getInstance()->stopHighlight();
    }
    return TrackGroupView::eventFilter(target, event);*/
}

void LocalTrackGroupView::on_toolButtonClicked()
{
    QMenu menu;



    //PRESETS-----------------------------

    //LOAD - using a submenu to list stored presets
    QMenu* loadPresetsSubmenu = new QMenu("Load preset");
    loadPresetsSubmenu->setIcon(QIcon(":/images/preset-load.png"));
    loadPresetsSubmenu->setDisabled(false);// so we can merge to master without confusion for the user until it works
    loadPresetsSubmenu->installEventFilter(this);// to deal with mouse buttons
    //adding a menu action for each stored preset
    Configurator *cfg= Configurator::getInstance();
    QStringList presetsNames=cfg->getPresetFilesNames(false);
    foreach(QString name,presetsNames )
    {
        //presetsNames.append(name);
        QAction* presetAction = loadPresetsSubmenu->addAction(name);
        presetAction->setData(name);//putting the preset name in the Action instance we can get this preset name inside event handler 'on_presetMenuActionClicked'
        QObject::connect(loadPresetsSubmenu, SIGNAL(triggered(QAction*)), this, SLOT(on_LoadPresetClicked(QAction*)));

    }

   /* foreach (QString presetName, presetsNames) {
        QAction* presetAction = loadPresetsSubmenu->addAction(presetName);
        presetAction->setData(presetName);//putting the preset name in the Action instance we can get this preset name inside event handler 'on_presetMenuActionClicked'
        QObject::connect(presetAction, SIGNAL(triggered(bool)), this, SLOT(on_LoadPresetClicked()));
    }*/
    menu.addMenu(loadPresetsSubmenu);

    //SAVE
    QAction* addPresetActionSave = menu.addAction(QIcon(":/images/preset-save.png"), "Save preset");
    addPresetActionSave->setDisabled(false);// so we can merge to master without confusion for the user until it works
    QObject::connect(addPresetActionSave, SIGNAL(triggered(bool)), this, SLOT(on_SavePresetClicked()));


    //RESET - in case of panic
    QAction* reset =  menu.addAction(QIcon(":/images/gear.png"),"Reset Preset");
    reset->setDisabled(false);// so we can merge to master without confusion for the user until it works
    QObject::connect(reset, SIGNAL(triggered()), this, SLOT(on_ResetPresetClicked()));

    menu.addSeparator();


    //-------------------------------------
    //CHANNELS

    menu.addSeparator();

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
        LocalTrackView* trackView = new LocalTrackView( trackViews.at(0)->getMainController(), getChannelIndex());
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

//PRESETS
void LocalTrackGroupView::on_LoadPresetClicked(QAction* a)
{
    mainFrame->getMainController()->loadPreset(a->data().toString());
    mainFrame->loadPresetToTrack();//that name is so good
}


void LocalTrackGroupView::on_SavePresetClicked()
{
    bool ok;
        QString text = QInputDialog::getText(this, tr("Save the preset ..."),
                                             tr("Preset name:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
        if (ok && !text.isEmpty())
        {text.append(".json");
        mainFrame->getMainController()->savePresets(mainFrame->getInputsSettings(),text);
        }

}

void LocalTrackGroupView::on_ResetPresetClicked()
{

   //qCDebug(jtConfigurator) << "************ PRESET RESET ***********";
   mainFrame->resetGroupChannel(this);
}

void LocalTrackGroupView::on_RemovePresetClicked()
{

    QStringList items=mainFrame->getMainController()->getPresetList();
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Remove preset"),
                                            tr("Preset:"), items, 0, false, &ok);

    //delete the file

    mainFrame->getMainController()->deletePreset(item);

}

//FOR TESTS ACTUALLY
void LocalTrackGroupView::on_presetMenuActionClicked()
{


}

//+++++++++++++++++++++++++++++
void LocalTrackGroupView::setPeakMeterMode(bool peakMeterOnly){
    if(this->peakMeterOnly != peakMeterOnly){
        this->peakMeterOnly = peakMeterOnly;
        this->ui->topPanel->setVisible(!this->peakMeterOnly);
        foreach (BaseTrackView* baseView, trackViews) {
            dynamic_cast<LocalTrackView*> (baseView)->setPeakMetersOnlyMode(peakMeterOnly, mainFrame->isRunningInMiniMode() );
        }
        xmitButton->setText(peakMeterOnly ? "X" : "Transmit");
        updateGeometry();
        adjustSize();
    }
}

void LocalTrackGroupView::togglePeakMeterOnlyMode(){
    setPeakMeterMode(!this->peakMeterOnly);
}
