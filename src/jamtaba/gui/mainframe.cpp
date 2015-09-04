#include "mainframe.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QList>
#include <QAction>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QFuture>

#include "PreferencesDialog.h"
#include "jamroomviewpanel.h"
#include "LocalTrackView.h"
#include "plugins/guis.h"
#include "FxPanel.h"
#include "pluginscandialog.h"
#include "NinjamRoomWindow.h"
#include "Highligther.h"
#include "ChatPanel.h"
#include "BusyDialog.h"

#include "../NinjamController.h"
#include "../ninjam/Server.h"
#include "../persistence/Settings.h"
#include "../JamtabaFactory.h"
#include "../audio/core/AudioDriver.h"
#include "../audio/vst/PluginFinder.h"
#include "../audio/core/plugins.h"
#include "../midi/MidiDriver.h"
#include "../MainController.h"
#include "../loginserver/LoginService.h"
#include "../Utils.h"
#include "../audio/RoomStreamerNode.h"

using namespace Audio;
using namespace Persistence;
using namespace Controller;
using namespace Ninjam;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::MainFrame(Controller::MainController *mainController, QWidget *parent)
    :
      QMainWindow(parent),
    busyDialog(0),
    mainController(mainController),
    pluginScanDialog(nullptr),
    ninjamWindow(nullptr),
    roomToJump(nullptr)
{
	ui.setupUi(this);
    initializeWindowState();//window size, maximization ...
    initializeLoginService();

    initializeVstFinderStuff();//vst finder...

    initializeMainControllerEvents();
    initializeMainTabWidget();
    timerID = startTimer(1000/50);

    //ui.menuPreferences
    QObject::connect(ui.menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(on_preferencesClicked(QAction*)));

    QObject::connect(mainController, SIGNAL(inputSelectionChanged(int)), this, SLOT(on_inputSelectionChanged(int)));

    QObject::connect( ui.localControlsCollapseButton, SIGNAL(clicked()), this, SLOT(on_localControlsCollapseButtonClicked()));

    QObject::connect(ui.xmitButton, SIGNAL(toggled(bool)), this, SLOT(on_xmitButtonClicked(bool)));


    QObject::connect( mainController->getRoomStreamer(), SIGNAL(error(QString)), this, SLOT(on_RoomStreamerError(QString)));

    ui.xmitButton->setChecked(mainController->isTransmiting());

    initializeLocalInputChannels();

    ui.chatArea->setVisible(false);//hide chat area until connect in a server to play

    ui.allRoomsContent->setLayout(new QVBoxLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0,0,6,0);
    ui.allRoomsContent->layout()->setSpacing(24);

    foreach (LocalTrackGroupView* channel, localChannels) {
        channel->refreshInputSelectionNames();
    }
}
//++++++++++++++++++++++++=
void MainFrame::on_localControlsCollapseButtonClicked(){
    foreach (LocalTrackGroupView* channel, localChannels) {
        channel->toggleFaderOnlyMode();
    }
    ui.labelSectionTitle->setVisible(!localChannels.first()->isFaderOnly());
    //ui.localControlsCollapseButton->setVisible(false);
    ui.xmitButton->setVisible(false);
    //adjustSize();
}
//++++++++++++++++++++++++=
Persistence::InputsSettings MainFrame::getInputsSettings() const{
    InputsSettings settings;
    foreach (LocalTrackGroupView* trackGroupView, localChannels) {
        trackGroupView->getTracks();
        Persistence::Channel channel(trackGroupView->getGroupName());
        foreach (LocalTrackView* trackView, trackGroupView->getTracks()) {
            Audio::LocalInputAudioNode* inputNode = trackView->getInputNode();
            Audio::ChannelRange inputNodeRange = inputNode->getAudioInputRange();
            int firstInput = inputNodeRange.getFirstChannel();
            int channelsCount = inputNodeRange.getChannels();
            int midiDevice = inputNode->getMidiDeviceIndex();
            int midiChannel = inputNode->getMidiChannelIndex();
            float gain = Utils::poweredGainToLinear( inputNode->getGain() );
            float pan = inputNode->getPan();
            bool muted = inputNode->isMuted();

            QList<const Audio::Plugin*> insertedPlugins = trackView->getInsertedPlugins();
            QList<Persistence::Plugin> plugins;
            foreach (const Audio::Plugin* p, insertedPlugins) {
                QByteArray serializedData = p->getSerializedData();
                plugins.append(Persistence::Plugin(p->getPath(), p->isBypassed(), serializedData));
            }
            Persistence::Subchannel sub(firstInput, channelsCount, midiDevice, midiChannel, gain, pan, muted, plugins);

            channel.subChannels.append(sub);
        }
        settings.channels.append(channel);
    }
    return settings;
}
//++++++++++++++++++++++++=
void MainFrame::stopCurrentRoomStream(){
    if(mainController->isPlayingRoomStream()){
        long long roomID = mainController->getCurrentStreamingRoomID();
        if(roomViewPanels[roomID]){
            roomViewPanels[roomID]->clearPeaks(true);
        }
    }
    mainController->stopRoomStream();
}

