#include "NinjamRoomWindow.h"
#include "ui_NinjamRoomWindow.h"
#include "NinjamTrackView.h"

#include <QComboBox>
#include <QDebug>

#include "../ninjam/User.h"
#include "../ninjam/Server.h"

#include "../audio/MetronomeTrackNode.h"
#include "../audio/core/AudioDriver.h"

#include "../NinjamController.h"
#include "../MainController.h"
#include "../ninjam/Service.h"

#include <cassert>

#include "NinjamPanel.h"
#include "ChatPanel.h"

#include <QMessageBox>

Q_LOGGING_CATEGORY(ninjamRoomWindow, "ninjamRoomWindow")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//VoteConfirmationDialog::VoteConfirmationDialog(QWidget *parent, QString title, QString text, int voteValue, VoteConfirmationType voteType)
//    :QMessageBox(parent), voteValue(voteValue), voteType(voteType)
//{
//    setIcon(QMessageBox::Warning);
//    setText(text);
//    setWindowTitle(title);
//    setStandardButtons(QMessageBox::Yes|QMessageBox::No);

//    setAttribute(Qt::WA_DeleteOnClose, true);
//}

//+++++++++++++++++++++++++

NinjamRoomWindow::NinjamRoomWindow(QWidget *parent, Login::RoomInfo roomInfo, Controller::MainController *mainController) :
    QWidget(parent),
    ui(new Ui::NinjamRoomWindow),
    mainController(mainController),
    chatPanel(new ChatPanel(this, mainController->getBotNames() ))
    //voteConfirmationDialog(nullptr)
{
    qCDebug(ninjamRoomWindow) << "NinjamRoomWindow construtor..";
    ui->setupUi(this);

    ui->licenceButton->setIcon(QIcon(QPixmap(":/images/licence.png")));

    ui->labelRoomName->setText(roomInfo.getName() + " (" + QString::number(roomInfo.getPort()) + ")");

    ui->tracksPanel->layout()->setAlignment(Qt::AlignLeft);//tracks are left aligned

    qCDebug(ninjamRoomWindow) << "connecting signals in ninjamController...";
    Controller::NinjamController* ninjamController = mainController->getNinjamController();
    QObject::connect(ninjamController, SIGNAL(currentBpiChanged(int)), this, SLOT(on_bpiChanged(int)));
    QObject::connect(ninjamController, SIGNAL(currentBpmChanged(int)), this, SLOT(on_bpmChanged(int)));
    QObject::connect(ninjamController, SIGNAL(intervalBeatChanged(int)), this, SLOT(on_intervalBeatChanged(int)));
    QObject::connect(ninjamController, SIGNAL(channelAdded(Ninjam::User,   Ninjam::UserChannel, long)), this, SLOT(on_channelAdded(  Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(channelRemoved(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(on_channelRemoved(Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)), this, SLOT(on_channelNameChanged(Ninjam::User, Ninjam::UserChannel, long)));
    QObject::connect(ninjamController, SIGNAL(chatMsgReceived(Ninjam::User,QString)), this, SLOT(on_chatMessageReceived(Ninjam::User,QString)));
    QObject::connect(ninjamController, SIGNAL(channelXmitChanged(long,bool)), this, SLOT(on_channelXmitChanged(long,bool)));

    QObject::connect(ui->topPanel->getBpiCombo(), SIGNAL(activated(QString)), this, SLOT(ninjamBpiComboChanged(QString)));
    QObject::connect(ui->topPanel->getBpmCombo(), SIGNAL(activated(QString)), this, SLOT(ninjamBpmComboChanged(QString)));
    QObject::connect(ui->topPanel->getAccentsCombo(), SIGNAL(currentIndexChanged(int)), this, SLOT(ninjamAccentsComboChanged(int)));

    QString serverLicence = Ninjam::Service::getInstance()->getCurrentServerLicence();
    ui->licenceButton->setVisible(!serverLicence.isEmpty());

    QObject::connect(chatPanel, SIGNAL(userSendingNewMessage(QString)), this, SLOT(userSendingNewChatMessage(QString)));

    float initialMetronomeGain = mainController->getSettings().getMetronomeGain();
    float initialMetronomePan = mainController->getSettings().getMetronomePan();
    bool initialMetronomeMuteStatus = mainController->getSettings().getMetronomeMuteStatus();

    ui->topPanel->getGainSlider()->setValue(100*initialMetronomeGain);
    ui->topPanel->getPanSlider()->setValue(4 * initialMetronomePan);
    ui->topPanel->getMuteButton()->setChecked(initialMetronomeMuteStatus);
    ui->topPanel->setIntervalShape(mainController->getSettings().getIntervalProgressShape());

    initializeMetronomeEvents();//signals and slots

    QObject::connect(chatPanel, SIGNAL(userConfirmingVoteToBpiChange(int)), this, SLOT(on_userConfirmingVoteToChangeBpi(int)));
    QObject::connect(chatPanel,SIGNAL(userConfirmingVoteToBpmChange(int)), this, SLOT(on_userConfirmingVoteToChangeBpm(int)));

    //testing many tracks
//    for (int t = 0; t < 16; ++t) {
//        BaseTrackView* trackView = new NinjamTrackView(ui->tracksPanel, this->mainController, t, "User", QString::number(t), "BR", "BR" );
//        ui->tracksPanel->layout()->addWidget(trackView);
//    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updateGeoLocations(){
    foreach (NinjamTrackGroupView* trackGroup, trackGroups) {
        trackGroup->updateGeoLocation();
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::initializeMetronomeEvents(){
    QObject::connect( ui->topPanel->getGainSlider(), SIGNAL(valueChanged(int)), this, SLOT(onFaderMoved(int)));
    QObject::connect( ui->topPanel->getPanSlider(), SIGNAL(valueChanged(int)), this, SLOT(onPanSliderMoved(int)));
    QObject::connect( ui->topPanel->getMuteButton(), SIGNAL(clicked(bool)), this, SLOT(onMuteClicked()));
    QObject::connect( ui->topPanel->getSoloButton(), SIGNAL(clicked(bool)), this, SLOT(onSoloClicked()));
}

void NinjamRoomWindow::onPanSliderMoved(int value){
    float sliderValue = value/(float)ui->topPanel->getPanSlider()->maximum();
    mainController->setTrackPan(Controller::NinjamController::METRONOME_TRACK_ID, sliderValue);
}

void NinjamRoomWindow::onFaderMoved(int value){
    mainController->setTrackLevel(Controller::NinjamController::METRONOME_TRACK_ID, value/100.0);
}

void NinjamRoomWindow::onMuteClicked(){
    mainController->setTrackMute(Controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsMuted(Controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::onSoloClicked(){
    mainController->setTrackSolo(Controller::NinjamController::METRONOME_TRACK_ID, !mainController->trackIsSoloed(Controller::NinjamController::METRONOME_TRACK_ID));
}

void NinjamRoomWindow::userSendingNewChatMessage(QString msg){
    mainController->getNinjamController()->sendChatMessage(msg);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::on_chatMessageReceived(Ninjam::User user, QString message){
    //qDebug() << user.getFullName() << message;
    chatPanel->addMessage(user.getName(), message);

    //local user is voting?
    static long long lastVoteCommand = 0;
    QString localUserFullName = Ninjam::Service::getInstance()->getConnectedUserName();
    if (user.getFullName() == localUserFullName && message.toLower().contains("!vote")) {
        lastVoteCommand = QDateTime::currentMSecsSinceEpoch();
    }
    bool isVoteMessage = !message.isNull() && message.toLower().startsWith("[voting system] leading candidate:");
    long timeSinceLastVote = QDateTime::currentMSecsSinceEpoch() - lastVoteCommand;
    if (isVoteMessage && timeSinceLastVote >= 1000) {
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

//void NinjamRoomWindow::on_voteConfirmationDialogClosed(QAbstractButton *button){
//    if(this->voteConfirmationDialog){
//        if(this->voteConfirmationDialog->standardButton(button) == QMessageBox::Yes){
//            if(mainController->isPlayingInNinjamRoom()){
//                Controller::NinjamController* controller = mainController->getNinjamController();
//                if(controller){
//                    if(voteConfirmationDialog->getVoteType() == VoteConfirmationType::BPI_CONFIRMATION_VOTE){
//                        controller->voteBpi(voteConfirmationDialog->getVoteValue());
//                    }
//                    else{
//                        controller->voteBpm(voteConfirmationDialog->getVoteValue());
//                    }
//                }
//            }
//        }
//        this->voteConfirmationDialog = nullptr;
//    }
//}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::updatePeaks(){
    foreach (NinjamTrackGroupView* view, trackGroups) {
        if(view){
            view->updatePeaks();
        }
    }
    Audio::AudioPeak metronomePeak = mainController->getTrackPeak(Controller::NinjamController::METRONOME_TRACK_ID);
    ui->topPanel->setMetronomePeaks(metronomePeak.getLeft(), metronomePeak.getRight());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::on_channelXmitChanged(long channelID, bool transmiting){
    qCDebug(ninjamRoomWindow) << "channel xmit changed:" << channelID << " state:" << transmiting;
    BaseTrackView* trackView = NinjamTrackView::getTrackViewByID(channelID);
    if(trackView){
        trackView->setEnabled(transmiting);
    }
}

void NinjamRoomWindow::on_channelRemoved(Ninjam::User user, Ninjam::UserChannel channel, long channelID){
    qCDebug(ninjamRoomWindow) << "channel removed:" << channel.getName();
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
    qCDebug(ninjamRoomWindow) << "channel name changed:" << channel.getName();
    //Q_UNUSED(channel);
    NinjamTrackView* trackView = dynamic_cast<NinjamTrackView*>(NinjamTrackView::getTrackViewByID(channelID));
    if(trackView){
        trackView->setChannelName(channel.getName());
    }
}

void NinjamRoomWindow::on_channelAdded(Ninjam::User user, Ninjam::UserChannel channel, long channelID){
    qCDebug(ninjamRoomWindow) << "channel added - creating channel view:" << user.getFullName() << " " << channel.getName();
    if(!trackGroups.contains(user.getFullName())){//first channel from this user?
        QString userName = user.getName();
        QString channelName = channel.getName();
        QString userIp = user.getIp();
        NinjamTrackGroupView* trackView = new NinjamTrackGroupView(ui->tracksPanel, this->mainController, channelID, userName, channelName, userIp );
        ui->tracksPanel->layout()->addWidget(trackView);
        trackGroups.insert(user.getFullName(), trackView);
        adjustTracksWidth();
    }
    else{//the second, or third channel from same user, group with other channels
        NinjamTrackGroupView* trackView = trackGroups[user.getFullName()];
        trackView->addTrackView(new NinjamTrackView(mainController, channelID, channel.getName()));
    }
    qCDebug(ninjamRoomWindow) << "channel view created";
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamAccentsComboChanged(int /*index*/){
    //qDebug() << ui->topPanel->getAccentsCombo()->currentData();
    int beatsPerAccent = ui->topPanel->getAccentsCombo()->currentData().toInt();
    mainController->getNinjamController()->setMetronomeBeatsPerAccent(beatsPerAccent);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpiComboChanged(QString newText){
    int currentBpi = mainController->getNinjamController()->getCurrentBpi();
    int newBpi = newText.toInt();
    if(newBpi == currentBpi){
        return;
    }
    QMessageBox::StandardButton reply;
    QString message = "Vote to change the BPI to " + QString::number(newBpi) + "?";
    reply = QMessageBox::question(this, "Changing BPI ...", message,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        mainController->getNinjamController()->voteBpi(newBpi);
    }
    else{
        ui->topPanel->getBpiCombo()->blockSignals(true);
        ui->topPanel->getBpiCombo()->setCurrentText(QString::number(currentBpi));
        ui->topPanel->getBpiCombo()->blockSignals(false);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::ninjamBpmComboChanged(QString newText){
    int currentBpm = mainController->getNinjamController()->getCurrentBpm();
    int newBpm = newText.toInt();
    if(newBpm == currentBpm){
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changing BPM ...", "Vote to change the BPM to " + QString::number(newBpm) + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        mainController->getNinjamController()->voteBpm(newBpm);
    }
    else{
        ui->topPanel->getBpmCombo()->blockSignals(true);
        ui->topPanel->getBpmCombo()->setCurrentText(QString::number(currentBpm));
        ui->topPanel->getBpmCombo()->blockSignals(false);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamRoomWindow::adjustTracksWidth(){
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
    qCDebug(ninjamRoomWindow) << "NinjamRoomWindow destructor";
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

    mainController->storeIntervalProgressShape( ui->topPanel->getIntervalShape());

    delete ui;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//ninjam controller events
void NinjamRoomWindow::on_bpiChanged(int bpi){
    ui->topPanel->setBpi(bpi);
}

void NinjamRoomWindow::on_bpmChanged(int bpm){
    ui->topPanel->setBpm(bpm);
}

void NinjamRoomWindow::on_intervalBeatChanged(int beat){
    ui->topPanel->setCurrentBeat(beat);
}

void NinjamRoomWindow::on_licenceButton_clicked()
{
    QString licence = Ninjam::Service::getInstance()->getCurrentServerLicence();
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setText(licence);
    msgBox->setWindowTitle(ui->labelRoomName->text());
    msgBox->setAttribute( Qt::WA_DeleteOnClose );

    //hack to set minimum width in QMEsageBox
    QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox->layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    //QMessageBox::about(this, ui->labelRoomName->text(), licence);//, QMessageBox::NoButton, QMessageBox::NoButton);
    msgBox->show();
}
