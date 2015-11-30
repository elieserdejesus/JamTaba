#include "MainWindow.h"

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
#include <QDesktopServices>

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
#include "PrivateServerDialog.h"
//#include "../performance/PerformanceMonitor.h"

#include "NinjamController.h"
#include "ninjam/Server.h"
#include "persistence/Settings.h"
#include "audio/core/AudioDriver.h"
#include "audio/vst/PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "midi/MidiDriver.h"
#include "MainController.h"
#include "loginserver/LoginService.h"
#include "Utils.h"
#include "audio/RoomStreamerNode.h"
#include "UserNameDialog.h"


#include "log/logging.h"
#include <QShortcut>


using namespace Audio;
using namespace Persistence;
using namespace Controller;
using namespace Ninjam;

//const int MainWindow::PERFORMANCE_MONITOR_REFRESH_TIME = 200;//in miliseconds

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainWindow::MainWindow(Controller::MainController *mainController, QWidget *parent)
    :
      QMainWindow(parent),
    busyDialog(0),
    mainController(mainController),
    pluginScanDialog(nullptr),
    ninjamWindow(nullptr),
    roomToJump(nullptr),
    fullViewMode(true)
    //lastPerformanceMonitorUpdate(0)
{
    qCInfo(jtGUI) << "Creating MainWindow...";
	ui.setupUi(this);



    setWindowTitle("Jomtobo v" + QApplication::applicationVersion());

    initializeLoginService();
    //initializePluginFinder(); //called in derived classes
    initializeMainTabWidget();
    initializeViewModeMenu();


//    QDir stylesDir(":/style");
//    QStringList cssList = stylesDir.entryList(QStringList("*.css"));
//    foreach (QString css, cssList) {
//        QAction* action = ui.menuThemes->addAction(css);
        //action->setData(css);
        //action->setEnabled(false);
        //action->setVisible(false);
//    }

//    QObject::connect(ui.menuThemes, SIGNAL(triggered(QAction*)), this, SLOT(on_newThemeSelected(QAction*)));

    timerID = startTimer(1000/50);

    QObject::connect(ui.menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(on_preferencesClicked(QAction*)));
    QObject::connect(ui.actionNinjam_community_forum, SIGNAL(triggered(bool)), this, SLOT(on_ninjamCommunityMenuItemTriggered()));
    QObject::connect(ui.actionNinjam_Official_Site, SIGNAL(triggered(bool)), this, SLOT(on_ninjamOfficialSiteMenuItemTriggered()));
    QObject::connect(ui.actionPrivate_Server, SIGNAL(triggered(bool)), this, SLOT(on_privateServerMenuItemTriggered()));
    QObject::connect(ui.actionReportBugs, SIGNAL(triggered(bool)), this, SLOT(on_reportBugMenuItemTriggered()));
    QObject::connect(ui.actionWiki, SIGNAL(triggered(bool)), this, SLOT(on_wikiMenuItemTriggered()));
    QObject::connect(ui.actionUsersManual, SIGNAL(triggered(bool)), this, SLOT(on_UsersManualMenuItemTriggered()));
    QObject::connect( ui.localControlsCollapseButton, SIGNAL(clicked()), this, SLOT(on_localControlsCollapseButtonClicked()));
    QObject::connect( mainController->getRoomStreamer(), SIGNAL(error(QString)), this, SLOT(on_RoomStreamerError(QString)));

    initializeLocalInputChannels();

    ui.chatArea->setVisible(false);//hide chat area until connect in a server to play

    ui.allRoomsContent->setLayout(new QGridLayout());
    ui.allRoomsContent->layout()->setContentsMargins(0,0,0,0);
    ui.allRoomsContent->layout()->setSpacing(24);

    foreach (LocalTrackGroupView* channel, controlSurfaceJTB) {
        channel->refreshInputSelectionNames();
    }

    if(mainController->isRunningAsVstPlugin()){
        this->ui.actionVstPreferences->setVisible(false);
        this->ui.actionAudioPreferences->setVisible(false);
        this->ui.actionMidiPreferences->setVisible(false);
        this->ui.actionQuit->setVisible(false);
        this->ui.actionFullscreenMode->setVisible(false);
    }

    initializeWindowState();//window size, maximization ...

    ui.localTracksWidget->installEventFilter(this);

    showBusyDialog("Loading rooms list ...");
    qCInfo(jtGUI) << "MainWindow created!";
}
//++++++++++++++++++++++++=
void MainWindow::initializePluginFinder(){
    Vst::PluginFinder* pluginFinder = mainController->getPluginFinder();
    if(pluginFinder){
        QObject::connect(pluginFinder, SIGNAL(scanStarted()), this, SLOT(onScanPluginsStarted()));
        QObject::connect(pluginFinder, SIGNAL(scanFinished(bool)), this, SLOT(onScanPluginsFinished(bool)));
        QObject::connect(pluginFinder, SIGNAL(badPluginDetected(QString)), this, SLOT(onBadPluginDetected(QString)));
        QObject::connect(pluginFinder, SIGNAL(pluginScanFinished(QString,QString,QString)), this, SLOT(onPluginFounded(QString,QString,QString)));
        QObject::connect(pluginFinder, SIGNAL(pluginScanStarted(QString)), this, SLOT(onScanPluginsStarted(QString)));
    }
}

//++++++++++++++++++++++++=
//void MainWindow::on_newThemeSelected(QAction *a){
//    QString css = a->data().toString();
//    mainController->configureStyleSheet(css);
//}

//++++++++++++++++++++++++=
void MainWindow::showPeakMetersOnlyInLocalControls(bool showPeakMetersOnly){
    foreach (LocalTrackGroupView* channel, controlSurfaceJTB) {
        //channel->togglePeakMeterOnlyMode();
        channel->setPeakMeterMode(showPeakMetersOnly);
    }
    ui.labelSectionTitle->setVisible(!showPeakMetersOnly);

    ui.localControlsCollapseButton->setChecked(showPeakMetersOnly);
    recalculateLeftPanelWidth();
}

void MainWindow::recalculateLeftPanelWidth(){
    int min = ui.localTracksWidget->sizeHint().width() + 12;
    int max = min;
    bool showingPeakMeterOnly = controlSurfaceJTB.first()->isShowingPeakMeterOnly();
    Qt::ScrollBarPolicy scrollPolicy = Qt::ScrollBarAlwaysOff;

    //limit the local inputs widget in mini mode
    if(!showingPeakMeterOnly && !fullViewMode){
        max = 180;
        if(min > max){
            min = max;
            scrollPolicy = Qt::ScrollBarAlwaysOn;
        }
    }
    ui.leftPanel->setMaximumWidth(max);
    ui.leftPanel->setMinimumWidth(min);
    ui.scrollArea->setHorizontalScrollBarPolicy(scrollPolicy);
}

