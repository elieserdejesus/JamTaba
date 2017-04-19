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
    channelNameLabel = createChannelNameLabel();

    chunksDisplay = new IntervalChunksDisplay(this);
    chunksDisplay->setVisible(false);

    buttonLowCut = createLowCutButton(false);
    updateLowCutButtonToolTip();

    buttonReceive = createReceiveButton();
    buttonReceive->setChecked(true); // receiving by default
    buttonReceive->setEnabled(false); // disabled until receive the first interval
    connect(buttonReceive, &QPushButton::toggled, this, &NinjamTrackView::setReceiveState);

    setupVerticalLayout();

    setActivatedStatus(true); // disabled/grayed until receive the first bytes.
}

void NinjamTrackView::setNinjamChannelData(const QString &userFullName, quint8 channelIndex)
{
    this->userFullName = userFullName;
    this->channelIndex = channelIndex;
}

void NinjamTrackView::setReceiveState(bool receive)
{
    setActivatedStatus(!receive);

    //send a message to ninjam server disabling channel receive status
    mainController->setChannelReceiveStatus(userFullName, channelIndex, receive);

    // stop rendering downloaded audio
    NinjamTrackNode *trackNode = dynamic_cast<NinjamTrackNode*>(mainController->getTrackNode(getTrackID()));
    trackNode->stopDecoding();
}

QPushButton *NinjamTrackView::createReceiveButton() const
{
    QPushButton *button = new QPushButton();
    button->setToolTip(tr("Receive"));
    button->setObjectName(QStringLiteral("receiveButton"));
    button->setCheckable(true);
    secondaryChildsLayout->addWidget(button);
    return button;
}

MarqueeLabel *NinjamTrackView::createChannelNameLabel() const
{
    MarqueeLabel *label = new MarqueeLabel();
    label->setObjectName("channelName");
    label->setText("");
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    label->setTextInteractionFlags(Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse);

    return label;
}

MultiStateButton *NinjamTrackView::createLowCutButton(bool checked)
{
    MultiStateButton *button = new MultiStateButton(3, this); // 3 states: Low cut OFF, NORMAL, and DRASTIC
    button->setCheckable(true);
    button->setChecked(checked);
    secondaryChildsLayout->addWidget(button);
    button->setObjectName("lowCutButton");
    connect(button, &QPushButton::clicked, this, &NinjamTrackView::setLowCutToNextState);
    return button;
}

void NinjamTrackView::updateLowCutButtonToolTip()
{
    Q_ASSERT(buttonLowCut);

    QString toolTipText = tr("Low cut") + " [" + getLowCutStateText() + "]";
    buttonLowCut->setToolTip(toolTipText);
}

QString NinjamTrackView::getLowCutStateText() const
{
    Q_ASSERT(mainController);

    NinjamTrackNode* trackNode = static_cast<NinjamTrackNode *>(mainController->getTrackNode(getTrackID()));

    if (trackNode != nullptr) {
        switch(trackNode->getLowCutState())
        {
        case NinjamTrackNode::OFF: return tr("Off");
        case NinjamTrackNode::NORMAl: return tr("Normal");
        case NinjamTrackNode::DRASTIC: return tr("Drastic");
        }
    }

    return tr("Off"); // just to be shure
}

void NinjamTrackView::updateStyleSheet()
{
    style()->unpolish(channelNameLabel);
    style()->polish(channelNameLabel);
    style()->unpolish(buttonLowCut);
    style()->polish(buttonLowCut);
    style()->polish(buttonReceive);

    BaseTrackView::updateStyleSheet();
}

void NinjamTrackView::setInitialValues(const Persistence::CacheEntry &initialValues)
{
    cacheEntry = initialValues;

    // remember last track values
    levelSlider->setValue(initialValues.getGain() * 100);
    panSlider->setValue(initialValues.getPan() * panSlider->maximum());
    if (initialValues.isMuted())
        muteButton->click();

    if (initialValues.getBoost() < 1.0) {
        buttonBoost->setState(1); // -12 dB
    } else {
        if (initialValues.getBoost() > 1.0) // +12 dB
            buttonBoost->setState(2);
        else
            buttonBoost->setState(0);
    }

    quint8 lowCutState = initialValues.getLowCutState();
    if (lowCutState < 3) { // Check for invalid lowCut state value, Low cut is 3 states: OFF, NOrmal and Drastic
        for (int var = 0; var < lowCutState; ++var) {
            buttonLowCut->click(); // force button state change
        }
    }
}

// +++++++++++++++
void NinjamTrackView::updateGuiElements()
{
    if (!isActivated())
        return;

    BaseTrackView::updateGuiElements();
    channelNameLabel->updateMarquee();
}

void NinjamTrackView::setActivatedStatus(bool deactivated)
{
    BaseTrackView::setActivatedStatus(deactivated);

    if (deactivated) {// remote user stop xmiting and the track is greyed/unlighted?
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
    updateStyleSheet();
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

    peakMeter->setOrientation(Qt::Horizontal);
    peakMeter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    metersLayout->setDirection(QBoxLayout::TopToBottom);

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
    if (downloadingFirstInterval){
        chunksDisplay->incrementDownloadedChunks();

        if (!chunksDisplay->isVisible())
            setDownloadedChunksDisplayVisibility(true);

        if (!buttonReceive->isEnabled())
            buttonReceive->setEnabled(true);
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

void NinjamTrackView::setLowCutToNextState()
{
    NinjamTrackNode* node = static_cast<NinjamTrackNode *>(mainController->getTrackNode(getTrackID()));
    if (node) {
        NinjamTrackNode::LowCutState newState = node->setLowCutToNextState();

        cacheEntry.setLowCutState(newState);
        mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);

        updateLowCutButtonToolTip();

        buttonLowCut->style()->unpolish(this);
        buttonLowCut->style()->polish(this);
    }
}
