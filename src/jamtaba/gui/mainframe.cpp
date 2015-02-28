#include "MainFrame.h"
#include <QCloseEvent>
#include "AudioDialog.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QList>
#include <QAction>
#include "JamRoomViewPanel.h"
#include "../persistence/ConfigStore.h"
#include "../JamtabaFactory.h"
#include "../audio/core/PortAudioDriver.h"
#include "../MainController.h"
#include "../loginserver/LoginService.h"
#include "../loginserver/JamRoom.h"
#include "../audio/core/plugins.h"
#include "plugins/guis.h"
#include "FxPanel.h"
#include "plugins/pluginwindow.h"

using namespace Audio;
using namespace Persistence;
using namespace Controller;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::MainFrame(MainController *mainController, QWidget *parent)
    : QMainWindow(parent),
    fxMenu(nullptr)
{
	ui.setupUi(this);
    this->mainController = mainController;

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
    timerID = startTimer(50);


    Login::LoginService* loginService = this->mainController->getLoginService();
    connect(loginService, SIGNAL(connectedInServer(QList<Login::AbstractJamRoom*>)),
                                 this, SLOT(on_connectedInServer(QList<Login::AbstractJamRoom*>)));

    fxMenu = createFxMenu();
    ui.localTrack->initializeFxPanel(fxMenu);

    QObject::connect(ui.localTrack, SIGNAL(editingPlugin(PluginGui*)), this, SLOT(on_editingPlugin(PluginGui*)));
    QObject::connect(ui.localTrack, SIGNAL(removingPlugin(PluginGui*)), this, SLOT(on_removingPlugin(PluginGui*)));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::on_removingPlugin(PluginGui *pluginGui){
    if(pluginGui->isVisible()){
         ((QDialog*)pluginGui->parentWidget())->close();
    }
    mainController->removePlugin(pluginGui->getPlugin());
    pluginGui->deleteLater();
}

void MainFrame::on_editingPlugin(PluginGui *pluginGui){
    showPluginGui(pluginGui);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::on_fxMenuActionTriggered(QAction* action){
    //add a new plugin
    Plugin::PluginDescriptor* pluginDescriptor = action->data().value<Plugin::PluginDescriptor*>();
    Audio::Plugin* plugin = mainController->addPlugin(pluginDescriptor);
    PluginGui* pluginGui = createPluginView(pluginDescriptor, plugin);
    ui.localTrack->addPlugin(pluginGui);
    showPluginGui(pluginGui);
}
//++++++++++++++++++++++++++++++++++++
void MainFrame::showPluginGui(PluginGui *pluginGui){
    if(!pluginGui->isVisible()){
        PluginWindow* pluginWindow = new PluginWindow(pluginGui, this);
        pluginWindow->show();
    }
    else{
        ((QDialog*)pluginGui->parentWidget())->activateWindow();
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PluginGui* MainFrame::createPluginView(Plugin::PluginDescriptor* d, Audio::Plugin* plugin){
    if(d->getGroup() == "Jamtaba"){
        if(d->getName() == "Delay"){
            return new DelayGui((Plugin::JamtabaDelay*)plugin);
        }
    }
    return nullptr;
}

//++++++++++++++++++++
QMenu* MainFrame::createFxMenu(){
    if(fxMenu ){
        fxMenu->close();
        fxMenu->deleteLater();
    }
    QMenu* menu = new QMenu(this);

    std::vector<Plugin::PluginDescriptor*> plugins = mainController->getPluginsDescriptors();
    for(Plugin::PluginDescriptor* pluginDescriptor  : plugins){
        QAction* action = menu->addAction(QString(pluginDescriptor->getName()));
        action->setData(QVariant::fromValue(pluginDescriptor));
    }

    menu->connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(on_fxMenuActionTriggered(QAction*)));
    return menu;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//esses eventos deveriam ser tratados no controller
void MainFrame::on_connectedInServer(QList<Login::AbstractJamRoom*> rooms){
    ui.allRoomsContent->setLayout(new QVBoxLayout(ui.allRoomsContent));
    foreach(Login::AbstractJamRoom* room, rooms){
        JamRoomViewPanel* roomViewPanel = new JamRoomViewPanel(room, ui.allRoomsContent);
        roomViewPanels.insert(room, roomViewPanel);
        ui.allRoomsContent->layout()->addWidget(roomViewPanel);
        connect( roomViewPanel, SIGNAL(startingListeningTheRoom(Login::AbstractJamRoom*)), this, SLOT(on_startingRoomStream(Login::AbstractJamRoom*)));
        connect( roomViewPanel, SIGNAL(finishingListeningTheRoom(Login::AbstractJamRoom*)), this, SLOT(on_stoppingRoomStream(Login::AbstractJamRoom*)));
    }
}

void MainFrame::on_startingRoomStream(Login::AbstractJamRoom* room){
    if(room->hasStreamLink()){
        mainController->playRoomStream(room);
    }
}

void MainFrame::on_stoppingRoomStream(Login::AbstractJamRoom * room){
    mainController->stopRoomStream();
    roomViewPanels[room]->clearPeaks();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::timerEvent(QTimerEvent *){

    if(mainController->isPlayingRoomStream()){
          Login::AbstractJamRoom* room = mainController->getCurrentStreamingRoom();
          JamRoomViewPanel* roomView =  roomViewPanels[room];
          MainController::Peaks peaks = mainController->getPeaks();
          roomView->addPeak(peaks.lastStreamRoomPeak);
    }
}

//++++++++++++=
void MainFrame::changeEvent(QEvent *ev){
    if(ev->type() == QEvent::WindowStateChange){
        ConfigStore::storeWindowState(isMaximized());
    }
    ev->accept();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::closeEvent(QCloseEvent *)
 {
    qDebug() << "MainFrame::closeEvent";
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
    mainController->start();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::~MainFrame()
{
    killTimer(timerID);
    //delete mainController;
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

//audio preferences
void MainFrame::on_actionAudio_triggered()
{
    PortAudioDriver* driver = (PortAudioDriver*)mainController->getAudioDriver();
    driver->stop();
    AudioIODialog dialog(driver, this);
    connect(&dialog, SIGNAL(audioIOPropertiesChanged(int,int,int,int,int,int,int)), this, SLOT(on_audioIOPropertiesChanged(int,int,int,int,int,int,int)));
    dialog.exec();
    driver->start();
    //audio driver is restarted in on_audioIOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.
}

void MainFrame::on_audioIOPropertiesChanged(int selectedDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
#ifdef _WIN32
    audioDriver->setProperties(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#else
    //preciso de um outro on_audioIoPropertiesChanged que me dê o input e o output device
    //audioDriver->setProperties(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#endif

    ConfigStore::storeAudioSettings(firstIn, lastIn, firstOut, lastOut, selectedDevice, selectedDevice, sampleRate, bufferSize);
}