void MainWindow::on_localControlsCollapseButtonClicked(){
    bool isShowingPeakMetersOnly = controlSurfaceJTB.first()->isShowingPeakMeterOnly();
    showPeakMetersOnlyInLocalControls(!isShowingPeakMetersOnly);//toggle
    recalculateLeftPanelWidth();
}
//++++++++++++++++++++++++=
Persistence::InputsSettings MainWindow::getInputsSettings() const{
    InputsSettings settings;
    foreach (LocalTrackGroupView* trackGroupView, controlSurfaceJTB) {
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
            float boost = Utils::linearToDb(inputNode->getBoost());
            float pan = inputNode->getPan();
            bool muted = inputNode->isMuted();

            QList<const Audio::Plugin*> insertedPlugins = trackView->getInsertedPlugins();
            QList<Persistence::Plugin> plugins;
            foreach (const Audio::Plugin* p, insertedPlugins) {
                QByteArray serializedData = p->getSerializedData();
                plugins.append(Persistence::Plugin(p->getPath(), p->isBypassed(), serializedData));
            }
            Persistence::Subchannel sub(firstInput, channelsCount, midiDevice, midiChannel, gain, boost, pan, muted, plugins);

            channel.subChannels.append(sub);
        }
        settings.channels.append(channel);
    }
    return settings;
}
//++++++++++++++++++++++++=
void MainWindow::stopCurrentRoomStream(){
    if(mainController->isPlayingRoomStream()){
        long long roomID = mainController->getCurrentStreamingRoomID();
        if(roomViewPanels[roomID]){
            roomViewPanels[roomID]->clearPeaks(true);
        }
    }
    mainController->stopRoomStream();
}

void MainWindow::showMessageBox(QString title, QString text, QMessageBox::Icon icon){
    QMessageBox* messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(title);
    messageBox->setText(text);
    messageBox->setIcon(icon);
    messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    messageBox->show();
    if(isRunningAsVstPlugin()){
        centerDialog(messageBox);
    }
}

void MainWindow::on_RoomStreamerError(QString msg){
    stopCurrentRoomStream();
    showMessageBox("Error!", msg, QMessageBox::Critical);
}

//++++++++++++++++++++++++=
//void MainFrame::on_toolButtonClicked(){
//    QMenu menu;
//    QAction* addChannelAction = menu.addAction(QIcon(":/images/more.png"), "Add channel");
//    QObject::connect(addChannelAction, SIGNAL(triggered()), this, SLOT(on_addChannelClicked()));
//    addChannelAction->setEnabled(controlSurfaceJTB.size() < 2);//at this moment users can't create more channels
//    if(controlSurfaceJTB.size() > 1){
//        menu.addSeparator();
//        for (int i = 2; i <= controlSurfaceJTB.size(); ++i) {
//            QString channelName = controlSurfaceJTB.at(i-1)->getGroupName();
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

void MainWindow::removeChannelsGroup(int channelIndex){
    if(controlSurfaceJTB.size() >1){//the first channel group can't be removed
        if(channelIndex >= 0 && channelIndex < controlSurfaceJTB.size())
        {
            LocalTrackGroupView* channel = controlSurfaceJTB.at(channelIndex);
            ui.localTracksLayout->removeWidget(channel);
            controlSurfaceJTB.removeAt(channelIndex);

            controlSurfaceJTB.first()->setToWide();

            channel->deleteLater();

            mainController->sendRemovedChannelMessage(channelIndex);
            update();
        }
        recalculateLeftPanelWidth();
    }
}

void MainWindow::highlightChannelGroup(int index) const{
    if(index >= 0 && index < controlSurfaceJTB.size()){
        Highligther::getInstance()->highlight(controlSurfaceJTB.at(index));
    }
}

void MainWindow::addChannelsGroup(QString name){
    int channelIndex = controlSurfaceJTB.size();
    addLocalChannel( channelIndex, name, true);
    mainController->updateInputTracksRange();
    mainController->sendNewChannelsNames(getChannelsNames());
    if(mainController->isPlayingInNinjamRoom()){
        mainController->getNinjamController()->scheduleEncoderChangeForChannel(channelIndex);//create an encoder for this channel in next interval
    }
    recalculateLeftPanelWidth();
}

//++++++++++++++++++++++++=
void MainWindow::initializeMainTabWidget(){
    //the rooms list tab bar is not closable
    QWidget* tabBar = nullptr;
    tabBar = ui.tabWidget->tabBar()->tabButton(0, QTabBar::RightSide);//try get the tabBar in right side (Windows)
    if(!tabBar){//try get the tabBar in left side (MAC OSX)
        tabBar = ui.tabWidget->tabBar()->tabButton(0, QTabBar::LeftSide);
    }
    if(tabBar){
        tabBar->resize(0, 0);
        tabBar->hide();
    }

    connect( ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(on_tabCloseRequest(int)));
    connect( ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabChanged(int)));
}


//++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::on_channelNameChanged(){
    mainController->sendNewChannelsNames(getChannelsNames());
}

