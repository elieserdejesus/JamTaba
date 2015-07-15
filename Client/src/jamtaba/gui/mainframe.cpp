#include "MainFrame.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QList>
#include <QAction>
#include <QMessageBox>

#include "PreferencesDialog.h"
#include "JamRoomViewPanel.h"
#include "LocalTrackView.h"
#include "LocalTrackGroupView.h"
#include "plugins/guis.h"
#include "FxPanel.h"
#include "pluginscandialog.h"
#include "NinjamRoomWindow.h"
#include "MetronomeTrackView.h"
#include "Highligther.h"
#include "ChatPanel.h"
#include "BusyDialog.h"

#include "../NinjamJamRoomController.h"
#include "../ninjam/Server.h"
#include "../persistence/ConfigStore.h"
#include "../JamtabaFactory.h"
#include "../audio/core/AudioDriver.h"
#include "../audio/vst/PluginFinder.h"
#include "../audio/core/plugins.h"
#include "../midi/MidiDriver.h"
#include "../MainController.h"
#include "../loginserver/LoginService.h"

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
    metronomeTrackView(nullptr),
    ninjamWindow(nullptr)
{
	ui.setupUi(this);
    initializeWindowState();//window size, maximization ...
    initializeLoginService();
    initializeLocalTrackView();
    initializeVstFinderStuff();//vst finder...

    initializeMainControllerEvents();
    initializeMainTabWidget();
    timerID = startTimer(1000/50);

    QObject::connect(ui.menuAudioPreferences, SIGNAL(triggered()), this, SLOT(on_preferencesClicked()));
    QObject::connect(mainController, SIGNAL(inputSelectionChanged()), this, SLOT(on_inputSelectionChanged()));

    QObject::connect( ui.toolButton, SIGNAL(clicked()), this, SLOT(on_toolButtonClicked()));
}
//++++++++++++++++++++++++=
void MainFrame::on_toolButtonClicked(){
    QMenu menu;
    QAction* addChannelAction = menu.addAction(QIcon(":/images/more.png"), "Add channel");
    QObject::connect(addChannelAction, SIGNAL(triggered()), this, SLOT(on_addChannelClicked()));
    if(localChannels.size() > 1){
        menu.addSeparator();
        for (int i = 2; i <= localChannels.size(); ++i) {
            QString channelName = localChannels.at(i-1)->getGroupName();
            QAction* action = menu.addAction(QIcon(":/images/less.png"), "Remove channel \"" + channelName + "\"");
            action->setData( i-1 );//use channel index as action data
        }
    }
    QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(on_toolButtonMenuActionTriggered(QAction*)));
    QObject::connect(&menu, SIGNAL(hovered(QAction*)), this, SLOT(on_toolButtonMenuActionHovered(QAction*)));
    QPoint pos = ui.toolButton->parentWidget()->mapToGlobal(ui.toolButton->pos() + QPoint(ui.toolButton->width(), 0));

    menu.move( pos );
    menu.exec();
}

void MainFrame::on_toolButtonMenuActionTriggered(QAction *action){
    if(action->data().isValid()){//only remove actions contains valid data (the channel index)
        int channelIndex = action->data().toInt();
        if(channelIndex >= 0 && channelIndex < localChannels.size()){
            LocalTrackGroupView* channel = localChannels.at(channelIndex);
            ui.localTracksLayout->removeWidget(channel);
            localChannels.removeAt(channelIndex);
            channel->deleteLater();
            update();
        }
    }
}
void MainFrame::on_toolButtonMenuActionHovered(QAction *action){
    if(action->data().isValid()){//only remove actions contains valid data (the channel index)
        int channelIndex = action->data().toInt();
        if(channelIndex >= 0 && channelIndex < localChannels.size()){
            Highligther::getInstance()->highlight(localChannels.at(channelIndex));
        }
    }
}

void MainFrame::on_addChannelClicked(){
    addLocalChannel();
}

