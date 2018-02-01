#include "gui/JamRoomViewPanel.h"
#include "ui_JamRoomViewPanel.h"

#include <QDebug>
#include "MainController.h"
#include "ninjam/client/User.h"
#include "ninjam/client/UserChannel.h"
#include "ninjam/client/Server.h"
#include "MapWidget.h"
#include "MapMarker.h"

JamRoomViewPanel::JamRoomViewPanel(const login::RoomInfo &roomInfo, controller::MainController *mainController) :
    QFrame(nullptr),
    ui(new Ui::JamRoomViewPanel),
    mainController(mainController),
    roomInfo(roomInfo)
{
    ui->setupUi(this);

    ui->wavePeakPanel->setEnabled(false); // is enable when user click in listen button

    connect(mainController, &controller::MainController::ipResolved, this, &JamRoomViewPanel::updateUserLocation);
    connect(ui->buttonListen, &QPushButton::clicked, this, &JamRoomViewPanel::toggleRoomListening);
    connect(ui->buttonEnter, &QPushButton::clicked, this, &JamRoomViewPanel::enterInTheRoom);

    createMapWidgets();

    // add wave peak panel as a layer in top of map widget
    ui->content->layout()->removeWidget(ui->wavePeakPanel);
    QHBoxLayout *mapWidgetLayout = new QHBoxLayout();
    map->setLayout(mapWidgetLayout);
    mapWidgetLayout->addWidget(ui->wavePeakPanel);
    mapWidgetLayout->setContentsMargins(1, 1, 1, 1);
    mapWidgetLayout->setSpacing(3);

    waveDrawingButtonsLayout = new QVBoxLayout();
    waveDrawingButtonsLayout->setContentsMargins(0, 0, 0, 0);
    waveDrawingButtonsLayout->setSpacing(0);
    createWaveDrawingButtonsLayout(waveDrawingButtonsLayout);

    mapWidgetLayout->addLayout(waveDrawingButtonsLayout);
    mapWidgetLayout->setAlignment(waveDrawingButtonsLayout, Qt::AlignBottom);

    // 'remember' the wave drawing mode
    WavePeakPanel::WaveDrawingMode lastDrawingMode = static_cast<WavePeakPanel::WaveDrawingMode>(mainController->getLastWaveDrawingMode());
    setWaveDrawingMode(lastDrawingMode);

    initialize(roomInfo);
}

void JamRoomViewPanel::setWaveDrawingButtonsVisibility(bool showButtons)
{
    for (QPushButton *button : waveDrawingButtons.values()) {
        button->setVisible(showButtons);
    }

    map->setBlurMode(showButtons);
}

void JamRoomViewPanel::createWaveDrawingButtonsLayout(QLayout *layout)
{
    const static QString icons[4] =
    {
        "wave_normal",
        "wave_pixelated",
        "wave_buildings",
        "wave_buildings_pixelated"
    };

    const static WavePeakPanel::WaveDrawingMode drawingModes[4] =
    {
        WavePeakPanel::SOUND_WAVE,
        WavePeakPanel::PIXELED_SOUND_WAVE,
        WavePeakPanel::BUILDINGS,
        WavePeakPanel::PIXELED_BUILDINGS
    };

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    for (int i = 0; i < 4; ++i) {
        QPushButton *button = new QPushButton();
        button->setVisible(false);
        button->setCheckable(true);
        button->setObjectName("buttonWaveDrawingMode");
        button->setIcon(QIcon(":/images/" + icons[i] + ".png"));
        buttonGroup->addButton(button);
        layout->addWidget(button);

        WavePeakPanel::WaveDrawingMode drawingMode = drawingModes[i];
        waveDrawingButtons.insert(drawingMode, button);

        connect(button, &QPushButton::clicked, [drawingMode, this]{
            setWaveDrawingMode(drawingMode);
            mainController->storeWaveDrawingMode(drawingMode);
        });
    }
}

void JamRoomViewPanel::setWaveDrawingMode(WavePeakPanel::WaveDrawingMode mode)
{
    ui->wavePeakPanel->setDrawingMode(mode);

    QPushButton *button = waveDrawingButtons[mode];
    if (button) {
        if (!button->isChecked())
            button->click();
    }
}

void JamRoomViewPanel::createMapWidgets()
{
    map = new MapWidget(this);
    ui->content->layout()->addWidget(map);
}

void JamRoomViewPanel::updateUserLocation(const QString &userIP)
{
    Q_UNUSED(userIP)
    for (const auto &user : roomInfo.getUsers()) {
        if (user.getIp() == userIP) {
            updateMap();
            update();
            break;
        }
    }

}

void JamRoomViewPanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange){
        translateUi();
    }
    QFrame::changeEvent(e);
}

void JamRoomViewPanel::translateUi()
{
    ui->labelRoomStatus->setText(buildRoomDescriptionString());
    ui->buttonEnter->setText(tr("enter"));
    ui->buttonListen->setText(tr("listen"));

    updateButtonListen();
}

QString JamRoomViewPanel::buildRoomDescriptionString()
{
    int botsCount = roomInfo.getUsers().count() - roomInfo.getNonBotUsersCount();
    int maxUsers = roomInfo.getMaxUsers() - botsCount;
    int users = roomInfo.getNonBotUsersCount();
    QString roomDescription = tr("%1 / %2 players ").arg(
                QString::number(users),
                QString::number(maxUsers));

    if (roomInfo.getBpm() > 0)
        roomDescription += "  " + QString::number(roomInfo.getBpm()) + " BPM ";
    if (roomInfo.getType() == login::RoomTYPE::NINJAM && roomInfo.getBpi() > 0)
        roomDescription += "  " + QString::number(roomInfo.getBpi()) + " BPI";
    return roomDescription;
}