LocalTrackGroupView *MainWindow::addLocalChannel(int channelGroupIndex, QString channelName, bool createFirstSubchannel){
    LocalTrackGroupView* localChannel = new LocalTrackGroupView(channelGroupIndex, this);
    QObject::connect(localChannel, SIGNAL(nameChanged()), this, SLOT(on_channelNameChanged()));
    QObject::connect(localChannel, SIGNAL(trackAdded()), this, SLOT(on_localTrackAdded()));
    QObject::connect(localChannel, SIGNAL(trackRemoved()), this, SLOT(on_localTrackRemoved()));

    controlSurfaceJTB.append( localChannel );

    localChannel->setGroupName(channelName);
    ui.localTracksLayout->addWidget(localChannel);

    if(createFirstSubchannel){
        LocalTrackView* localTrackView = new LocalTrackView(this->mainController, channelGroupIndex);
        localChannel->addTrackView( localTrackView );

        if(controlSurfaceJTB.size() > 1){
            if(!mainController->isRunningAsVstPlugin()){
                //in standalone the second channel is always initialized as noInput
                localTrackView->setToNoInput();
            }
        }
        else{
            localTrackView->refreshInputSelectionName();
        }
    }

    if(!fullViewMode && controlSurfaceJTB.count() > 1){
        foreach (LocalTrackGroupView* trackGroup, controlSurfaceJTB) {
            trackGroup->setToNarrow();
        }
    }

    return localChannel;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool MainWindow::isRunningAsVstPlugin() const{
    return mainController->isRunningAsVstPlugin();
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

void MainWindow::restorePluginsList(){
    qCInfo(jtGUI) << "Restoring plugins list...";
    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        LocalTrackGroupView* channelView = controlSurfaceJTB.at(channelIndex);
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
                        if(pluginInstance){
                            try{
                                pluginInstance->restoreFromSerializedData( plugin.data);
                            }
                            catch(...){
                                qWarning() << "Exception restoring " << pluginInstance->getName();
                            }

                            subChannelView->addPlugin(pluginInstance, plugin.bypassed);
                        }
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
    qCInfo(jtGUI) << "Restoring plugins list done!";
}

//USED BY PRESETS
void MainWindow::loadPresetToTrack()
{

//we gonna assign each group of the console surface

 int groupSize=controlSurfaceJTB.size();
 qCDebug(jtConfigurator) << "************PRESET LOADING ***********";

 qCInfo(jtConfigurator) << "Initializing ControlSurface...";
 qCInfo(jtConfigurator) << "Number of groups in controlSurface :"<<groupSize;

 QList< LocalTrackView * > 	tracks;
 Persistence::PresetsSettings preset = mainController->getSettings().getPresetSettings();
 //now the preset's group count ;
 int PRST_CH_COUNT=preset.channels.size();
 qCInfo(jtConfigurator) << "Number of groups in Preset :"<<PRST_CH_COUNT;

 //if there is more groups in the preset
 if(groupSize<PRST_CH_COUNT)
  {
     int count=PRST_CH_COUNT-groupSize;
     qCInfo(jtConfigurator) << "Creating :"<<count<<" group";

     for(int i = 0 ; i < count ; i ++ )
     {
         addLocalChannel(0," new Group", true);groupSize++;
         qCInfo(jtConfigurator) << "Group size is now :"<<groupSize<<" group";
         //groupSize++;
     }
  }

 else if(groupSize>PRST_CH_COUNT)
 {
     int count=groupSize-PRST_CH_COUNT;
     qCInfo(jtConfigurator) << "removing :"<<count<<" group";

     for(int i = 0 ; i < count ; i ++ )
     {
         removeChannelsGroup(count);
         groupSize--;
         qCInfo(jtConfigurator) << "Group size is now :"<<groupSize<<" group";


     }

 }

 //LOOP inside the controlSurfaceJTB groups
 for(int group=0;group<groupSize;group++)
  {
   //load the name of the group
   qCInfo(jtConfigurator) << "......................................";
   controlSurfaceJTB.at(group)->setGroupName(preset.channels.at(group).name);
   //get the tracks of that group
   tracks=controlSurfaceJTB.at(group)->getTracks();
   int tracksCount=tracks.size();
   qCInfo(jtConfigurator) << "Loading group :"<<group;
   qCInfo(jtConfigurator) << "Number of tracks in group :"<<tracksCount;

   //compute tracks to create ( if any ) in that group
   int TRK_TO_CREATE=0;
   int PRST_TRK_COUNT=preset.channels.at(group).subChannels.size();
    qCInfo(jtConfigurator) << "Number of tracks in preset :"<<PRST_TRK_COUNT;

    //ADD OR DELETE TRACKS
   if(tracksCount<PRST_TRK_COUNT)//must create a track
   {
     TRK_TO_CREATE=PRST_TRK_COUNT-tracksCount;
     qCInfo(jtConfigurator) << "Number of tracks to create : "<<TRK_TO_CREATE;

     for(int i = 0 ; i < TRK_TO_CREATE ; i ++ )
     {
         LocalTrackView* subChannelView = new LocalTrackView( mainController, i+1
                                                              , 0, BaseTrackView::intToBoostValue(0), 0, 0);
         controlSurfaceJTB.at(group)->addTrackView(subChannelView);
         qCInfo(jtConfigurator) << "SubTrack added in group : "<<group;

         //tracksCount++;

     }
   }
   else if(tracksCount>PRST_TRK_COUNT)//must delete a track
   {
     int TRK_TO_DEL=tracksCount-PRST_TRK_COUNT;
     qCInfo(jtConfigurator) << "Number of tracks to delete : "<<TRK_TO_DEL;

     for(int i = 0 ; i < TRK_TO_DEL ; i ++ )
     {
        controlSurfaceJTB.at(group)->removeTrackView(TRK_TO_DEL);
         qCInfo(jtConfigurator) << "SubTrack removed in group : "<<group;

         //tracksCount--;

     }
   }

   //now the preset's SUB track count ;
   //int PRST_SUB_COUNT=preset.tracks.at(index).subChannels.size();
   //qCInfo(jtConfigurator) << "Number of Sub Tracks in group :"<<PRST_SUB_COUNT;


tracks=controlSurfaceJTB.at(group)->getTracks();
tracksCount=tracks.size();
   //assign preset to indexed tracks
   for(int index=0;index<tracksCount;index++)
       {
         //gain
         qCInfo(jtConfigurator) << "<<<<<<<<<<<<<<<<<<<<";
         float gain=preset.channels.at(group).subChannels.at(index).gain;
         tracks.at(index)->getInputNode()->setGain(gain);
         qCInfo(jtConfigurator) << "Track"<<index<<" gain : "<<gain<<" for"<<index;
         //pan
         float pan=preset.channels.at(group).subChannels.at(index).pan;
         tracks.at(index)->getInputNode()->setPan(pan);
         qCInfo(jtConfigurator) << "Track "<<index<<"Pan : "<<pan<<" for"<<index;
         //boost
         int boost=preset.channels.at(group).subChannels.at(index).boost;
         BaseTrackView::BoostValue boostValue = BaseTrackView::intToBoostValue(boost);
         tracks.at(index)->initializeBoostButtons(boostValue);
         qCInfo(jtConfigurator) << "Boost "<<index<<"index : "<<boostValue<<" for"<<index;
         //mute
         bool muted=preset.channels.at(group).subChannels.at(index).muted;
         tracks.at(index)->getInputNode()->setMuteStatus(muted);
         qCInfo(jtConfigurator) << "Mute "<<index<<"state : "<<muted<<" for"<<index;

         //solo
         //bool solo=preset.channels.at(group).subChannels.at(index).;
         //tracks.at(index)->getInputNode()->setMuteStatus(muted);
         //qCInfo(jtConfigurator) << "Mute state : "<<muted<<" for"<<index;

         //NEW FUNK getFxPanel() MADE FOR PRESETS
         //first we remove plugins
          tracks.at(index)->resetFXPanel();

          //get plugins
          //create the plugins list
          QList<Persistence::Plugin> plugins=preset.channels.at(group).subChannels.at(index).plugins;
          int plugCount=plugins.size();
          for(int i=0;i<plugCount;i++)
          {
              QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(plugins.at(i).path);
              Audio::PluginDescriptor descriptor(pluginName, "VST", plugins.at(i).path);
              Audio::Plugin* pluginInstance = mainController->addPlugin(index, descriptor);
              if(pluginInstance){
                  try{
                      pluginInstance->restoreFromSerializedData(plugins.at(i).data);
                  }
                  catch(...){
                      qWarning() << "Exception restoring " << pluginInstance->getName();
                  }

                 tracks.at(index)->addPlugin(pluginInstance, plugins.at(i).bypassed);
                 qCInfo(jtConfigurator) << "Plugin Added :"<<pluginInstance->getName()<<" in track : "<<index;

              }
          }
          if(!mainController->isRunningAsVstPlugin())
          {
              Persistence::Subchannel subChannel=preset.channels.at(group).subChannels.at(index);
              //using midi
              if(subChannel.midiDevice >= 0)
              {
                  qCInfo(jtConfigurator) << "\t\tSubchannel using MIDI";
                  //check if midiDevice index is valid
                  if(subChannel.midiDevice < mainController->getMidiDriver()->getMaxInputDevices())
                  {
                      qCInfo(jtConfigurator) << "\t\tMidi device index : "<<subChannel.midiDevice;

                      mainController->setInputTrackToMIDI(tracks.at(index)->getInputIndex(), subChannel.midiDevice, subChannel.midiChannel);
                  }
                  else
                  {
                      if(mainController->getMidiDriver()->hasInputDevices())
                      {qCInfo(jtConfigurator) << "\t\tSubchannel using default Midi ";

                          //use the first midi device and all channels
                          mainController->setInputTrackToMIDI(tracks.at(index)->getInputIndex(), 0, -1);
                      }
                      else
                      {
                          qCInfo(jtConfigurator) << "\t\tSubchannel using "<<subChannel.midiDevice;

                          mainController->setInputTrackToNoInput(tracks.at(index)->getInputIndex());
                      }
                  }
              }
              else if(subChannel.channelsCount <= 0){
                  qCInfo(jtConfigurator) << "\t\tsetting Subchannel to no noinput";
                  mainController->setInputTrackToNoInput(tracks.at(index)->getInputIndex());
              }
              else if(subChannel.channelsCount == 1){
                  qCInfo(jtConfigurator) << "\t\tsetting Subchannel to mono input";
                  mainController->setInputTrackToMono(tracks.at(index)->getInputIndex(), subChannel.firstInput);
              }
              else{
                  qCInfo(jtConfigurator) << "\t\tsetting Subchannel to stereo input";
                  mainController->setInputTrackToStereo(tracks.at(index)->getInputIndex(), subChannel.firstInput);
              }
          }
        //if(tracksCount>PRST_CH_COUNT && index==PRST_CH_COUNT )break;

       }

 }
 qCInfo(jtConfigurator) << "***********************************";
}

void MainWindow::initializeLocalInputChannels(){
    qCInfo(jtGUI) << "Initializing local inputs...";
    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    int channelIndex = 0;
    foreach (Persistence::Channel channel, inputsSettings.channels) {
        qCInfo(jtGUI) << "\tCreating channel "<< channel.name;
        LocalTrackGroupView* channelView = addLocalChannel(channelIndex, channel.name, channel.subChannels.isEmpty());
        foreach (Persistence::Subchannel subChannel, channel.subChannels) {
            qCInfo(jtGUI) << "\t\tCreating sub-channel ";
            BaseTrackView::BoostValue boostValue = BaseTrackView::intToBoostValue(subChannel.boost);
            LocalTrackView* subChannelView = new LocalTrackView( mainController, channelIndex, subChannel.gain, boostValue, subChannel.pan, subChannel.muted);
            channelView->addTrackView(subChannelView);
            if(!mainController->isRunningAsVstPlugin()){
                if(subChannel.midiDevice >= 0){//using midi
                    qCInfo(jtGUI) << "\t\tSubchannel using MIDI";
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
                    qCInfo(jtGUI) << "\t\tsetting Subchannel to no noinput";
                    mainController->setInputTrackToNoInput(subChannelView->getInputIndex());
                }
                else if(subChannel.channelsCount == 1){
                    qCInfo(jtGUI) << "\t\tsetting Subchannel to mono input";
                    mainController->setInputTrackToMono(subChannelView->getInputIndex(), subChannel.firstInput);
                }
                else{
                    qCInfo(jtGUI) << "\t\tsetting Subchannel to stereo input";
                    mainController->setInputTrackToStereo(subChannelView->getInputIndex(), subChannel.firstInput);
                }
            }
            else{//VST plugin always use stereo audio input
                mainController->setInputTrackToStereo(subChannelView->getInputIndex(), 0 + (channelIndex * 2));
            }
            if(mainController->isRunningAsVstPlugin()){
                break;//avoid hacking in config file to create more subchannels in VST plugin.
            }
        }
        channelIndex++;
    }
    if(channelIndex == 0){//no channels in settings file or no settings file...
        addLocalChannel(0, "your channel", true);
    }
    qCInfo(jtGUI) << "Initializing local inputs done!";
}

void MainWindow::initializeLoginService(){
    Login::LoginService* loginService = this->mainController->getLoginService();
    connect( loginService, SIGNAL(roomsListAvailable(QList<Login::RoomInfo>)), this, SLOT(on_roomsListAvailable(QList<Login::RoomInfo>)));
    connect( loginService, SIGNAL(incompatibilityWithServerDetected()), this, SLOT(on_incompatibilityWithServerDetected()));
    connect( loginService, SIGNAL(newVersionAvailableForDownload()), this, SLOT(on_newVersionAvailableForDownload()));
    connect( loginService, SIGNAL(errorWhenConnectingToServer(QString)), this, SLOT(on_errorConnectingToServer(QString)));
}

void MainWindow::initializeWindowState(){

    setFullViewStatus(mainController->getSettings().windowsWasFullViewMode());

    if(mainController->getSettings().windowWasMaximized() && fullViewMode){
        qCDebug(jtGUI)<< "setting window state to maximized";
        setWindowState(Qt::WindowMaximized);
    }
    else{
        //FULLSCREENMODE ?
        if(mainController->getSettings().windowsWasFullViewMode())
            this->setWindowState(Qt::WindowFullScreen);
        QPointF location = mainController->getSettings().getLastWindowLocation();
        QDesktopWidget* desktop = QApplication::desktop();
        int desktopWidth = desktop->width();
        int desktopHeight = desktop->height();
        int x = desktopWidth * location.x();
        int y = desktopHeight * location.y();
        this->move(x, y);
        qCDebug(jtGUI)<< "Restoring window to position:" << x << ", " << y;
        qCDebug(jtGUI)<< "Window size:" << width() << ", " << height();
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::on_tabCloseRequest(int index){
    if(index > 0){//the first tab is not closable
        showBusyDialog("disconnecting ...");
        if(mainController->getNinjamController()->isRunning()){

            mainController->stopNinjamController();
        }
    }
}

void MainWindow::on_tabChanged(int index){
    if(index > 0){//click in room tab?
        if(mainController->isPlayingInNinjamRoom() && mainController->isPlayingRoomStream()){
            stopCurrentRoomStream();
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::showBusyDialog(){
    showBusyDialog("");
}

void MainWindow::showBusyDialog(QString message){
    busyDialog.setParent(this);
    centerBusyDialog();
    busyDialog.show(message);
}

void MainWindow::hideBusyDialog(){
    busyDialog.hide();
}


void MainWindow::centerBusyDialog(){
    int newX = ui.contentPanel->width()/2 - busyDialog.width()/2;
    int newY = ui.contentPanel->height()/2 - busyDialog.height()/2;
    if(newX > 0){
        busyDialog.move(newX + ui.contentPanel->x(), newY + ui.contentPanel->y());
    }
    else{//first time ("loading rooms list message") the contentPanel.width is zero
        busyDialog.move(this->width()/2 - busyDialog.width()/2, height()/2 - busyDialog.height()/2);
    }
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

bool MainWindow::jamRoomLessThan(Login::RoomInfo r1, Login::RoomInfo r2){
     return r1.getNonBotUsersCount() > r2.getNonBotUsersCount();
}

void MainWindow::on_incompatibilityWithServerDetected(){
    hideBusyDialog();
    QString text = "Your Jomtobo version is compatible with previous versions!";
    QMessageBox::warning(this, "Compatibility Hack (mwah ah ah!)", text );
    //close();
}

void MainWindow::detachMainController(){
    mainController = nullptr;
}

void MainWindow::on_errorConnectingToServer(QString errorMsg){
    hideBusyDialog();
    QMessageBox::warning(this, "Error!", "Error connecting in Jamtaba server!\n" + errorMsg);
    close();

}

void MainWindow::on_newVersionAvailableForDownload(){
    hideBusyDialog();
    QString text = "A new Jamtaba version is available for download! Please use the <a href='http://www.jamtaba.com'>new version</a>!";
    QMessageBox::information(this, "New Jamtaba version available!", text );
}

void MainWindow::on_roomsListAvailable(QList<Login::RoomInfo> publicRooms){
    hideBusyDialog();

    refreshPublicRoomsList(publicRooms);

    if(mainController->isPlayingInNinjamRoom()){
        this->ninjamWindow->updateGeoLocations();
        //update country flag and country names. This is necessary because the call to webservice used to get country codes and
        //country names is not synchronous. So, if country code and name are not cached we receive these data in some seconds.
    }
}


void MainWindow::refreshPublicRoomsList(QList<Login::RoomInfo> publicRooms){
    qSort(publicRooms.begin(), publicRooms.end(), jamRoomLessThan);
    int index = 0;
    foreach(Login::RoomInfo roomInfo, publicRooms ){
        if(roomInfo.getType() == Login::RoomTYPE::NINJAM){//skipping other rooms at moment
            int rowIndex = fullViewMode ? (index / 2) : (index);
            int collumnIndex = fullViewMode ? (index % 2) : 0;//use one collumn if user choosing mini view mode
            JamRoomViewPanel* roomViewPanel = roomViewPanels[roomInfo.getID()];
            if(roomViewPanel){
                roomViewPanel->refresh(roomInfo);
                ui.allRoomsContent->layout()->removeWidget(roomViewPanel); //the widget is removed but added again
            }
            else{
                roomViewPanel = new JamRoomViewPanel(roomInfo, ui.allRoomsContent, mainController);
                roomViewPanels.insert(roomInfo.getID(), roomViewPanel);
                connect( roomViewPanel, SIGNAL(startingListeningTheRoom(Login::RoomInfo)), this, SLOT(on_startingRoomStream(Login::RoomInfo)));
                connect( roomViewPanel, SIGNAL(finishingListeningTheRoom(Login::RoomInfo)), this, SLOT(on_stoppingRoomStream(Login::RoomInfo)));
                connect( roomViewPanel, SIGNAL(enteringInTheRoom(Login::RoomInfo)), this, SLOT(on_enteringInRoom(Login::RoomInfo)));
            }
            ((QGridLayout*)ui.allRoomsContent->layout())->addWidget(roomViewPanel, rowIndex, collumnIndex);
            index++;
        }
    }
}

//+++++++++++++++++++++++++++++++++++++
void MainWindow::on_startingRoomStream(Login::RoomInfo roomInfo){
    //clear all plots
    foreach (JamRoomViewPanel* viewPanel, this->roomViewPanels.values()) {
        viewPanel->clearPeaks(roomInfo.getID() != viewPanel->getRoomInfo().getID());
    }

    if(roomInfo.hasStream()){//just in case...
        mainController->playRoomStream(roomInfo);
    }
}

void MainWindow::on_stoppingRoomStream(Login::RoomInfo roomInfo){
    Q_UNUSED(roomInfo)
    stopCurrentRoomStream();
}

QStringList MainWindow::getChannelsNames() const{
    QStringList channelsNames;
    foreach (LocalTrackGroupView* channel, controlSurfaceJTB) {
        channelsNames.append(channel->getGroupName());
    }
    return channelsNames;
}

//user trying enter in a room
void MainWindow::on_enteringInRoom(Login::RoomInfo roomInfo, QString password){

    //stop room stream before enter in a room
    if(mainController->isPlayingRoomStream()){
        long long roomID = mainController->getCurrentStreamingRoomID();
        if(roomViewPanels[roomID]){
            roomViewPanels[roomID]->clearPeaks(true);
        }
        mainController->stopRoomStream();
    }

    if(!mainController->userNameWasChoosed()){
//        bool ok;
//        QString lastUserName = mainController->getUserName();
//        QString newUserName = QInputDialog::getText(this, "", "Enter your user name:", QLineEdit::Normal, lastUserName , &ok, Qt::FramelessWindowHint);
//        if (ok && !newUserName.isEmpty()){
//           mainController->setUserName(newUserName);
//           setWindowTitle("Jamtaba (" + mainController->getUserName() + ")");
//        }
        QString lastUserName = mainController->getUserName();
        UserNameDialog dialog(this, lastUserName);
        if(isRunningAsVstPlugin()){
            centerDialog(&dialog);
        }
        if(dialog.exec() == QDialog::Accepted){
            QString userName = dialog.getUserName().trimmed();
            if(!userName.isEmpty()){
                mainController->setUserName(userName);
                setWindowTitle("Jamtaba v" + QApplication::applicationVersion() + " (" + userName + ")");
            }
            else{
                QMessageBox::warning(this, "Warning!", "Empty name is not allowed!");
            }
        }
        else{
            qWarning() << "name dialog canceled";
        }
    }

    if(mainController->isPlayingInNinjamRoom()){
        //store the room to jump and wait for disconnectedFromServer event to connect in this new room
        roomToJump.reset( new Login::RoomInfo(roomInfo));
        passwordToJump = password;

        mainController->stopNinjamController();//disconnect from current ninjam server
    }
    else if(mainController->userNameWasChoosed()){
        showBusyDialog("Connecting in " + roomInfo.getName() + " ...");
        mainController->enterInRoom(roomInfo, getChannelsNames(), password);
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//estes eventos são disparados pelo controlador depois que já aconteceu a conexão com uma das salas

void MainWindow::enterInRoom(Login::RoomInfo roomInfo){
    qCDebug(jtGUI) << "hidding busy dialog...";
    hideBusyDialog();

    qCDebug(jtGUI) << "creating NinjamRoomWindow...";
    ninjamWindow.reset( createNinjamWindow(roomInfo, mainController));
    QString tabName = roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")";
    ninjamWindow->setFullViewStatus(this->fullViewMode);
    int index = ui.tabWidget->addTab(ninjamWindow.data(), tabName);
    ui.tabWidget->setCurrentIndex(index);


    //add the chat panel in main window
    qCDebug(jtGUI) << "adding ninjam chat panel...";
    ChatPanel* chatPanel = ninjamWindow->getChatPanel();
    ui.chatTabWidget->addTab(chatPanel, QIcon(":/images/ninja.png"), "chat " + roomInfo.getName());

    //show chat area
    ui.chatArea->setVisible(true);

    ui.leftPanel->adjustSize();
    //ui.leftPanel->setMinimumWidth(500);
    qCDebug(jtGUI) << "MainWindow::enterInRoom() done!";

    QObject::connect(mainController->getNinjamController(), SIGNAL(preparedToTransmit()), this, SLOT(ninjamTransmissionStarted()));
    QObject::connect(mainController->getNinjamController(), SIGNAL(preparingTransmission()), this, SLOT(ninjamPreparingToTransmit()));
}

//+++++++++++++++ PREPARING TO XMIT +++++++++++
//this signal is received when ninjam controller is ready to transmit (after wait for 1 or 2 complete intervals).
void MainWindow::ninjamTransmissionStarted(){
    setInputTracksPreparingStatus(false);//tracks are prepared to transmit
}

void MainWindow::ninjamPreparingToTransmit(){
    //tracks are waiting to start transmiting
    setInputTracksPreparingStatus(true);
}

//+++++++++++++++++++++++++++++
void MainWindow::exitFromRoom(bool normalDisconnection){
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

    ninjamWindow.reset();

    //hide chat area
    ui.chatArea->setVisible(false);

    setInputTracksPreparingStatus(false);//reset the prepating status when user leave the room. This is specially necessary if user enter in a room and leave before the track is prepared to transmit.

    if(!normalDisconnection){
        //QMessageBox::warning(this, "Warning", "Disconnected from server!", QMessageBox::NoButton, QMessageBox::NoButton);
        showMessageBox("Error", "Disconnected from ninjam server", QMessageBox::Warning);
    }
    else{
        if(roomToJump){//waiting the disconnection to connect in a new room?
            showBusyDialog("Connecting in " + roomToJump->getName());
            mainController->enterInRoom(*roomToJump, getChannelsNames(), (passwordToJump.isNull() || passwordToJump.isEmpty()) ? "" : passwordToJump);
            roomToJump.reset();
            passwordToJump = "";
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::setInputTracksPreparingStatus(bool preparing){
    foreach (LocalTrackGroupView* trackGroup, controlSurfaceJTB) {
        trackGroup->setPreparingStatus(preparing);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainWindow::timerEvent(QTimerEvent *){

    if(!mainController){
        return;
    }

//    static long lastBeatUpdate = QDateTime::currentMSecsSinceEpoch();
//    static int beat = 0;
//    long ellapsed = QDateTime::currentMSecsSinceEpoch() - lastBeatUpdate;
//    if(ellapsed >= 500){
//        lastBeatUpdate = QDateTime::currentMSecsSinceEpoch();
//        beat = (beat + 1) % ui.circularProgress->getBeatsPerInterval();
//        ui.circularProgress->setCurrentBeat(beat);
//    }

    //update local input track peaks
    foreach (TrackGroupView* channel, controlSurfaceJTB) {
        channel->updatePeaks();
    }

    //update metronome peaks
    if(mainController->isPlayingInNinjamRoom()){

        //update tracks peaks
        if(ninjamWindow){
            ninjamWindow->updatePeaks();
        }
    }

    //update cpu and memmory usage
//    qint64 now = QDateTime::currentMSecsSinceEpoch();
//    if(now - lastPerformanceMonitorUpdate >= PERFORMANCE_MONITOR_REFRESH_TIME){
//        ui.tabWidget->setResourcesUsage(performanceMonitor.getCpuUsage(), performanceMonitor.getMemmoryUsage());
//        lastPerformanceMonitorUpdate = now;
//    }

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
void MainWindow::resizeEvent(QResizeEvent *){
    if(busyDialog.isVisible()){
        centerBusyDialog();
    }
}

void MainWindow::changeEvent(QEvent *ev){
    if(ev->type() == QEvent::WindowStateChange && mainController){
//        if(isMaximized() && !fullViewMode){
//            setFullViewStatus(true);
//        }
        mainController->storeWindowSettings(isMaximized(), fullViewMode, computeLocation() );
    }
    ev->accept();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QPointF MainWindow::computeLocation() const{
    QRect screen = QApplication::desktop()->screenGeometry();
    float x = (float)this->pos().x()/screen.width();
    float y = (float)this->pos().y()/screen.height();
    return QPointF(x, y);
}

void MainWindow::closeEvent(QCloseEvent *){
    if(mainController){

        mainController->storeWindowSettings(isMaximized(), fullViewMode, computeLocation() );
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainWindow::~MainWindow()
{
    qCDebug(jtGUI) << "MainWindow destructor...";
    setParent(nullptr);
    if(mainController){
        mainController->saveLastUserSettings(getInputsSettings());

    }


    foreach (LocalTrackGroupView* groupView, this->controlSurfaceJTB ) {

        groupView->detachMainControllerInSubchannels();
    }
    mainController = nullptr;

    killTimer(timerID);
    qCDebug(jtGUI) << "Main frame timer killed!";
    qCDebug(jtGUI) << "MainWindow destructor finished.";
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

void MainWindow::on_privateServerConnectionAccepted(QString server, int serverPort, QString password){

    mainController->storePrivateServerSettings(server, serverPort, password);

    Login::RoomInfo roomInfo(server, serverPort, Login::RoomTYPE::NINJAM,32, 32);
    on_enteringInRoom(roomInfo, password);
}

void MainWindow::on_reportBugMenuItemTriggered(){
    QDesktopServices::openUrl(QUrl("https://github.com/elieserdejesus/JamTaba/issues"));
}

void MainWindow::on_wikiMenuItemTriggered(){
    QDesktopServices::openUrl(QUrl("https://github.com/elieserdejesus/JamTaba/wiki"));
}

void MainWindow::on_UsersManualMenuItemTriggered(){
    QDesktopServices::openUrl(QUrl("https://github.com/elieserdejesus/JamTaba/wiki/Jamtaba's-user-guide"));
}


void MainWindow::on_privateServerMenuItemTriggered(){
    Settings settings = mainController->getSettings();
    QString server = settings.getLastPrivateServer();
    QString password = settings.getLastPrivateServerPassword();
    int port = settings.getLastPrivateServerPort();
    PrivateServerDialog* privateServerDialog = new PrivateServerDialog(this, server, port, password);
    QObject::connect(privateServerDialog, SIGNAL(connectionAccepted(QString, int, QString)), this, SLOT(on_privateServerConnectionAccepted(QString, int, QString)));
    privateServerDialog->show();

    if(isRunningAsVstPlugin()){//dialogs need a workaround to appear in center of plugin screen
        centerDialog(privateServerDialog);
    }

}

void MainWindow::centerDialog(QWidget *dialog){
    QPoint globalPosition = mapToGlobal( this->pos());
    int x = globalPosition.x() + width()/2 - dialog->width()/2;
    int y = globalPosition.y() + height()/2 - dialog->height()/2;
    dialog->move(x,y);
}

void MainWindow::on_ninjamCommunityMenuItemTriggered(){
    QDesktopServices::openUrl(QUrl("http://www.ninbot.com"));
}

void MainWindow::on_ninjamOfficialSiteMenuItemTriggered(){
    QDesktopServices::openUrl(QUrl("http://www.cockos.com/ninjam/"));

}

// preferences menu
void MainWindow::on_preferencesClicked(QAction* action)
{
    //Use of action now !
    if(!mainController->isRunningAsVstPlugin()){
        if(action->objectName().contains("actionQuit")){
            this->close();return;
        }
    }
    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    AudioDriver* audioDriver = mainController->getAudioDriver();
    if(audioDriver){
        audioDriver->stop();
    }
    if(midiDriver){
        midiDriver->stop();
    }

    stopCurrentRoomStream();

    PreferencesDialog dialog(mainController, this);
    if(!mainController->isRunningAsVstPlugin()){
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
    }
    else{
        dialog.selectRecordingTab();
    }
    connect(&dialog, SIGNAL(ioPreferencesChanged(QList<bool>,int,int,int,int,int,int,int)), this, SLOT(on_IOPreferencesChanged(QList<bool>,int,int,int,int,int,int,int)));
    int result = dialog.exec();
    if(result == QDialog::Rejected){
        if(midiDriver){
            midiDriver->start();//restart audio and midi drivers if user cancel the preferences menu
        }
        if(audioDriver){
            audioDriver->start();
        }
    }

    //audio driver parameters are changed in on_IOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.
}

void MainWindow::on_IOPreferencesChanged(QList<bool> midiInputsStatus, int audioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize){
    //qCDebug(jtGUI) << "midi device: " << midiDeviceIndex << endl;
    //bool midiDeviceChanged =  midiDeviceIndex


    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();

#ifdef Q_OS_WIN
    audioDriver->setProperties(audioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#endif
#ifdef Q_OS_MACX
    audioDriver->setProperties(sampleRate, bufferSize);
#endif
    mainController->storeIOSettings(firstIn, lastIn, firstOut, lastOut, audioDevice, sampleRate, bufferSize, midiInputsStatus);

    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    midiDriver->setInputDevicesStatus(midiInputsStatus);

    mainController->updateInputTracksRange();

    foreach (LocalTrackGroupView* channel, controlSurfaceJTB) {
        channel->refreshInputSelectionNames();
    }

    mainController->getMidiDriver()->start();
    try{
        mainController->getAudioDriver()->start();
    }
    catch(const std::runtime_error& error){
        qCritical() << "Error starting audio device: " << QString::fromUtf8(error.what());
        QMessageBox::warning(this, "Audio error!", "The audio device can't be started! Please check your audio device and try restart Jamtaba!");
        mainController->useNullAudioDriver();
    }
}

//input selection changed by user or by system
void MainWindow::refreshTrackInputSelection(int inputTrackIndex){
    foreach (LocalTrackGroupView* channel, controlSurfaceJTB) {
        channel->refreshInputSelectionName(inputTrackIndex);
    }
    //localTrackGroupView->refreshInputSelectionName();
}

//plugin finder events
void MainWindow::onScanPluginsStarted(){
    if(!pluginScanDialog){
        pluginScanDialog.reset( new PluginScanDialog(this));
        QObject::connect(pluginScanDialog.data(), SIGNAL(rejected()), this, SLOT(on_pluginFinderDialogCanceled()));
    }
    pluginScanDialog->show();
}

void MainWindow::on_pluginFinderDialogCanceled(){
    mainController->cancelPluginFinder();
    pluginScanDialog.reset();//reset to null pointer
}

void MainWindow::onScanPluginsFinished(bool finishedWithoutError){
    Q_UNUSED(finishedWithoutError);
    if(pluginScanDialog){
        pluginScanDialog->close();
    }
    pluginScanDialog.reset();

}

void MainWindow::onBadPluginDetected(QString pluginPath){
    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(pluginPath);
    QWidget* parent = !pluginScanDialog.isNull() ? (QWidget*)pluginScanDialog.data() : (QWidget*)this;
    QMessageBox::warning(parent, "Plugin Error!", pluginName + " can't be loaded and will be black listed!");
    mainController->addBlackVstToSettings(pluginPath);
}

void MainWindow::onPluginFounded(QString name, QString group, QString path){
    Q_UNUSED(path);
    Q_UNUSED(group);
    if(pluginScanDialog){
        //pluginScanDialog->setCurrentScaning(path);
        pluginScanDialog->addFoundedPlugin(name);
    }
}

void MainWindow::onScanPluginsStarted(QString pluginPath){
    if(pluginScanDialog){
        pluginScanDialog->setCurrentScaning(pluginPath);
    }
}

void MainWindow::initializeViewModeMenu(){

    QObject::connect(ui.menuViewMode, SIGNAL(triggered(QAction*)), this, SLOT(on_menuViewModeTriggered(QAction*)));

    QActionGroup* group = new QActionGroup(this);
    ui.actionFullView->setActionGroup(group);
    ui.actionMiniView->setActionGroup(group);

}


void MainWindow::on_menuViewModeTriggered(QAction *)
{
    setFullViewStatus(ui.actionFullView->isChecked());
}


void MainWindow::setFullViewStatus(bool fullViewActivated){
    this->fullViewMode = fullViewActivated;
    if(!fullViewActivated){//mini view
        setMinimumSize(QSize(800, 600));
        setMaximumSize(minimumSize());
    }
    else{//full view
        setMinimumSize(QSize(1180, 790));
        setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
    }

    int tabLayoutMargim = fullViewMode ? 9 : 0;
    ui.tabLayout->setContentsMargins(tabLayoutMargim, tabLayoutMargim, tabLayoutMargim, tabLayoutMargim);

    //int leftPanelMargim = fullViewMode ? 6 : 3;
    //ui.leftPanel->setContentsMargins(leftPanelMargim, 3, leftPanelMargim, leftPanelMargim);

    showPeakMetersOnlyInLocalControls(!fullViewMode || controlSurfaceJTB.first()->isShowingPeakMeterOnly());

    ui.chatArea->setMinimumWidth(fullViewMode ? 280 : 180);

    //refresh the public rooms list
    if(!mainController->isPlayingInNinjamRoom()){
        QList<Login::RoomInfo> roomInfos;
        foreach (JamRoomViewPanel* roomView, roomViewPanels) {
            roomInfos.append(roomView->getRoomInfo());
        }
        refreshPublicRoomsList(roomInfos);
    }
    else{
        if(ninjamWindow){
            ninjamWindow->setFullViewStatus(fullViewMode);
        }
    }

    //local tracks are narrowed in mini mode
    if(!fullViewMode && controlSurfaceJTB.count() > 1){
        foreach (LocalTrackGroupView* localTrackGroup, controlSurfaceJTB) {
            localTrackGroup->setToNarrow();
        }
    }

    ui.centralWidget->layout()->setSpacing(fullViewMode ? 12 : 3);

    ui.actionFullView->setChecked(fullViewMode);
    ui.actionMiniView->setChecked(!fullViewMode);

    //recalculateLeftPanelWidth();
}
//+++++++++++++++++++++++++++
void MainWindow::on_localTrackAdded(){
    recalculateLeftPanelWidth();
}

void MainWindow::on_localTrackRemoved(){
    recalculateLeftPanelWidth();
}


//++++++++++
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui.localTracksWidget) {
        if (event->type() == QEvent::Resize) {
            recalculateLeftPanelWidth();
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::on_actionFullscreenMode_triggered()
{
      if(!ui.actionFullscreenMode->isChecked())
      {
          this->setWindowState(Qt::WindowMaximized);
          mainController->setFullScreenView(false);

      }
      else
      {this->setWindowState(Qt::WindowFullScreen);
       mainController->setFullScreenView(true);}
}


//PRESETS STUFF
void MainWindow::resetGroupChannel(LocalTrackGroupView *group)
{
    if(getChannelGroupsCount()<1)
        return;
    qCInfo(jtConfigurator) << "!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    qCInfo(jtConfigurator) << "Reseting local inputs...";
    Persistence::InputsSettings inputsSettings = mainController->getSettings().getInputsSettings();
    QList<LocalTrackView*> trackViews = group->getTracks();
    //all tracks in a group
    for(int track=0;track<group->getTracksCount();track++)
    {
        //mute audio and gui
        mainController->setTrackMute(track,false);
        trackViews.at(track)->mute(false);

        //solo audio and gui
        mainController->setTrackSolo(track,false);
        trackViews.at(track)->solo(false);

        //reset audio and midi to none
        trackViews.at(track)->setToNoInput();

        qCInfo(jtConfigurator) << "\tInput reset on channel "<< trackViews.at(track)->getTrackID();

        //NEW FUNK getFxPanel() MADE FOR PRESETS
        //now we remove plugins
         FxPanel *panel=trackViews.at(track)->getFxPanel();
         if(panel)
         {
           int fxCount=panel->getItems().size();
           if(fxCount>0)
           {
               for(int i=0;i<fxCount;i++)
               {
                   panel->removePlugin();
               }
           }
         }
         //volume now
         trackViews.at(track)->getInputNode()->setGain(1.0f);
         //pan now
         trackViews.at(track)->getInputNode()->setPan(0.0f);
         //boost
         trackViews.at(track)->getInputNode()->setBoost(1.0f);


     }

        qCInfo(jtConfigurator) << "Reseting local inputs done!";
}

//++++++++++++++++++++++++
void MainWindow::setTransmitingStatus(int channelID, bool xmitStatus){
    mainController->setTransmitingStatus(channelID, xmitStatus);
}

bool MainWindow::isTransmiting(int channelID) const{
    return mainController->isTransmiting(channelID);
}

//++++++++++++