//++++++++++++++++++++++++=
void MainFrame::initializeMainTabWidget(){
    //the rooms list tab bar is not closable
    ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

    connect( ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(on_tabCloseRequest(int)));
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

    QStringList vstPaths = ConfigStore::getVstPluginsPaths();
    if(vstPaths.empty()){//no vsts in database cache, try scan
        mainController->scanPlugins();
    }
    else{//use vsts stored in settings file
        mainController->initializePluginsList(vstPaths);
        onPluginScanFinished();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::addLocalChannel(){
    LocalTrackGroupView* localChannel = new LocalTrackGroupView(this);
    localChannels.append( localChannel );
    localChannel->setGroupName(Persistence::ConfigStore::getLastChannelName(0));

    //subchannels
    LocalTrackView* localTrackView = new LocalTrackView(localChannel, this->mainController);
    localChannel->addTrackView( localTrackView );
    ui.localTracksLayout->addWidget(localChannel);
    //localTrackView->initializeFxPanel(fxMenu);

    //QObject::connect(localTrackView, SIGNAL(editingPlugin(Audio::Plugin*)), this, SLOT(on_editingPlugin(Audio::Plugin*)));
    //QObject::connect(localTrackView, SIGNAL(removingPlugin(Audio::Plugin*)), this, SLOT(on_removingPlugin(Audio::Plugin*)));

    localTrackView->refreshInputSelectionName();


}

void MainFrame::initializeLocalTrackView(){
    //fxMenu = createFxMenu();
    addLocalChannel();
}

void MainFrame::initializeLoginService(){
    Login::LoginService* loginService = this->mainController->getLoginService();
    connect(loginService, SIGNAL(roomsListAvailable(QList<Login::RoomInfo>)),
            this, SLOT(on_roomsListAvailable(QList<Login::RoomInfo>)));
}

void MainFrame::initializeWindowState(){
    if(ConfigStore::windowWasMaximized()){
        setWindowState(Qt::WindowMaximized);
    }
    else{
        QPointF location = ConfigStore::getLastWindowLocation();
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
            //mainController->getAudioDriver()->stop();
            mainController->getNinjamController()->stop();//disconnect from server
            //mainController->getAudioDriver()->start();
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
     return r1.getUsers().size() > r2.getUsers().size();
}

void MainFrame::on_roomsListAvailable(QList<Login::RoomInfo> publicRooms){
    hideBusyDialog();
    ui.allRoomsContent->setLayout(new QVBoxLayout(ui.allRoomsContent));
    qSort(publicRooms.begin(), publicRooms.end(), jamRoomLessThan);
    foreach(Login::RoomInfo server, publicRooms ){
        JamRoomViewPanel* roomViewPanel = new JamRoomViewPanel(server, ui.allRoomsContent, mainController);
        roomViewPanels.insert(server.getID(), roomViewPanel);
        ui.allRoomsContent->layout()->addWidget(roomViewPanel);
        connect( roomViewPanel, SIGNAL(startingListeningTheRoom(Login::RoomInfo)),
                 this, SLOT(on_startingRoomStream(Login::RoomInfo)));
        connect( roomViewPanel, SIGNAL(finishingListeningTheRoom(Login::RoomInfo)),
                 this, SLOT(on_stoppingRoomStream(Login::RoomInfo)));
        connect( roomViewPanel, SIGNAL(enteringInTheRoom(Login::RoomInfo)),
                 this, SLOT(on_enteringInRoom(Login::RoomInfo)));
    }

}

//+++++++++++++++++++++++++++++++++++++
void MainFrame::on_startingRoomStream(Login::RoomInfo roomInfo){
    //clear all plots
    foreach (JamRoomViewPanel* viewPanel, this->roomViewPanels.values()) {
        viewPanel->clearPeaks();
    }

    if(roomInfo.hasStream()){//just in case...
        mainController->playRoomStream(roomInfo);
    }
}

void MainFrame::on_stoppingRoomStream(Login::RoomInfo roomInfo){
    mainController->stopRoomStream();
    roomViewPanels[roomInfo.getID()]->clearPeaks();
}

void MainFrame::on_enteringInRoom(Login::RoomInfo roomInfo){
    showBusyDialog("Connecting in " + roomInfo.getName() + " ...");
    mainController->enterInRoom(roomInfo);

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
    float metronomeInitialGain = ConfigStore::getMetronomeGain();
    float metronomeInitialPan = ConfigStore::getMetronomePan();
    metronomeTrackView = new MetronomeTrackView(this, mainController, NinjamJamRoomController::METRONOME_TRACK_ID, metronomeInitialGain, metronomeInitialPan );

    ui.localTracksLayout->addWidget(metronomeTrackView);

    //add the chat panel in main window
    ChatPanel* chatPanel = ninjamWindow->getChatPanel();
    ui.chatTabWidget->addTab(chatPanel, QIcon(":/images/ninja.png"), roomInfo.getName());
}

void MainFrame::on_exitedFromRoom(bool normalDisconnection){
    hideBusyDialog();
    if(metronomeTrackView){
        ui.localTracksLayout->removeWidget(metronomeTrackView);
    }

    //remove the jam room tab (the last tab)
    if(ui.tabWidget->count() > 1){
        ui.tabWidget->widget(1)->deleteLater();//delete the room window
        ui.tabWidget->removeTab(1);
    }

    if(ui.chatTabWidget->count() > 0){
        ui.chatTabWidget->widget(0)->deleteLater();
        ui.chatTabWidget->removeTab(0);
    }

    if(!normalDisconnection){
        QMessageBox::warning(this, "Warning", "Disconnected from server!", QMessageBox::NoButton, QMessageBox::NoButton);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::timerEvent(QTimerEvent *){
    //update local input track peaks

    foreach (LocalTrackGroupView* channel, localChannels) {
        channel->updatePeaks();
    }

    //update metronome peaks
    if(mainController->isPlayingInNinjamRoom()){
        if(metronomeTrackView){
            AudioPeak peaks = mainController->getTrackPeak(metronomeTrackView->getTrackID());
            metronomeTrackView->setPeaks(peaks.getLeft(), peaks.getRight());
        }

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
              roomView->addPeak(mainController->getRoomStreamPeak().max());
          }
    }
}

//++++++++++++=

void MainFrame::resizeEvent(QResizeEvent *){
    if(busyDialog.isVisible()){
        centerBusyDialog();
    }
}

void MainFrame::changeEvent(QEvent *ev){
    if(ev->type() == QEvent::WindowStateChange){
        ConfigStore::storeWindowState(isMaximized());
    }
    ev->accept();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::closeEvent(QCloseEvent *)
 {
    //qDebug() << "MainFrame::closeEvent";
    if(mainController != NULL){
        mainController->stop();
    }
    //store window position
    QRect screen = QApplication::desktop()->screenGeometry();
    float x = (float)this->pos().x()/screen.width();
    float y = (float)this->pos().y()/screen.height();
    QPointF location(x, y) ;
    ConfigStore::storeWindowLocation( location ) ;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::showEvent(QShowEvent *)
{
    if(!mainController->isStarted()){
        showBusyDialog("Loading rooms list ...");
        mainController->start();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::~MainFrame()
{
    killTimer(timerID);

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
void MainFrame::on_preferencesClicked()
{
    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    AudioDriver* audioDriver = mainController->getAudioDriver();
    audioDriver->stop();
    midiDriver->stop();
    PreferencesDialog dialog(mainController, this);
    connect(&dialog, SIGNAL(ioChanged(int,int,int,int,int,int,int,int)), this, SLOT(on_IOPropertiesChanged(int, int,int,int,int,int,int,int)));
    dialog.exec();

    //midiDriver->start();
    //audioDriver->start();

    //audio driver parameters are changed in on_IOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.
}

void MainFrame::on_IOPropertiesChanged(int midiDeviceIndex, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    //qDebug() << "midi device: " << midiDeviceIndex << endl;
    //bool midiDeviceChanged =  midiDeviceIndex

    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    midiDriver->setInputDeviceIndex(midiDeviceIndex);

#ifdef _WIN32
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    audioDriver->setProperties(audioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#else
    //preciso de um outro on_audioIoPropertiesChanged que me dê o input e o output device
    //audioDriver->setProperties(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#endif
    mainController->updateInputTracksRange();
    ConfigStore::storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, audioDevice, sampleRate, bufferSize, midiDeviceIndex);

    mainController->getMidiDriver()->start();
    mainController->getAudioDriver()->start();
}

//input selection changed by user or by system
void MainFrame::on_inputSelectionChanged(){
    //PRECISO VER COMO VOU INDEXAR O SUBCANAL
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