void JamRoomViewPanel::updateMap()
{
    if (!roomInfo.isEmpty()) {
        QList<login::UserInfo> userInfos = roomInfo.getUsers();
        qSort(userInfos.begin(), userInfos.end(), userInfoLessThan);
        QList<MapMarker> newMarkers;
        for (const auto &user : userInfos) {
            if (!userIsBot(user)) {
                auto userLocation = mainController->getGeoLocation(user.getIp());
                if (userLocation.isUnknown())
                    continue; // skip invalid locations

                QPointF latLong(userLocation.getLatitude(), userLocation.getLongitude());
                QPixmap flag(":/flags/flags/" + userLocation.getCountryCode().toLower() + ".png");
                MapMarker marker(user.getName(), userLocation.getCountryName(), latLong, flag.toImage());
                newMarkers.append(marker);
            }
        }

        map->setMarkers(newMarkers);
    }

    map->setVisible(!roomInfo.isEmpty());
    map->update();
}

void JamRoomViewPanel::refresh(const login::RoomInfo &roomInfo)
{
    this->roomInfo = roomInfo;

    ui->labelRoomStatus->setText(buildRoomDescriptionString());

    updateButtonListen();

    ui->buttonEnter->setEnabled(!roomInfo.isFull());

    updateMap();

    setProperty("empty", roomInfo.isEmpty());

    updateStyleSheet();

}

void JamRoomViewPanel::updateStyleSheet()
{
    style()->unpolish(this);
    style()->unpolish(ui->content);
    style()->unpolish(ui->labelName);
    style()->unpolish(ui->labelRoomStatus);
    style()->unpolish(ui->buttonListen);

    style()->polish(this);
    style()->polish(ui->content);
    style()->polish(ui->labelName);
    style()->polish(ui->labelRoomStatus);
    style()->polish(ui->buttonListen);

    update();
}

void JamRoomViewPanel::updateButtonListen()
{
    ui->buttonListen->setEnabled(roomInfo.hasStream() && !roomInfo.isEmpty());

    if (!roomInfo.hasStream()) {
        ui->buttonListen->setIcon(QIcon(":/images/warning.png"));
        ui->buttonListen->setToolTip(tr("The audio stream of this room is not available at moment!"));
    } else {
        ui->buttonListen->setIcon(QIcon()); // remove the icon
        ui->buttonListen->setToolTip(""); // clean the tooltip
    }

    style()->unpolish(ui->buttonListen);
    style()->polish(ui->buttonListen);
}

bool JamRoomViewPanel::userInfoLessThan(const login::UserInfo &u1, const login::UserInfo &u2)
{
    return u1.getName() < u2.getName();
}

void JamRoomViewPanel::addPeak(float peak)
{
    ui->wavePeakPanel->addPeak(peak);
}

void JamRoomViewPanel::setShowBufferingState(bool showBuffering)
{
    ui->wavePeakPanel->setShowBuffering(showBuffering);
}

void JamRoomViewPanel::setBufferingPercentage(int percentage)
{
    ui->wavePeakPanel->setBufferingPercentage(percentage);
}

bool JamRoomViewPanel::userIsBot(const login::UserInfo &userInfo)
{
    return mainController->getBotNames().contains(userInfo.getName());
}

bool JamRoomViewPanel::roomContainsBotsOnly(const login::RoomInfo &roomInfo)
{
    QStringList botsNames = mainController->getBotNames();
    for (const auto &user : roomInfo.getUsers()) {
        if (!botsNames.contains(user.getName()))
            return false;
    }
    return true;
}

bool JamRoomViewPanel::canShowNinjamServerPort(const QString &serverName)
{
    return serverName.startsWith("ninbot") || serverName.startsWith("ninjamer");
}

void JamRoomViewPanel::initialize(const login::RoomInfo &roomInfo)
{
    QString roomName = roomInfo.getName();
    if (roomName.endsWith(".com"))
        roomName = roomName.replace(".com", "");

    if (roomInfo.getType() == login::RoomTYPE::NINJAM && canShowNinjamServerPort(roomName))
        roomName += " (" + QString::number(roomInfo.getPort()) + ")";

    ui->labelName->setText(roomName);

    refresh(roomInfo);
}

JamRoomViewPanel::~JamRoomViewPanel()
{
    delete ui;
}

void JamRoomViewPanel::clear(bool resetListenButton)
{
    ui->wavePeakPanel->clearPeaks();
    ui->wavePeakPanel->setShowBuffering(false);

    if (resetListenButton)
        ui->buttonListen->setChecked(false);

    updateButtonListen();
    updateStyleSheet();

    setWaveDrawingButtonsVisibility(false);
}

void JamRoomViewPanel::toggleRoomListening()
{
    bool listening = ui->buttonListen->isChecked();
    if (listening)
        emit startingListeningTheRoom(roomInfo);
    else
        emit finishingListeningTheRoom(roomInfo);

    ui->wavePeakPanel->setEnabled(listening);
    ui->wavePeakPanel->updateGeometry();

    map->setBlurMode(listening); // when listening the map is drawed with a tranparent black layer in top

    setWaveDrawingButtonsVisibility(listening); // when listening the buttons appears
}

void JamRoomViewPanel::enterInTheRoom()
{
    emit enteringInTheRoom(roomInfo);
}