void MainFrame::showMessageBox(QString title, QString text, QMessageBox::Icon icon){
    QMessageBox* messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    messageBox->setIcon(icon);
    messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    messageBox->show();
}

void MainFrame::on_RoomStreamerError(QString msg){
    stopCurrentRoomStream();
    showMessageBox("Error!", msg, QMessageBox::Critical);
}

//++++++++++++++++++++++++=
//void MainFrame::on_toolButtonClicked(){
//    QMenu menu;
//    QAction* addChannelAction = menu.addAction(QIcon(":/images/more.png"), "Add channel");
//    QObject::connect(addChannelAction, SIGNAL(triggered()), this, SLOT(on_addChannelClicked()));
//    addChannelAction->setEnabled(localChannels.size() < 2);//at this moment users can't create more channels
//    if(localChannels.size() > 1){
//        menu.addSeparator();
//        for (int i = 2; i <= localChannels.size(); ++i) {
//            QString channelName = localChannels.at(i-1)->getGroupName();
//            QAction* action = menu.addAction(QIcon(":/images/less.png"), "Remove channel \"" + channelName + "\"");
//            action->setData( i-1 );//use channel index as action data
//        }
//    }
//    QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_toolButtonMenuActionTriggered(QAction*)));
//    QObject::connect(&menu, SIGNAL(hovered(QAction*)), this, SLOT(on_toolButtonMenuActionHovered(QAction*)));
//    QPoint pos = ui.toolButton->parentWidget()->mapToGlobal(ui.toolButton->pos() + QPoint(ui.toolButton->width(), 0));

//    menu.move( pos );
//    menu.exec();
//}

void MainFrame::removeChannelsGroup(int channelIndex){
    if(localChannels.size() > 1){//the first channel group can't be removed
        if(channelIndex >= 0 && channelIndex < localChannels.size()){
            TrackGroupView* channel = localChannels.at(channelIndex);
            ui.localTracksLayout->removeWidget(channel);
            localChannels.removeAt(channelIndex);
            channel->deleteLater();

            mainController->sendRemovedChannelMessage(channelIndex);
            update();
        }
    }
}

//void MainFrame::on_toolButtonMenuActionTriggered(QAction *action){
//    if(action->data().isValid()){//only remove actions contains valid data (the channel index)
//        int channelIndex = action->data().toInt();
//        removeLocalChannel(channelIndex);
//    }
//}

void MainFrame::highlightChannelGroup(int index) const{
    if(index >= 0 && index < localChannels.size()){
        Highligther::getInstance()->highlight(localChannels.at(index));
    }
}

void MainFrame::addChannelsGroup(QString name){
    int channelIndex = localChannels.size();
    addLocalChannel( channelIndex, name, true);
    mainController->updateInputTracksRange();
    mainController->sendNewChannelsNames(getChannelsNames());
    if(mainController->isPlayingInNinjamRoom()){
        mainController->getNinjamController()->scheduleEncoderChangeForChannel(channelIndex);//create an encoder for this channel in next interval
    }
}

