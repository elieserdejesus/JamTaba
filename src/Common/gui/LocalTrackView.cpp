#include "LocalTrackView.h"
#include "ui_BaseTrackView.h"
#include "FxPanel.h"
#include "FxPanelItem.h"
#include "plugins/Guis.h"
#include "MainController.h"
#include "midi/MidiDriver.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/AudioNode.h"
#include "Highligther.h"
#include <QMenu>
#include <QToolButton>

#include "log/Logging.h"

// LocalTrackView::LocalTrackView(Controller::MainController *mainController, int channelIndex,
// float initialGain, BaseTrackView::BoostValue boostValue,
// float initialPan, bool muted) :
// BaseTrackView(mainController, 1),
// inputNode(nullptr)
// {
// init(channelIndex, initialGain, boostValue, initialPan, muted);
// }

LocalTrackView::LocalTrackView(Controller::MainController *mainController, int channelIndex) :
    BaseTrackView(mainController, 1),
    inputNode(nullptr)
{
    init(channelIndex, 1, BaseTrackView::BoostValue::ZERO, 0, false);// unit gain and pan in center, not muted
}

void LocalTrackView::setInitialValues(float initialGain, BaseTrackView::BoostValue boostValue,
                                      float initialPan, bool muted)
{
    inputNode->setGain(initialGain);
    inputNode->setPan(initialPan);
    initializeBoostButtons(boostValue);
    if (muted)
        inputNode->setMute(muted);
}

void LocalTrackView::detachMainController()
{
    this->mainController = nullptr;
}

void LocalTrackView::closeAllPlugins()
{
    inputNode->closeProcessorsWindows();// close vst editors
}

void LocalTrackView::init(int channelIndex, float initialGain, BaseTrackView::BoostValue boostValue,
                          float initialPan, bool muted)
{
    if (!mainController) {
        qCritical() << "LocalTrackView::init() mainController is null!";
        return;
    }

    // insert a input node in controller
    inputNode = new Audio::LocalInputAudioNode(channelIndex);
    trackID = mainController->addInputTrackNode(this->inputNode);
    bindThisViewWithTrackNodeSignals();// now is secure bind this LocalTrackView with the respective TrackNode model

    setInitialValues(initialGain, boostValue, initialPan, muted);

    setUnlightStatus(false);

    peakMetersOnly = false;
}

void LocalTrackView::mute(bool b)
{
    getInputNode()->setMute(b);// audio only
    ui->muteButton->setChecked(b);// gui only
}

void LocalTrackView::solo(bool b)
{
    getInputNode()->setSolo(b);// audio only
    ui->soloButton->setChecked(b);// gui only
}

void LocalTrackView::initializeBoostButtons(BoostValue boostValue)
{
    switch (boostValue) {
    case BoostValue::MINUS:
        ui->buttonBoostMinus12->click();
        break;
    case BoostValue::PLUS:
        ui->buttonBoostPlus12->click();
        break;
    default:
        ui->buttonBoostZero->click();
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QSize LocalTrackView::sizeHint() const
{
    if (peakMetersOnly) {
        int width = ui->mainWidget->sizeHint().width() + layout()->contentsMargins().left()
                    + layout()->contentsMargins().right();
        return QSize(width, height());
    }
    return BaseTrackView::sizeHint();
}

void LocalTrackView::setPeakMetersOnlyMode(bool peakMetersOnly, bool runningInMiniMode)
{
    if (this->peakMetersOnly != peakMetersOnly) {
        this->peakMetersOnly = peakMetersOnly;
        ui->boostPanel->setVisible(!this->peakMetersOnly);
        ui->leftWidget->setVisible(!this->peakMetersOnly);

        ui->panSpacer->changeSize(20, 20, QSizePolicy::Minimum,
                                  this->peakMetersOnly ? QSizePolicy::Ignored : QSizePolicy::Fixed);

        QMargins margins = layout()->contentsMargins();
        margins.setLeft((peakMetersOnly || runningInMiniMode) ? 2 : 6);
        margins.setRight((peakMetersOnly || runningInMiniMode) ? 2 : 6);
        layout()->setContentsMargins(margins);

        ui->soloButton->setVisible(!peakMetersOnly);
        ui->muteButton->setVisible(!peakMetersOnly);
        ui->peaksDbLabel->setVisible(!peakMetersOnly);
        ui->levelSlider->parentWidget()->layout()->setAlignment(ui->levelSlider,
                                                                peakMetersOnly ? Qt::AlignRight : Qt::AlignHCenter);

        this->drawDbValue = !peakMetersOnly;

        updateGeometry();

        setProperty("faderOnly", QVariant(peakMetersOnly));
        style()->unpolish(this);
        style()->polish(this);
    }
}

void LocalTrackView::togglePeakMetersOnlyMode(bool runninsInMiniMode)
{
    setPeakMetersOnlyMode(!peakMetersOnly, runninsInMiniMode);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::setUnlightStatus(bool unlighted)
{
    BaseTrackView::setUnlightStatus(unlighted);
    update();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Audio::LocalInputAudioNode *LocalTrackView::getInputNode() const
{
    return inputNode;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LocalTrackView::reset()
{
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LocalTrackView::~LocalTrackView()
{
    if (mainController)
        mainController->removeInputTrackNode(getTrackID());
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
