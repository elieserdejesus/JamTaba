#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"

#include <QComboBox>
#include <QDebug>
#include <QDateTime>

#include "ninjam/User.h"
#include "ninjam/Server.h"

#include "audio/MetronomeTrackNode.h"
#include "audio/core/AudioDriver.h"

#include "NinjamController.h"
#include "MainController.h"
#include "ninjam/Service.h"

#include "chords/ChordsPanel.h"
#include "chords/ChordProgression.h"

#include <cassert>

#include "NinjamPanel.h"
#include "ChatPanel.h"

#include "MainWindow.h"

#include <QMessageBox>

#include "log/logging.h"

//+++++++++++++++++++++++++
NinjamRoomWindow::NinjamRoomWindow(MainWindow *parent, Login::RoomInfo roomInfo, Controller::MainController *mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    mainController(mainController),
    chatPanel(new ChatPanel(this, mainController->getBotNames() )),
    fullViewMode(true),
    chordsPanel(nullptr),
    ninjamPanel(nullptr)

{
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow construtor..";
    ui->setupUi(this);

    ui->licenceButton->setIcon(QIcon(QPixmap(":/images/licence.png")));

    ui->labelRoomName->setText(roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")");

    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft);//tracks are left aligned

    qCDebug(jtNinjamGUI) << "connecting signals in ninjamController...";
    Controller::NinjamController* ninjamController = mainController->getNinjamController();
    QObject::connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(on_bpiChanged(int)));
    QObject::connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(on_bpmChanged(int)));
    QObject::connect(ninjamController, SIGNAL(intervalBeatChanged(int)), this, SLOT(on_intervalBeatChanged(int)));
    QObject::connect(ninjamController, SIGNAL(channelAdded(Ninjam::User,   Ninjam::UserChannel, long)), this, SLOT(on_channelAdded(  Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(on_channelRemoved(Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(on_channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(chatMsgReceived(Ninjam::User,QString)), this, SLOT(on_chatMessageReceived(Ninjam::User,QString)));
    QObject::connect(ninjamController, SIGNAL(channelXmitChanged(long,bool)), this, SLOT(on_channelXmitChanged(long,bool)));
    QObject::connect(ninjamController, SIGNAL(userLeave(QString)), this, SLOT(on_userLeave(QString)));
    QObject::connect(ninjamController, SIGNAL(userEnter(QString)), this, SLOT(on_userEnter(QString)));

    this->ninjamPanel = createNinjamPanel();

    QString serverLicence = mainController->getNinjamService()->getCurrentServerLicence();
    ui->licenceButton->setVisible(!serverLicence.isEmpty());

    QObject::connect(chatPanel, SIGNAL(userSendingNewMessage(QString)), this, SLOT(userSendingNewChatMessage(QString)));
    QObject::connect(chatPanel, SIGNAL(userConfirmingVoteToBpiChange(int)), this, SLOT(on_userConfirmingVoteToChangeBpi(int)));
    QObject::connect(chatPanel, SIGNAL(userConfirmingVoteToBpmChange(int)), this, SLOT(on_userConfirmingVoteToChangeBpm(int)));
    QObject::connect(chatPanel, SIGNAL(userConfirmingChordProgression(ChordProgression)), this, SLOT(on_userConfirmingChordProgression(ChordProgression)));

    chatPanel->setPreferredTranslationLanguage(mainController->getSettings().getTranslation());

}

NinjamPanel* NinjamRoomWindow::createNinjamPanel(){
    NinjamPanel* panel = new NinjamPanel();
    float initialMetronomeGain = mainController->getSettings().getMetronomeGain();
    float initialMetronomePan = mainController->getSettings().getMetronomePan();
    bool initialMetronomeMuteStatus = mainController->getSettings().getMetronomeMuteStatus();

    panel->setGainSliderValue(100 * initialMetronomeGain);
    panel->setPanSliderValue(4 * initialMetronomePan);
    panel->setMuteButtonStatus(initialMetronomeMuteStatus);
    panel->setIntervalShape(mainController->getSettings().getIntervalProgressShape());

    QObject::connect(panel, SIGNAL(bpiComboActivated(QString)), this, SLOT(ninjamBpiComboChanged(QString)));
    QObject::connect(panel, SIGNAL(bpmComboActivated(QString)), this, SLOT(ninjamBpmComboChanged(QString)));
    QObject::connect(panel, SIGNAL(accentsComboChanged(int)), this, SLOT(ninjamAccentsComboChanged(int)));

    QObject::connect( panel, SIGNAL(gainSliderChanged(int)), this, SLOT(on_MetronomeFaderMoved(int)));
    QObject::connect( panel, SIGNAL(panSliderChanged(int)), this, SLOT(on_MetronomePanSliderMoved(int)));
    QObject::connect( panel, SIGNAL(muteButtonClicked()), this, SLOT(on_MetronomeMuteClicked()));
    QObject::connect( panel, SIGNAL(soloButtonClicked()), this, SLOT(on_MetronomeSoloClicked()));


    return panel;
}

//+++++++++=
void NinjamRoomWindow::setFullViewStatus(bool fullView){
    if(fullView == this->fullViewMode){
        return;
    }
    this->fullViewMode = fullView;
    int contentMargin = fullView ? 9 : 3;
    ui->contentLayout->setContentsMargins(contentMargin, 6, contentMargin, 0);
    ui->contentLayout->setSpacing(fullView ? 24 : 6);

    //main layout
    int topMargim = fullView ? 9 : 0;
    int bottomMargim = fullView ? 9 : 3;
    layout()->setContentsMargins(0, topMargim, 0, bottomMargim);

    ui->tracksPanel->layout()->setSpacing(fullView ? 6 : 3);

    foreach (NinjamTrackGroupView* trackGroup, trackGroups.values()) {
        trackGroup->setNarrowStatus(!fullView);
    }

    update();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::showChordsPanel(ChordProgression progression){
    if(!chordsPanel){
        chordsPanel = new ChordsPanel(this);
        QObject::connect(chordsPanel, SIGNAL(buttonSendChordsToChatClicked()), this, SLOT(on_buttonSendChordsToChatClicked()));
    }
    else{
        chordsPanel->setVisible(true);
    }
    chordsPanel->setChords(progression);
    ui->contentLayout->insertWidget(1, chordsPanel);
}

void NinjamRoomWindow::on_buttonSendChordsToChatClicked(){
    if(chordsPanel){//just in case
        ChordProgression chordProgression = chordsPanel->getChordProgression();
        mainController->getNinjamController()->sendChatMessage(chordProgression.toString());
    }
}

void NinjamRoomWindow::hideChordsPanel(){
    if(chordsPanel){
        chordsPanel->setVisible(false);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updateGeoLocations(){
    foreach (NinjamTrackGroupView* trackGroup, trackGroups) {
        trackGroup->updateGeoLocation();
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void NinjamRoomWindow::on_MetronomePanSliderMoved(int value){
    float sliderValue = value/(float)ninjamPanel->getPanSliderMaximumValue();
    mainController->setTrackPan(Controller::NinjamController::METRONOME_TRACK_ID, sliderValue);
}

void NinjamRoomWindow::on_MetronomeFaderMoved(int value){
    mainController->setTrackGain(Controller::NinjamController::METRONOME_TRACK_ID, value/100.0);
}

void NinjamRoomWindow::on_MetronomeMuteClicked(){
    mainController->setTrackMute(Controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsMuted(Controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::on_MetronomeSoloClicked(){
    mainController->setTrackSolo(Controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsSoloed(Controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::userSendingNewChatMessage(QString msg){
    mainController->getNinjamController()->sendChatMessage(msg);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::on_userLeave(QString userName){
    if(chatPanel){
        chatPanel->addMessage("Jamtaba", userName + " leave the room.");
    }
}

void NinjamRoomWindow::on_userEnter(QString userName){
    if(chatPanel){
        chatPanel->addMessage("Jamtaba", userName + " enter in room.");
    }
}

void NinjamRoomWindow::on_chatMessageReceived(Ninjam::User user, QString message){

    bool isVoteMessage = !message.isNull() && message.toLower().startsWith("[voting system] leading candidate:");
    bool isChordProgressionMessage = false;
    try{
        ChatChordsProgressionParser chordsParser;
        isChordProgressionMessage = chordsParser.containsProgression(message);
    }
    catch(...){
        isChordProgressionMessage = false;//just in case
    }

    bool showTranslationButton = !isChordProgressionMessage;
    chatPanel->addMessage(user.getName(), message, showTranslationButton);

    if(isVoteMessage){
        handleVoteMessage(user, message);
    }
    else if(isChordProgressionMessage){
        handleChordProgressionMessage(user, message);
    }
}

void NinjamRoomWindow::handleChordProgressionMessage(Ninjam::User user, QString message){
    Q_UNUSED(user)
    ChatChordsProgressionParser parser;
    try{
        ChordProgression chordProgression = parser.parse(message);
        chatPanel->addChordProgressionConfirmationMessage(chordProgression);
    }
    catch(const std::runtime_error& e){
        qCCritical(jtNinjamGUI) << e.what();
    }
}

void NinjamRoomWindow::handleVoteMessage(Ninjam::User user, QString message){
    //local user is voting?
    static long long lastVoteCommand = 0;
    QString localUserFullName = mainController->getNinjamService()->getConnectedUserName();
    if (user.getFullName() == localUserFullName && message.toLower().contains("!vote")) {
        lastVoteCommand = QDateTime::currentMSecsSinceEpoch();
    }
    long timeSinceLastVote = QDateTime::currentMSecsSinceEpoch() - lastVoteCommand;
    if (timeSinceLastVote >= 1000) {
        QString commandType = (message.toLower().contains("bpm")) ? "BPM" : "BPI";

        //[voting system] leading candidate: 1/2 votes for 12 BPI [each vote expires in 60s]
        int forIndex = message.indexOf("for");
        assert(forIndex >= 0);
        int spaceAfterValueIndex = message.indexOf(" ", forIndex + 4);
        QString voteValueString = message.mid(forIndex + 4, spaceAfterValueIndex - (forIndex + 4));
        int voteValue = voteValueString.toInt();

        if(commandType == "BPI"){
            chatPanel->addBpiVoteConfirmationMessage(voteValue);
        }
        else if(commandType == "BPM"){//just in case
            chatPanel->addBpmVoteConfirmationMessage(voteValue);
        }
    }
}

void NinjamRoomWindow::on_userConfirmingVoteToChangeBpi(int newBpi){
    if(mainController->isPlayingInNinjamRoom()){
        Controller::NinjamController* controller = mainController->getNinjamController();
        if(controller){
             controller->voteBpi(newBpi);
        }
    }
}

void NinjamRoomWindow::on_userConfirmingVoteToChangeBpm(int newBpm){
    if(mainController->isPlayingInNinjamRoom()){
        Controller::NinjamController* controller = mainController->getNinjamController();
        if(controller){
             controller->voteBpm(newBpm);
        }
    }
}

void NinjamRoomWindow::on_userConfirmingChordProgression(ChordProgression chordProgression){
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    if(chordProgression.canBeUsed(currentBpi)){
        bool needStrech = chordProgression.getBeatsPerInterval() != currentBpi;
        showChordsPanel( needStrech ? chordProgression.getStretchedVersion(currentBpi) : chordProgression );
    }
    else{
        int measures = chordProgression.getMeasures().size();
        QString msg = "These chords (" + QString::number(measures) + " measures) can't be use in a " + QString::number(currentBpi) + " bpi interval!";
        QMessageBox::warning(this, "Problem...", msg);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updatePeaks(){
    foreach (NinjamTrackGroupView* view, trackGroups) {
        if(view){
            view->updatePeaks();
        }
    }
    Audio::AudioPeak metronomePeak = mainController->getTrackPeak(Controller::NinjamController::METRONOME_TRACK_ID);
    ninjamPanel->setMetronomePeaks(metronomePeak.getLeft(), metronomePeak.getRight());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::on_channelXmitChanged(long channelID, bool transmiting){
    qCDebug(jtNinjamGUI) << "channel xmit changed:" << channelID << " state:" << transmiting;
    BaseTrackView* trackView = NinjamTrackView::getTrackViewByID(channelID);
    if(trackView){
        trackView->setUnlightStatus(!transmiting);
    }
}

void NinjamRoomWindow::on_channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID){
    qCDebug(jtNinjamGUI) << "channel removed:" << channel.getName();
    Q_UNUSED(channel);

    NinjamTrackGroupView* group = trackGroups[user.getFullName()];
    if(group){
        if(group->getTracksCount() == 1){//removing the last track, the group is removed too
            trackGroups.remove(user.getFullName());
            ui->tracksPanel->layout()->removeWidget(group);
            group->deleteLater();
        }
        else{//remove one subchannel
            BaseTrackView* trackView = BaseTrackView::getTrackViewByID(channelID);
            if(trackView){
                group->removeTrackView(trackView);
            }
        }
        adjustTracksWidth();
    }
}

void NinjamRoomWindow::on_channelNameChanged(Ninjam::User, Ninjam::UserChannel channel, long channelID){
    qCDebug(jtNinjamGUI) << "channel name changed:" << channel.getName();
    //Q_UNUSED(channel);
    NinjamTrackView* trackView = dynamic_cast<NinjamTrackView*>(NinjamTrackView::getTrackViewByID(channelID));
    if(trackView){
        trackView->setChannelName(channel.getName());
    }
}

void NinjamRoomWindow::on_channelAdded(Ninjam::User user, Ninjam::UserChannel channel, long channelID){
    qCDebug(jtNinjamGUI) << "channel added - creating channel view:" << user.getFullName() << " " << channel.getName();
    if(!trackGroups.contains(user.getFullName())){//first channel from this user?
        QString userName = user.getName();
        QString channelName = channel.getName();
        QString userIp = user.getIp();
        NinjamTrackGroupView* trackView = new NinjamTrackGroupView(ui->tracksPanel, this->mainController, channelID, userName, channelName, userIp );
        trackView->setNarrowStatus(!fullViewMode);
        ui->tracksPanel->layout()->addWidget(trackView);
        trackGroups.insert(user.getFullName(), trackView);
        adjustTracksWidth();
    }
    else{//the second, or third channel from same user, group with other channels
        NinjamTrackGroupView* trackView = trackGroups[user.getFullName()];
        trackView->addTrackView(new NinjamTrackView(mainController, channelID, channel.getName()));
    }
    qCDebug(jtNinjamGUI) << "channel view created";
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::adjustTracksWidth(){
    if(!fullViewMode){//in mini view mode tracks are always narrowed
        return;
    }

    int availableWidth = ui->scrollArea->width();

    //can use wide to all tracks?
    int wideWidth = 0;
    foreach (NinjamTrackGroupView* trackGroup, trackGroups) {
        int subTracks = trackGroup->getTracksCount();
        wideWidth += (subTracks > 1) ? BaseTrackView::WIDE_WIDTH : BaseTrackView::NARROW_WIDTH * subTracks;
    }
    bool canUseWideTracks = wideWidth <= availableWidth;
    foreach (NinjamTrackGroupView* trackGroup, trackGroups) {
        trackGroup->setNarrowStatus(!canUseWideTracks);
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NinjamRoomWindow::~NinjamRoomWindow(){
    qCDebug(jtNinjamGUI) << "NinjamRoomWindow destructor";
    Controller::NinjamController* ninjamController = mainController->getNinjamController();
    if(ninjamController){
        QObject::disconnect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(on_bpiChanged(int)));
        QObject::disconnect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(on_bpmChanged(int)));
        QObject::disconnect(ninjamController, SIGNAL(intervalBeatChanged(int)), this, SLOT(on_intervalBeatChanged(int)));
        QObject::disconnect(ninjamController, SIGNAL(channelAdded(Ninjam::User,   Ninjam::UserChannel, long)), this, SLOT(on_channelAdded(  Ninjam::User, Ninjam::UserChannel, long)));
        QObject::disconnect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(on_channelRemoved(Ninjam::User, Ninjam::UserChannel, long)));
        QObject::disconnect(ninjamController, SIGNAL(channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(on_channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)));
        QObject::disconnect(ninjamController, SIGNAL(chatMsgReceived(Ninjam::User,QString)), this, SLOT(on_chatMessageReceived(Ninjam::User,QString)));
        QObject::disconnect(ninjamController, SIGNAL(channelXmitChanged(long,bool)), this, SLOT(on_channelXmitChanged(long,bool)));
    }

    mainController->storeIntervalProgressShape( ninjamPanel->getIntervalShape());

    if(ninjamPanel){
        ninjamPanel->setParent(0);
        ninjamPanel->deleteLater();
        ninjamPanel = nullptr;
    }

    delete ui;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//ninjam controller events
void NinjamRoomWindow::on_bpiChanged(int bpi){
    ninjamPanel->setBpi(bpi);
    if(chordsPanel){
        bool bpiWasAccepted = chordsPanel->setBpi(bpi);
        if(!bpiWasAccepted){
            hideChordsPanel();
        }
    }
}

void NinjamRoomWindow::on_bpmChanged(int bpm){
    if(ninjamPanel){
        ninjamPanel->setBpm(bpm);
    }
}

void NinjamRoomWindow::on_intervalBeatChanged(int beat){
    if(ninjamPanel){
        ninjamPanel->setCurrentBeat(beat);
    }

    if(chordsPanel){
        chordsPanel->setCurrentBeat(beat);
    }
}

void NinjamRoomWindow::on_licenceButton_clicked()
{
    QString licence = mainController->getNinjamService()->getCurrentServerLicence();
    QMessageBox* msgBox = new QMessageBox(parentWidget());
    msgBox->setText(licence);
    msgBox->setWindowTitle(ui->labelRoomName->text());
    msgBox->setAttribute( Qt::WA_DeleteOnClose );


    //hack to set minimum width in QMEsageBox
    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox->layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    //QMessageBox::about(this, ui->labelRoomName->text(), licence);//, QMessageBox::NoButton, QMessageBox::NoButton);
    msgBox->show();

    if(mainController->isRunningAsVstPlugin()){
        QPoint basePosition = mapToGlobal(parentWidget()->pos());//parent widget is QTabWidget;
        int x = basePosition.x() + parentWidget()->width()/2 - msgBox->width()/2;
        int y = basePosition.y() + parentWidget()->height()/2 - msgBox->height()/2;
        msgBox->move(x,y);
    }

}

//----------
void NinjamRoomWindow::ninjamAccentsComboChanged(int /*index*/){
    int beatsPerAccent = ninjamPanel->getCurrentBeatsPerAccent();
    mainController->getNinjamController()->setMetronomeBeatsPerAccent(beatsPerAccent);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpiComboChanged(QString newText){
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    int newBpi = newText.toInt();
    if(newBpi == currentBpi){
        return;
    }

     mainController->getNinjamController()->voteBpi(newBpi);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpmComboChanged(QString newText){
    int currentBpm = mainController->getNinjamController()->getCurrentBpm();
    int newBpm = newText.toInt();
    if(newBpm == currentBpm){
        return;
    }

    mainController->getNinjamController()->voteBpm(newBpm);
}

