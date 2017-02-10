#include "LocalTrackView.h"
#include "MainController.h"
#include "audio/core/LocalInputNode.h"
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QStyle>

LocalTrackView::LocalTrackView(Controller::MainController *mainController, int channelIndex) :
    BaseTrackView(mainController, channelIndex),
    inputNode(nullptr),
    peakMetersOnly(false),
    buttonStereoInversion(createStereoInversionButton())
{
    Q_ASSERT(mainController);

    // insert a input node in controller
    inputNode = new Audio::LocalInputNode(mainController, channelIndex);
    trackID = mainController->addInputTrackNode(this->inputNode);
    bindThisViewWithTrackNodeSignals();// now is secure bind this LocalTrackView with the respective TrackNode model

    setInitialValues(1.0f, BaseTrackView::Boost::ZERO, 0.0f, false, false);

    setActivatedStatus(false);

    secondaryChildsLayout->addWidget(buttonStereoInversion);

}

void LocalTrackView::bindThisViewWithTrackNodeSignals()
{
    BaseTrackView::bindThisViewWithTrackNodeSignals();

    connect(inputNode, &Audio::LocalInputNode::stereoInversionChanged, this, &LocalTrackView::setStereoInversion);
}

void LocalTrackView::setInitialValues(float initialGain, BaseTrackView::Boost boostValue,
                                      float initialPan, bool muted, bool stereoInverted)
{
    inputNode->setGain(initialGain);
    inputNode->setPan(initialPan);
    initializeBoostButtons(boostValue);
    if (muted)
        inputNode->setMute(muted);

    setStereoInversion(stereoInverted);
}

void LocalTrackView::detachMainController()
{
    this->mainController = nullptr;
}

void LocalTrackView::closeAllPlugins()
{
    inputNode->closeProcessorsWindows();// close vst editors
}

void LocalTrackView::mute(bool b)
{
    getInputNode()->setMute(b);// audio only
    muteButton->setChecked(b);// gui only
}

void LocalTrackView::solo(bool b)
{
    getInputNode()->setSolo(b);// audio only
    soloButton->setChecked(b);// gui only
}

void LocalTrackView::initializeBoostButtons(Boost boostValue)
{
    switch (boostValue) {
    case Boost::MINUS:
        buttonBoostMinus12->click();
        break;
    case Boost::PLUS:
        buttonBoostPlus12->click();
        break;
    default:
        buttonBoostZero->click();
    }
}

QSize LocalTrackView::sizeHint() const
{
    if (peakMetersOnly) {
        return QSize(16, height());
    }
    return BaseTrackView::sizeHint();
}

void LocalTrackView::setupMetersLayout()
{
    metersLayout->addWidget(peakMeterLeft);
    metersLayout->addWidget(peakMeterRight);
}

void LocalTrackView::setPeakMetersOnlyMode(bool peakMetersOnly)
{
    if (this->peakMetersOnly != peakMetersOnly) {
        this->peakMetersOnly = peakMetersOnly;

        BaseTrackView::setLayoutWidgetsVisibility(secondaryChildsLayout, !this->peakMetersOnly);
        BaseTrackView::setLayoutWidgetsVisibility(primaryChildsLayout, !this->peakMetersOnly);

        if(peakMetersOnly){//add the peak meters directly in main layout, so these meters are horizontally centered
            mainLayout->addWidget(peakMeterLeft, 0, 0);
            mainLayout->addWidget(peakMeterRight, 0, 1);
        }
        else{// put the meter in the original layout
            setupMetersLayout();
        }

        const static int spacing = 3;

        mainLayout->setHorizontalSpacing(spacing);

        peakMeterLeft->setVisible(true);//peak meters are always visible
        peakMeterRight->setVisible(true);

        QMargins margins = layout()->contentsMargins();
        margins.setLeft(spacing);
        margins.setRight(spacing);
        layout()->setContentsMargins(margins);

        soloButton->setVisible(!peakMetersOnly);
        muteButton->setVisible(!peakMetersOnly);
        peaksDbLabel->setVisible(!peakMetersOnly);
        Qt::Alignment alignment = peakMetersOnly ? Qt::AlignRight : Qt::AlignHCenter;
        levelSlider->parentWidget()->layout()->setAlignment(levelSlider, alignment);

        this->drawDbValue = !peakMetersOnly;

        updateGeometry();

        setProperty("peakMetersOnly", QVariant(peakMetersOnly));
        style()->unpolish(this);
        style()->polish(this);
    }
}

void LocalTrackView::togglePeakMetersOnlyMode()
{
    setPeakMetersOnlyMode(!peakMetersOnly);
}

void LocalTrackView::setActivatedStatus(bool unlighted)
{
    BaseTrackView::setActivatedStatus(unlighted);
    update();
}

Audio::LocalInputNode *LocalTrackView::getInputNode() const
{
    return inputNode;
}

void LocalTrackView::reset()
{
    mainController->resetTrack(getTrackID());
}

LocalTrackView::~LocalTrackView()
{
    if (mainController)
        mainController->removeInputTrackNode(getTrackID());
}


QPushButton *LocalTrackView::createStereoInversionButton()
{
    QPushButton *button = new QPushButton();
    button->setObjectName(QStringLiteral("buttonStereoInversion"));
    button->setToolTip(tr("Invert stereo"));
    button->setCheckable(true);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(setStereoInversion(bool)));
    return button;
}

void LocalTrackView::setStereoInversion(bool stereoInverted)
{
    mainController->setTrackStereoInversion(getInputIndex(), stereoInverted);
    buttonStereoInversion->setChecked(stereoInverted);
}

void LocalTrackView::refreshStyleSheet()
{
    BaseTrackView::refreshStyleSheet();

    style()->unpolish(buttonStereoInversion); // this is necessary to change the stereo inversion button colors when the transmit button is clicled
    style()->polish(buttonStereoInversion);
}
