#include "NinjamTrackView.h"
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

void NinjamTrackView::setInitialValues(const Persistence::CacheEntry &initialValues)
{
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

    if(status){//remote user stop xmiting and the track is greyed/unlighted?
        Audio::AudioNode *trackNode = mainController->getTrackNode(getTrackID());
        trackNode->resetLastPeak(); //reset the internal node last peak to avoid getting the last peak calculated when the remote user was transmiting.
    }
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