//++++++++++++++++++++++++=
void MainFrame::initializeMainTabWidget(){
    //the rooms list tab bar is not closable
    ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

    connect( ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(on_tabCloseRequest(int)));
    connect( ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabChanged(int)));
}

void MainFrame::initializeMainControllerEvents(){
    QObject::connect(mainController, SIGNAL(enteredInRoom(Login::RoomInfo)), this, SLOT(on_enteredInRoom(Login::RoomInfo)));
    QObject::connect(mainController, SIGNAL(exitedFromRoom(bool)), this, SLOT(on_exitedFromRoom(bool)));
}

void MainFrame::initializeVstFinderStuff(){
    const Vst::PluginFinder& pluginFinder = mainController->getPluginFinder();
    QObject::connect(&pluginFinder, SIGNAL(scanStarted()), this, SLOT(onPluginScanStarted()));
    QObject::connect(&pluginFinder, SIGNAL(scanFinished()), this, SLOT(onPluginScanFinished()));
    QObject::connect(&pluginFinder, SIGNAL(vstPluginFounded(QString,QString,QString)), this, SLOT(onPluginFounded(QString,QString,QString)));

    QStringList vstPaths = mainController->getSettings().getVstPluginsPaths();
    if(vstPaths.empty()){//no vsts in database cache, try scan
        if(mainController->getSettings().getVstScanPaths().isEmpty()){
            mainController->addDefaultVstScanPath();
        }
        mainController->scanPlugins();
    }
    else{//use vsts stored in settings file
        mainController->initializePluginsList(vstPaths);
        onPluginScanFinished();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::on_channelNameChanged(){
    mainController->sendNewChannelsNames(getChannelsNames());
}

LocalTrackGroupView *MainFrame::addLocalChannel(int channelGroupIndex, QString channelName, bool createFirstSubchannel){
    LocalTrackGroupView* localChannel = new LocalTrackGroupView(channelGroupIndex, this);
    QObject::connect(localChannel, SIGNAL(nameChanged()), this, SLOT(on_channelNameChanged()));
    localChannels.append( localChannel );
    localChannel->setGroupName(channelName);
    ui.localTracksLayout->addWidget(localChannel);

    if(createFirstSubchannel){
        LocalTrackView* localTrackView = new LocalTrackView(this->mainController, channelGroupIndex);
        localChannel->addTrackView( localTrackView );

        if(localChannels.size() > 1){
            localTrackView->setToNoInput();
        }
        else{
            localTrackView->refreshInputSelectionName();
        }
    }
    return localChannel;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//PluginLoader::PluginLoader(Controller::MainController* mainController, Persistence::Plugin plugin, LocalTrackView* trackView)
//    :plugin(plugin), trackView(trackView), mainController(mainController){

//}

//PluginLoader::~PluginLoader(){
//    qWarning() << "destrutor Plugin Loader";
//}

//void PluginLoader::load(){
//    qWarning() << "loading plugin";
//    QObject::connect(&futureWatcher, SIGNAL(finished()), this, SLOT(on_futureWatcherFinished()));
//    QFuture<Audio::Plugin*> result = QtConcurrent::run(this, &loadPlugin);
//    futureWatcher.setFuture(result);
//}

//void PluginLoader::on_futureWatcherFinished(){
//    qWarning() << "plugin loaded, adding in trackview";
//    Audio::Plugin* pluginInstance = futureWatcher.future().result();
//    trackView->addPlugin(pluginInstance, this->plugin.bypassed);
//    deleteLater();
//}

//Audio::Plugin* PluginLoader::loadPlugin(){
//    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugin.path);
//    Audio::PluginDescriptor descriptor(pluginName, "VST", plugin.path );
//    Audio::Plugin* pluginInstance = mainController->addPlugin(trackView->getInputIndex(), descriptor);// subChannelView->getInputIndex(), descriptor);
//    pluginInstance->restoreFromSerializedData( plugin.data);
//    return pluginInstance;
//}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::restorePluginsList(){
    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        LocalTrackGroupView* channelView = localChannels.at(channelIndex);
        if(channelView){
            int subChannelIndex = 0;
            QList<LocalTrackView*> tracks = channelView->getTracks();
            foreach (Persistence::Subchannel subChannel, channel.subChannels) {
                LocalTrackView* subChannelView = tracks.at(subChannelIndex);
                if(subChannelView){
                    //create the plugins list
                    foreach (Persistence::Plugin plugin, subChannel.plugins) {
                        QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugin.path);
                        Audio::PluginDescriptor descriptor(pluginName, "VST", plugin.path );
                        Audio::Plugin* pluginInstance = mainController->addPlugin(subChannelView->getInputIndex(), descriptor);
                        pluginInstance->restoreFromSerializedData( plugin.data);
                        subChannelView->addPlugin(pluginInstance, plugin.bypassed);
                        QApplication::processEvents();

                        //PluginLoader* loader = new PluginLoader(mainController, plugin, subChannelView);
                        //loader->load();
                    }
                }
                subChannelIndex++;
            }
        }
        channelIndex++;
    }
}

