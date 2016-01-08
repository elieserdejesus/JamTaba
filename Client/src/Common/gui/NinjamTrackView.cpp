#include "NinjamTrackView.h"
#include "ui_TrackGroupView.h"
#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QStyle>
#include "MainController.h"
#include "Utils.h"

// +++++++++++++++++++++++++
NinjamTrackView::NinjamTrackView(Controller::MainController *mainController, long trackID) :
    BaseTrackView(mainController, trackID)
{
    channelNameLabel = new MarqueeLabel();
    channelNameLabel->setObjectName("channelName");
    channelNameLabel->setText("");

    ui->mainLayout->insertSpacing(0, 12);
    ui->mainLayout->insertWidget(1, channelNameLabel);

    chunksDisplay = new IntervalChunksDisplay(this);
    chunksDisplay->setObjectName("chunksDisplay");
    ui->mainLayout->addSpacing(6);
    ui->mainLayout->addWidget(chunksDisplay);

    setUnlightStatus(true);/** disabled/grayed until receive the first bytes. When the first bytes
    // are downloaded the 'on_channelXmitChanged' slot is executed and this track is enabled.*/

}

void NinjamTrackView::setInitialValues(const Persistence::CacheEntry &initialValues){
    cacheEntry = initialValues;

    // remember last track values
    ui->levelSlider->setValue(initialValues.getGain() * 100);
    ui->panSlider->setValue(initialValues.getPan() * ui->panSlider->maximum());
    if (initialValues.isMuted())
        ui->muteButton->click();
    if (initialValues.getBoost() < 1) {
        ui->buttonBoostMinus12->click();
    } else {
        if (initialValues.getBoost() > 1)
            ui->buttonBoostPlus12->click();
        else
            ui->buttonBoostZero->click();
    }
}

// +++++++++++++++
void NinjamTrackView::updateGuiElements()
{
    BaseTrackView::updateGuiElements();
    channelNameLabel->updateMarquee();
}

// +++++= interval chunks visual feedback ++++++=
void NinjamTrackView::setUnlightStatus(bool status)
{
    BaseTrackView::setUnlightStatus(status);
    chunksDisplay->setVisible(!status && chunksDisplay->isVisible());
}

void NinjamTrackView::finishCurrentDownload()
{
    if (chunksDisplay->isVisible())
        chunksDisplay->startNewInterval();
}

void NinjamTrackView::incrementDownloadedChunks()
{
    chunksDisplay->incrementDownloadedChunks();
}

void NinjamTrackView::setDownloadedChunksDisplayVisibility(bool visible)
{
    if (chunksDisplay->isVisible() != visible) {
        chunksDisplay->reset();
        chunksDisplay->setVisible(visible);
    }
}

// +++++++++++++++++++

void NinjamTrackView::setChannelName(const QString &name)
{
    this->channelNameLabel->setText(name);
    int nameWidth = this->channelNameLabel->fontMetrics().width(name);
    if (nameWidth <= this->channelNameLabel->contentsRect().width())
        this->channelNameLabel->setAlignment(Qt::AlignCenter);
    else
        this->channelNameLabel->setAlignment(Qt::AlignLeft);
    this->channelNameLabel->setToolTip(name);
}

// +++++++++++++++++++++

void NinjamTrackView::setPan(int value)
{
    BaseTrackView::setPan(value);
    cacheEntry.setPan(mainController->getTrackNode(getTrackID())->getPan());
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

void NinjamTrackView::setGain(int value)
{
    BaseTrackView::setGain(value);
    cacheEntry.setGain(value/100.0);
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

void NinjamTrackView::toggleMuteStatus()
{
    BaseTrackView::toggleMuteStatus();
    cacheEntry.setMuted(mainController->getTrackNode(getTrackID())->isMuted());
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

void NinjamTrackView::updateBoostValue()
{
    BaseTrackView::updateBoostValue();
    cacheEntry.setBoost(mainController->getTrackNode(getTrackID())->getBoost());
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamTrackGroupView::updateGeoLocation()
{
    Geo::Location location = mainController->getGeoLocation(this->userIP);
    countryLabel->setText(
        "<img src=:/flags/flags/" + location.getCountryCode().toLower() +".png> <br>"
        + location.getCountryName());
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

NinjamTrackGroupView::NinjamTrackGroupView(QWidget *parent,
                                           Controller::MainController *mainController, long trackID,
                                           QString channelName,
                                           Persistence::CacheEntry initialValues) :
    TrackGroupView(parent),
    mainController(mainController),
    userIP(initialValues.getUserIP())
{
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

    // change the top panel layout to vertical (original is horizontal)
    ui->topPanel->layout()->removeWidget(ui->groupNameField);
    delete ui->topPanel->layout();
    ui->topPanel->setLayout(new QVBoxLayout());
    ui->topPanel->layout()->setContentsMargins(3, 6, 3, 3);

    // replace the original QLineEdit with a MarqueeLabel

    groupNameLabel = new MarqueeLabel();
    delete ui->groupNameField;
    groupNameLabel->setObjectName("groupNameField");
    ui->topPanel->layout()->addWidget(groupNameLabel);

    setGroupName(initialValues.getUserName());

    // country flag label
    countryLabel = new QLabel();
    countryLabel->setObjectName("countryLabel");
    countryLabel->setTextFormat(Qt::RichText);
    updateGeoLocation();

    ui->topPanel->layout()->addWidget(countryLabel);

    // create the first subchannel by default
    NinjamTrackView* newTrackView = addTrackView(trackID);
    newTrackView->setChannelName(channelName);
    newTrackView->setInitialValues(initialValues);
}

NinjamTrackView *NinjamTrackGroupView::createTrackView(long trackID)
{
    return new NinjamTrackView(mainController, trackID);
}

void NinjamTrackGroupView::setGroupName(const QString &groupName)
{
    groupNameLabel->setText(groupName);
}

void NinjamTrackGroupView::setNarrowStatus(bool narrow)
{
    foreach (BaseTrackView *trackView, trackViews) {
        bool setToWide = !narrow && trackViews.size() <= 1;
        if (setToWide)
            trackView->setToWide();
        else
            trackView->setToNarrow();
    }
}

void NinjamTrackGroupView::updateGuiElements()
{
    TrackGroupView::updateGuiElements();
    groupNameLabel->updateMarquee();
}

NinjamTrackGroupView::~NinjamTrackGroupView()
{
}
