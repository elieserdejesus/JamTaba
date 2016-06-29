#include "NinjamTrackView.h"
#include "BaseTrackView.h"
#include "PeakMeter.h"
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QSlider>
#include <QStyle>
#include "MainController.h"
#include "Utils.h"
#include "audio/NinjamTrackNode.h"

const int NinjamTrackView::WIDE_HEIGHT = 70; //height used in horizontal layout for wide tracks

// +++++++++++++++++++++++++
NinjamTrackView::NinjamTrackView(Controller::MainController *mainController, long trackID) :
    BaseTrackView(mainController, trackID),
    orientation(Qt::Vertical),
    downloadingFirstInterval(true)
{
    channelNameLabel = new MarqueeLabel();
    channelNameLabel->setObjectName("channelName");
    channelNameLabel->setText("");
    channelNameLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

    chunksDisplay = new IntervalChunksDisplay(this);
    chunksDisplay->setVisible(false);

    buttonLowCut = createLowCutButton(false);

    setupVerticalLayout();

    setUnlightStatus(true); // disabled/grayed until receive the first bytes.
}

QPushButton *NinjamTrackView::createLowCutButton(bool checked)
{
    QPushButton *button = new QPushButton(this);
    button->setCheckable(true);
    button->setChecked(checked);
    secondaryChildsLayout->addWidget(button);
    button->setObjectName("lowCutButton");
    button->setToolTip(tr("Low cut"));
    connect(button, &QPushButton::clicked, this, &NinjamTrackView::setLowCutStatus);
    return button;
}

void NinjamTrackView::refreshStyleSheet()
{
    style()->unpolish(channelNameLabel);
    style()->polish(channelNameLabel);
    style()->unpolish(buttonLowCut);
    style()->polish(buttonLowCut);
    BaseTrackView::refreshStyleSheet();
}

void NinjamTrackView::setInitialValues(const Persistence::CacheEntry &initialValues)
{
    cacheEntry = initialValues;

    // remember last track values
    levelSlider->setValue(initialValues.getGain() * 100);
    panSlider->setValue(initialValues.getPan() * panSlider->maximum());
    if (initialValues.isMuted())
        muteButton->click();
    if (initialValues.getBoost() < 1) {
        buttonBoostMinus12->click();
    } else {
        if (initialValues.getBoost() > 1)
            buttonBoostPlus12->click();
        else
            buttonBoostZero->click();
    }

    if (initialValues.isLowCutActivated())
        buttonLowCut->click();
}

// +++++++++++++++
void NinjamTrackView::updateGuiElements()
{
    BaseTrackView::updateGuiElements();
    channelNameLabel->updateMarquee();
}

void NinjamTrackView::setUnlightStatus(bool unlighted)
{
    BaseTrackView::setUnlightStatus(unlighted);

    if (unlighted) {// remote user stop xmiting and the track is greyed/unlighted?
        Audio::AudioNode *trackNode = mainController->getTrackNode(getTrackID());
        if (trackNode)
            trackNode->resetLastPeak(); // reset the internal node last peak to avoid getting the last peak calculated when the remote user was transmiting.

        downloadingFirstInterval = true; //waiting for the first interval
        chunksDisplay->reset();
    }
}

QSize NinjamTrackView::sizeHint() const
{
    if (orientation == Qt::Horizontal) {
        if (narrowed)
            return QWidget::sizeHint();
        else
            return QSize(width(), WIDE_HEIGHT);
    }
    return BaseTrackView::sizeHint();
}

void NinjamTrackView::setOrientation(Qt::Orientation newOrientation)
{
    if (this->orientation == newOrientation)
        return;

    this->orientation = newOrientation;
    if (newOrientation == Qt::Horizontal)
        setupHorizontalLayout();
    else
        setupVerticalLayout();

    setProperty("horizontal", newOrientation == Qt::Horizontal ? true : false);
    refreshStyleSheet();
}