void MainFrame::initializeLocalInputChannels(){
    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        LocalTrackGroupView* channelView = addLocalChannel(channelIndex, channel.name, channel.subChannels.isEmpty());
        foreach (Persistence::Subchannel subChannel, channel.subChannels) {
            LocalTrackView* subChannelView = new LocalTrackView( mainController, channelIndex, subChannel.gain, subChannel.pan, subChannel.muted);
            channelView->addTrackView(subChannelView);
            if(subChannel.midiDevice >= 0){//using midi
                //check if midiDevice index is valid
                if(subChannel.midiDevice < mainController->getMidiDriver()->getMaxInputDevices()){
                    mainController->setInputTrackToMIDI( subChannelView->getInputIndex(), subChannel.midiDevice, subChannel.midiChannel);
                }
                else{
                    if(mainController->getMidiDriver()->hasInputDevices()){
                        //use the first midi device and all channels
                        mainController->setInputTrackToMIDI(subChannelView->getInputIndex(), 0, -1);
                    }
                    else{
                        mainController->setInputTrackToNoInput(subChannelView->getInputIndex());
                    }
                }
            }
            else if(subChannel.channelsCount <= 0){
                mainController->setInputTrackToNoInput(subChannelView->getInputIndex());
            }
            else if(subChannel.channelsCount == 1){
                mainController->setInputTrackToMono(subChannelView->getInputIndex(), subChannel.firstInput);
            }
            else{
                mainController->setInputTrackToStereo(subChannelView->getInputIndex(), subChannel.firstInput);
            }

            //create the plugins list
//            foreach (Persistence::Plugin plugin, subChannel.plugins) {
//                QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugin.path);
//                Audio::PluginDescriptor descriptor(pluginName, "VST", plugin.path );
//                Audio::Plugin* pluginInstance = mainController->addPlugin(subChannelView->getInputIndex(), descriptor);
//                pluginInstance->restoreFromSerializedData( plugin.data);
//                subChannelView->addPlugin(pluginInstance, plugin.bypassed);

//                //PluginLoader* loader = new PluginLoader(mainController, plugin, subChannelView);
//                //loader->load();
//            }
        }
        channelIndex++;
    }
    if(channelIndex == 0){//no channels in settings file or no settings file...
        addLocalChannel(0, "your channel", true);
    }
}

void MainFrame::initializeLoginService(){
    Login::LoginService* loginService = this->mainController->getLoginService();
    connect( loginService, SIGNAL(roomsListAvailable(QList<Login::RoomInfo>)), this, SLOT(on_roomsListAvailable(QList<Login::RoomInfo>)));
    connect( loginService, SIGNAL(incompatibilityWithServerDetected()), this, SLOT(on_incompatibilityWithServerDetected()));
    connect( loginService, SIGNAL(newVersionAvailableForDownload()), this, SLOT(on_newVersionAvailableForDownload()));
    connect( loginService, SIGNAL(errorWhenConnectingToServer()), this, SLOT(on_errorConnectingToServer()));
}

void MainFrame::initializeWindowState(){
    if(mainController->getSettings().windowWasMaximized()){
        setWindowState(Qt::WindowMaximized);
    }
    else{
        QPointF location = mainController->getSettings().getLastWindowLocation();
        QDesktopWidget* desktop = QApplication::desktop();
        int desktopWidth = desktop->width();
        int desktopHeight = desktop->height();
        int x = desktopWidth * location.x();
        int y = desktopHeight * location.y();
        this->move(x, y);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::on_tabCloseRequest(int index){
    if(index > 0){//the first tab is not closable
        showBusyDialog("disconnecting ...");
        if(mainController->getNinjamController()->isRunning()){

            mainController->stopNinjamController();
        }
    }
}

void MainFrame::on_tabChanged(int index){
    if(index > 0){//click in room tab?
        if(mainController->isPlayingInNinjamRoom() && mainController->isPlayingRoomStream()){
            stopCurrentRoomStream();
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::showBusyDialog(){
    showBusyDialog("");
}

void MainFrame::showBusyDialog(QString message){
    busyDialog.setParent(this);
    centerBusyDialog();
    busyDialog.show(message);
}

void MainFrame::hideBusyDialog(){
    busyDialog.hide();
}


void MainFrame::centerBusyDialog(){
    int newX = ui.contentPanel->width()/2 - busyDialog.width()/2;
    int newY = ui.contentPanel->height()/2 - busyDialog.height()/2;
    busyDialog.move(newX + ui.contentPanel->x(), newY + ui.contentPanel->y());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void MainFrame::on_removingPlugin(Audio::Plugin *plugin){
//    PluginWindow* window = PluginWindow::getWindow(this, plugin);
//    if(window){
//        window->close();
//    }
//    mainController->removePlugin(plugin);

//}

//void MainFrame::on_editingPlugin(Audio::Plugin *plugin){
//    showPluginGui(plugin);
//}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//PluginGui* MainFrame::createPluginView(Plugin::PluginDescriptor* d, Audio::Plugin* plugin){
//    if(d->getGroup() == "Jamtaba"){
//        if(d->getName() == "Delay"){
//            return new DelayGui((Plugin::JamtabaDelay*)plugin);
//        }
//    }
//    return nullptr;
//}

//++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//esses eventos deveriam ser tratados no controller

bool MainFrame::jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2){
     return r1.getNonBotUsersCount() > r2.getNonBotUsersCount();
}

void MainFrame::on_incompatibilityWithServerDetected(){
    hideBusyDialog();
    QString text = "Your Jamtaba version is not compatible with previous versions! Please download the new <a href='http://www.jamtaba.com'>Jamtaba</a> version!";
    QMessageBox::warning(this, "Compatibility error!", text );
    close();
}

void MainFrame::on_errorConnectingToServer(){
    hideBusyDialog();
    QMessageBox::warning(this, "Error!", "Error connecting in Jamtaba server!");
    close();
}

void MainFrame::on_newVersionAvailableForDownload(){
    hideBusyDialog();
    QString text = "A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!";
    QMessageBox::information(this, "New Jamtaba version available!", text );
}

void MainFrame::on_roomsListAvailable(QList<Login::RoomInfo> publicRooms){
    hideBusyDialog();
    qSort(publicRooms.begin(), publicRooms.end(), jamRoomLessThan);
    foreach(Login::RoomInfo roomInfo, publicRooms ){
        if(roomInfo.getType() == Login::RoomTYPE::NINJAM){//skipping other rooms at moment
            if(roomViewPanels.contains(roomInfo.getID())){
                JamRoomViewPanel* roomViewPanel = roomViewPanels[roomInfo.getID()];
                roomViewPanel->refreshUsersList(roomInfo);
            }
            else{
                JamRoomViewPanel* roomViewPanel = new JamRoomViewPanel(roomInfo, ui.allRoomsContent, mainController);
                roomViewPanels.insert(roomInfo.getID(), roomViewPanel);
                ui.allRoomsContent->layout()->addWidget(roomViewPanel);
                connect( roomViewPanel, SIGNAL(startingListeningTheRoom(Login::RoomInfo)),
                         this, SLOT(on_startingRoomStream(Login::RoomInfo)));
                connect( roomViewPanel, SIGNAL(finishingListeningTheRoom(Login::RoomInfo)),
                         this, SLOT(on_stoppingRoomStream(Login::RoomInfo)));
                connect( roomViewPanel, SIGNAL(enteringInTheRoom(Login::RoomInfo)),
                         this, SLOT(on_enteringInRoom(Login::RoomInfo)));
            }
        }
    }
}

//+++++++++++++++++++++++++++++++++++++
void MainFrame::on_startingRoomStream(Login::RoomInfo roomInfo){
    //clear all plots
    foreach (JamRoomViewPanel* viewPanel, this->roomViewPanels.values()) {
        viewPanel->clearPeaks(roomInfo.getID() != viewPanel->getRoomInfo().getID());
    }

    if(roomInfo.hasStream()){//just in case...
        mainController->playRoomStream(roomInfo);
    }
}

void MainFrame::on_stoppingRoomStream(Login::RoomInfo roomInfo){
    Q_UNUSED(roomInfo)
    stopCurrentRoomStream();
}

QStringList MainFrame::getChannelsNames() const{
    QStringList channelsNames;
    foreach (LocalTrackGroupView* channel, localChannels) {
        channelsNames.append(channel->getGroupName());
    }
    return channelsNames;
}

//user trying enter in a room
void MainFrame::on_enteringInRoom(Login::RoomInfo roomInfo){

    //stop room stream before enter in a room
    if(mainController->isPlayingRoomStream()){
        long long roomID = mainController->getCurrentStreamingRoomID();
        if(roomViewPanels[roomID]){
            roomViewPanels[roomID]->clearPeaks(true);
        }
        mainController->stopRoomStream();
    }

    if(!mainController->userNameWasChoosed()){
        bool ok;
        QString lastUserName = mainController->getUserName();
        QString newUserName = QInputDialog::getText(this, "", "Enter your user name:", QLineEdit::Normal, lastUserName , &ok, Qt::FramelessWindowHint);
        //newUserName = QString(newUserName.toLatin1());
        if (ok && !newUserName.isEmpty()){
           mainController->setUserName(newUserName);
           setWindowTitle("Jamtaba (" + mainController->getUserName() + ")");
        }
    }

    if(mainController->isPlayingInNinjamRoom()){
        mainController->stopNinjamController();//disconnect from current ninjam server
        //store the room to jump and wait for disconnectedFromServer event to connect in this new room
        roomToJump = new Login::RoomInfo(roomInfo);
    }
    else if(mainController->userNameWasChoosed()){
        showBusyDialog("Connecting in " + roomInfo.getName() + " ...");
        mainController->enterInRoom(roomInfo, getChannelsNames());
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//estes eventos são disparados pelo controlador depois que já aconteceu a conexão com uma das salas

void MainFrame::on_enteredInRoom(Login::RoomInfo roomInfo){

    hideBusyDialog();

//    if(ninjamWindow){
//        ninjamWindow->deleteLater();
//    }
    ninjamWindow = new NinjamRoomWindow(ui.tabWidget, roomInfo, mainController);
    int index = ui.tabWidget->addTab(ninjamWindow, roomInfo.getName());
    ui.tabWidget->setCurrentIndex(index);

    //add metronome track view
//    float metronomeInitialGain = mainController->getSettings().getMetronomeGain();
//    float metronomeInitialPan = mainController->getSettings().getMetronomePan();
//    bool metronomeInitialMuteStatus = mainController->getSettings().getMetronomeMuteStatus();
//    metronomeTrackView = new MetronomeTrackView(mainController, NinjamController::METRONOME_TRACK_ID, metronomeInitialGain, metronomeInitialPan, metronomeInitialMuteStatus );

    //ui.localTracksLayout->addWidget(metronomeTrackView);

    //add the chat panel in main window
    ChatPanel* chatPanel = ninjamWindow->getChatPanel();
    ui.chatTabWidget->addTab(chatPanel, QIcon(":/images/ninja.png"), roomInfo.getName());

    //show chat area
    ui.chatArea->setVisible(true);

    ui.leftPanel->adjustSize();
    //ui.leftPanel->setMinimumWidth(500);
}

void MainFrame::on_exitedFromRoom(bool normalDisconnection){
    hideBusyDialog();

    //remove the jam room tab (the last tab)
    if(ui.tabWidget->count() > 1){
        ui.tabWidget->widget(1)->deleteLater();//delete the room window
        ui.tabWidget->removeTab(1);
    }

    if(ui.chatTabWidget->count() > 0){
        ui.chatTabWidget->widget(0)->deleteLater();
        ui.chatTabWidget->removeTab(0);
    }

    //hide chat area
    ui.chatArea->setVisible(false);

    if(!normalDisconnection){
        QMessageBox::warning(this, "Warning", "Disconnected from server!", QMessageBox::NoButton, QMessageBox::NoButton);
    }
    else{
        if(roomToJump){//waiting the disconnection to connect in a new room?
            showBusyDialog("Connecting in " + roomToJump->getName());
            mainController->enterInRoom(*roomToJump, getChannelsNames());
            delete roomToJump;
            roomToJump = nullptr;
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::timerEvent(QTimerEvent *){

//    static long lastBeatUpdate = QDateTime::currentMSecsSinceEpoch();
//    static int beat = 0;
//    long ellapsed = QDateTime::currentMSecsSinceEpoch() - lastBeatUpdate;
//    if(ellapsed >= 500){
//        lastBeatUpdate = QDateTime::currentMSecsSinceEpoch();
//        beat = (beat + 1) % ui.circularProgress->getBeatsPerInterval();
//        ui.circularProgress->setCurrentBeat(beat);
//    }

    //update local input track peaks
    foreach (TrackGroupView* channel, localChannels) {
        channel->updatePeaks();
    }

    //update metronome peaks
    if(mainController->isPlayingInNinjamRoom()){

        //update tracks peaks
        if(ninjamWindow){
            ninjamWindow->updatePeaks();
        }
    }

    //update room stream plot
    if(mainController->isPlayingRoomStream()){
          long long roomID = mainController->getCurrentStreamingRoomID();
          JamRoomViewPanel* roomView = roomViewPanels[roomID];
          if(roomView){
              Audio::AudioPeak peak = mainController->getRoomStreamPeak();
              roomView->addPeak(peak.getMax());
          }
    }
}

//++++++++++++=
void MainFrame::on_xmitButtonClicked(bool checked){
    foreach (LocalTrackGroupView* localChannel, localChannels) {
        localChannel->setUnlightStatus(!checked);
    }
    mainController->setTransmitingStatus(checked);



}

//++++++++++++=

void MainFrame::resizeEvent(QResizeEvent *){
    if(busyDialog.isVisible()){
        centerBusyDialog();
    }
}

void MainFrame::changeEvent(QEvent *ev){
    if(ev->type() == QEvent::WindowStateChange){
        mainController->storeWindowSettings(isMaximized(), computeLocation() );
    }
    ev->accept();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QPointF MainFrame::computeLocation() const{
    QRect screen = QApplication::desktop()->screenGeometry();
    float x = (float)this->pos().x()/screen.width();
    float y = (float)this->pos().y()/screen.height();
    return QPointF(x, y);
}

void MainFrame::closeEvent(QCloseEvent *)
 {
    //qDebug() << "MainFrame::closeEvent";
    if(mainController != NULL){
        mainController->stop();
    }
    mainController->storeWindowSettings(isMaximized(), computeLocation() );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::showEvent(QShowEvent *)
{
    qWarning() << "Thread da GUI: " << QThread::currentThreadId();

    int availableDevices = mainController->getAudioDriver()->getDevicesCount();
    if(!mainController->isStarted() && availableDevices > 0){
        showBusyDialog("Loading rooms list ...");
        mainController->start();

        //wait 50 ms before resotre the plugins list to avoid freeze the GUI in hidden state while plugins are loaded
        QTimer::singleShot(50, this, &MainFrame::restorePluginsList);
    }
    else{
        QMessageBox::critical(this, "ERROR", "No audio device!");
        close();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::~MainFrame()
{
    qDebug() << "Main frame desctructor!";
    killTimer(timerID);
    qDebug() << "Main frame timer killed!";

    //delete peakMeter;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void MainFrame::on_freshDataReceivedFromLoginServer(const Login::LoginServiceParser &response){
//    if(ui.allRoomsContent->layout() == 0){
//        QLayout* layout = new QVBoxLayout(ui.allRoomsContent);
//        layout->setSpacing(20);
//        ui.allRoomsContent->setLayout(layout);
//    }
//    foreach (Model::AbstractJamRoom* room, response.getRooms()) {
//        QWidget* widget = new JamRoomViewPanel(room, this->ui.allRoomsContent);
//        ui.allRoomsContent->layout()->addWidget(widget);

//    }
//}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// preferences menu
void MainFrame::on_preferencesClicked(QAction* action)
{
    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    AudioDriver* audioDriver = mainController->getAudioDriver();
    audioDriver->stop();
    midiDriver->stop();

    stopCurrentRoomStream();

    PreferencesDialog dialog(mainController, this);
    if(action == ui.actionAudioPreferences){
        dialog.selectAudioTab();
    }
    else if(action == ui.actionMidiPreferences){
        dialog.selectMidiTab();
    }
    else if(action == ui.actionVstPreferences){
        dialog.selectVstPluginsTab();
    }
    else{
        dialog.selectRecordingTab();
    }
    connect(&dialog, SIGNAL(ioPreferencesChanged(QList<bool>,int,int,int,int,int,int,int)), this, SLOT(on_IOPreferencesChanged(QList<bool>,int,int,int,int,int,int,int)));
    int result = dialog.exec();
    if(result == QDialog::Rejected){
        midiDriver->start();//restart audio and midi drivers if user cancel the preferences menu
        audioDriver->start();
    }

    //audio driver parameters are changed in on_IOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.


}

void MainFrame::on_IOPreferencesChanged(QList<bool> midiInputsStatus, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize){
    //qDebug() << "midi device: " << midiDeviceIndex << endl;
    //bool midiDeviceChanged =  midiDeviceIndex



#ifdef _WIN32
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    audioDriver->setProperties(audioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#else
    //preciso de um outro on_audioIoPropertiesChanged que me dê o input e o output device
    //audioDriver->setProperties(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#endif
    mainController->storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, audioDevice, sampleRate, bufferSize, midiInputsStatus);

    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    midiDriver->setInputDevicesStatus(midiInputsStatus);

    mainController->updateInputTracksRange();

    foreach (LocalTrackGroupView* channel, localChannels) {
        channel->refreshInputSelectionNames();
    }



    mainController->getMidiDriver()->start();
    mainController->getAudioDriver()->start();
}

//input selection changed by user or by system
void MainFrame::on_inputSelectionChanged(int inputTrackIndex){
    foreach (LocalTrackGroupView* channel, localChannels) {
        channel->refreshInputSelectionName(inputTrackIndex);
    }
    //localTrackGroupView->refreshInputSelectionName();
}

//plugin finder events
void MainFrame::onPluginScanStarted(){
    if(!pluginScanDialog){
        pluginScanDialog = new PluginScanDialog(this);
    }
    pluginScanDialog->show();
}

void MainFrame::onPluginScanFinished(){
    if(pluginScanDialog){
        pluginScanDialog->close();
    }
    delete pluginScanDialog;
    pluginScanDialog = nullptr;

}

void MainFrame::onPluginFounded(QString name, QString group, QString path){
    Q_UNUSED(name);
    Q_UNUSED(group);
    if(pluginScanDialog){
        pluginScanDialog->setText(path);
    }
}