void NinjamTrackView::setupVerticalLayout()
{
    BaseTrackView::setupVerticalLayout();

    mainLayout->removeWidget(channelNameLabel);
    mainLayout->removeItem(primaryChildsLayout);
    mainLayout->removeItem(secondaryChildsLayout);
    mainLayout->removeWidget(chunksDisplay);

    mainLayout->addWidget(channelNameLabel, 0, 0, 1, 2);// insert channel name label in top
    mainLayout->addLayout(primaryChildsLayout, 1, 0);
    mainLayout->addLayout(secondaryChildsLayout, 1, 1);
    mainLayout->addWidget(chunksDisplay, 2, 0, 1, 2); // append chunks display in bottom

    primaryChildsLayout->setDirection(QBoxLayout::TopToBottom);
    secondaryChildsLayout->setDirection(QBoxLayout::TopToBottom);

}

void NinjamTrackView::setupHorizontalLayout()
{
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    mainLayout->removeWidget(channelNameLabel);
    mainLayout->removeItem(primaryChildsLayout);
    mainLayout->removeItem(secondaryChildsLayout);
    mainLayout->removeWidget(chunksDisplay);

    mainLayout->addWidget(channelNameLabel, 0, 0);
    mainLayout->addLayout(primaryChildsLayout, 0, 1);
    mainLayout->addLayout(secondaryChildsLayout, 1, 0, 1, 2);
    mainLayout->addWidget(chunksDisplay, 2, 0, 1, 2); // append chunks display in bottom

    mainLayout->setContentsMargins(6, 3, 6, 3);
    mainLayout->setVerticalSpacing(6);

    primaryChildsLayout->setDirection(QBoxLayout::RightToLeft);
    secondaryChildsLayout->setDirection(QBoxLayout::LeftToRight);

    levelSlider->setOrientation(Qt::Horizontal);
    levelSliderLayout->setDirection(QBoxLayout::RightToLeft);

    boostWidgetsLayout->setDirection(QHBoxLayout::RightToLeft);

    peakMeterLeft->setOrientation(Qt::Horizontal);
    peakMeterLeft->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    peakMeterRight->setOrientation(Qt::Horizontal);
    peakMeterRight->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    metersLayout->setDirection(QBoxLayout::TopToBottom);
    meterWidgetsLayout->setDirection(QBoxLayout::LeftToRight);

    muteSoloLayout->setDirection(QHBoxLayout::LeftToRight);
}


QPoint NinjamTrackView::getDbValuePosition(const QString &dbValueText,
                                           const QFontMetrics &metrics) const
{
    if (orientation == Qt::Vertical)
        return BaseTrackView::getDbValuePosition(dbValueText, metrics);

    float sliderPosition = (double)levelSlider->value()/levelSlider->maximum();
    int textX = sliderPosition * levelSlider->width() + levelSlider->x()
                - metrics.width(dbValueText) - FADER_HEIGHT;
    int textY = levelSlider->y() + levelSlider->height() + 3;
    return QPoint(textX, textY);
}

void NinjamTrackView::finishCurrentDownload()
{
     if(downloadingFirstInterval){
        chunksDisplay->finish();
        downloadingFirstInterval = false;
        setDownloadedChunksDisplayVisibility(false);
     }
}

void NinjamTrackView::incrementDownloadedChunks()
{
    if(downloadingFirstInterval){
        chunksDisplay->incrementDownloadedChunks();
        if(!chunksDisplay->isVisible())
            setDownloadedChunksDisplayVisibility(true);
    }
}

void NinjamTrackView::setEstimatedChunksPerInterval(int estimatedChunks)
{
    chunksDisplay->setEstimatedTotalChunks(estimatedChunks);
}

void NinjamTrackView::setDownloadedChunksDisplayVisibility(bool visible)
{
    chunksDisplay->reset();
    chunksDisplay->setVisible(visible);
}

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
    Audio::AudioNode *trackNode = mainController->getTrackNode(getTrackID());
    if (trackNode) {
        cacheEntry.setBoost(trackNode->getBoost());
        mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
    }
}

void NinjamTrackView::setLowCutStatus(bool activated)
{
    NinjamTrackNode* node = static_cast<NinjamTrackNode *>(mainController->getTrackNode(getTrackID()));
    if (node) {
        node->setLowCutStatus(activated);
        cacheEntry.setLowCutActivated(activated);
        mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
    }
}
