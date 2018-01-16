#include "BaseTrackView.h"
#include "MainController.h"
#include "Utils.h"
#include "PeakMeter.h"
#include "audio/core/AudioPeak.h"
#include "BoostSpinBox.h"
#include "IconFactory.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QEvent>

const int BaseTrackView::FADER_HEIGHT = 12;

const int BaseTrackView::NARROW_WIDTH = 85;
const int BaseTrackView::WIDE_WIDTH = 120;

QMap<long, BaseTrackView *> BaseTrackView::trackViews; // static map to quick lookup the views

// -----------------------------------------------------------------------------------------

BaseTrackView::BaseTrackView(controller::MainController *mainController, long trackID) :
    mainController(mainController),
    trackID(trackID),
    activated(true),
    narrowed(false),
    tintColor(Qt::black)
{
    createLayoutStructure();
    setupVerticalLayout();

    connect(muteButton, &QPushButton::clicked, this, &BaseTrackView::toggleMuteStatus);
    connect(soloButton, &QPushButton::clicked, this, &BaseTrackView::toggleSoloStatus);
    connect(levelSlider, &QSlider::valueChanged, this, &BaseTrackView::setGain);
    connect(panSlider, &QSlider::valueChanged, this, &BaseTrackView::setPan);
    connect(boostSpinBox, &BoostSpinBox::boostChanged, this, &BaseTrackView::updateBoostValue);

    // add in static map
    BaseTrackView::trackViews.insert(trackID, this);

    setAttribute(Qt::WA_NoBackground);
}

void BaseTrackView::setTintColor(const QColor &color)
{
    this->tintColor = color;

    lowLevelIcon->setPixmap(IconFactory::createLowLevelIcon(getTintColor()));
    highLevelIcon->setPixmap(IconFactory::createHighLevelIcon(getTintColor()));
}

void BaseTrackView::setupVerticalLayout()
{
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

    levelSlider->setOrientation(Qt::Vertical);
    levelSliderLayout->setDirection(QBoxLayout::TopToBottom);

    peakMeter->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
    peakMeter->setOrientation(Qt::Vertical);

    metersLayout->setDirection(QHBoxLayout::LeftToRight);

    muteSoloLayout->setDirection(QBoxLayout::TopToBottom);

    mainLayout->setVerticalSpacing(9);
    mainLayout->setContentsMargins(3, 0, 3, 3);
}

void BaseTrackView::createLayoutStructure()
{
    setObjectName(QStringLiteral("BaseTrackView"));

    mainLayout = new QGridLayout(this);

    panWidgetsLayout = new QHBoxLayout();
    panWidgetsLayout->setSpacing(0);
    panWidgetsLayout->setContentsMargins(0, 0, 0, 0);

    labelPanL = new QLabel();
    labelPanL->setObjectName(QStringLiteral("labelPanL"));
    labelPanL->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
    labelPanL->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    labelPanR = new QLabel();
    labelPanR->setObjectName(QStringLiteral("labelPanR"));
    labelPanR->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred));
    labelPanR->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    panSlider = new Slider();
    panSlider->setObjectName(QStringLiteral("panSlider"));
    panSlider->setMinimum(-4);
    panSlider->setMaximum(4);
    panSlider->setOrientation(Qt::Horizontal);
    panSlider->setSliderType(Slider::PanSlider);

    panWidgetsLayout->addWidget(labelPanL);
    panWidgetsLayout->addWidget(panSlider);
    panWidgetsLayout->addWidget(labelPanR);

    levelSlider = new Slider();
    levelSlider->setObjectName(QStringLiteral("levelSlider"));
    levelSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    levelSlider->setMaximum(120);
    levelSlider->setValue(100);
    levelSlider->setTickPosition(QSlider::NoTicks);
    levelSlider->setSliderType(Slider::AudioSlider);

    levelSliderLayout = new QVBoxLayout();
    levelSliderLayout->setSpacing(2);
    levelSliderLayout->setContentsMargins(0, 0, 0, 0);
    highLevelIcon = new QLabel(this);
    lowLevelIcon = new QLabel(this);
    highLevelIcon->setPixmap(IconFactory::createHighLevelIcon(getTintColor()));
    lowLevelIcon->setPixmap(IconFactory::createLowLevelIcon(getTintColor()));
    highLevelIcon->setAlignment(Qt::AlignCenter);
    lowLevelIcon->setAlignment(Qt::AlignCenter);

    levelSliderLayout->addWidget(highLevelIcon);
    levelSliderLayout->addWidget(levelSlider);
    levelSliderLayout->addWidget(lowLevelIcon);

    peakMeter = new AudioMeter(this);
    peakMeter->setObjectName(QStringLiteral("peakMeter"));

    metersLayout = new QHBoxLayout();
    metersLayout->setSpacing(1);
    metersLayout->setContentsMargins(0, 0, 0, 0);
    metersLayout->addWidget(peakMeter);

    muteButton = new QPushButton();
    muteButton->setObjectName(QStringLiteral("muteButton"));
    muteButton->setEnabled(true);
    muteButton->setLayoutDirection(Qt::LeftToRight);
    muteButton->setCheckable(true);

    soloButton = new QPushButton();
    soloButton->setObjectName(QStringLiteral("soloButton"));
    soloButton->setLayoutDirection(Qt::LeftToRight);
    soloButton->setCheckable(true);

    muteSoloLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    muteSoloLayout->setSpacing(1);
    muteSoloLayout->setContentsMargins(0, 0, 0, 0);
    muteSoloLayout->addWidget(muteButton);
    muteSoloLayout->addWidget(soloButton);

    boostSpinBox = new BoostSpinBox(this);

    primaryChildsLayout = new QVBoxLayout();
    primaryChildsLayout->setSpacing(12);
    primaryChildsLayout->setContentsMargins(0, 0, 0, 0);

    secondaryChildsLayout = new QVBoxLayout();
    secondaryChildsLayout->setSpacing(6);
    secondaryChildsLayout->setContentsMargins(0, 0, 0, 0);

    primaryChildsLayout->addLayout(panWidgetsLayout);
    primaryChildsLayout->addLayout(levelSliderLayout, 1);

    secondaryChildsLayout->addLayout(metersLayout);
    secondaryChildsLayout->addLayout(muteSoloLayout);
    secondaryChildsLayout->addWidget(boostSpinBox, 0, Qt::AlignCenter);

    mainLayout->addLayout(primaryChildsLayout, 0, 0);
    mainLayout->addLayout(secondaryChildsLayout, 0, 1);

    translateUI();
}

void BaseTrackView::translateUI()
{
    labelPanL->setText(tr("L"));
    labelPanR->setText(tr("R"));

    muteButton->setText(tr("M"));
    soloButton->setText(tr("S"));

    boostSpinBox->updateToolTip();
}

void BaseTrackView::bindThisViewWithTrackNodeSignals()
{
    auto trackNode = mainController->getTrackNode(trackID);
    Q_ASSERT(trackNode);
    connect(trackNode, &audio::AudioNode::gainChanged, this, &BaseTrackView::setGainSliderPosition);
    connect(trackNode, &audio::AudioNode::panChanged, this, &BaseTrackView::setPanKnobPosition);
    connect(trackNode, &audio::AudioNode::muteChanged, this, &BaseTrackView::setMuteStatus);
    connect(trackNode, &audio::AudioNode::soloChanged, this, &BaseTrackView::setSoloStatus);
    connect(trackNode, &audio::AudioNode::boostChanged, this, &BaseTrackView::setBoostStatus);
}

// ++++++  signals emitted by Audio Node +++++++
/*
    The values are changed in the model, so the view (this class) need
react and update. This changes in the model can done in initialization (when
Jamtaba is opened and the last gain, pan values, etc. are loaded) or by
a midi message for example. So we can't expect the gain and pan values are
changed only by user mouse interaction, these values can be changed using another
methods (like midi messages).

*/

void BaseTrackView::setBoostStatus(float newBoostValue)
{
    if (newBoostValue > 1.0) // boost value is a gain multiplier, 1.0 means 0 dB boost (boost OFF)
        boostSpinBox->setToMax();
    else if (newBoostValue < 1.0)
        boostSpinBox->setToMin();
    else
        boostSpinBox->setToOff(); // 0 dB - OFF
}

void BaseTrackView::setGainSliderPosition(float newGainValue)
{
    levelSlider->setValue(newGainValue * 100);
    update(); // repaint to update the Db value
}

void BaseTrackView::setPanKnobPosition(float newPanValue)
{
    // pan range is[-4,4], zero is center
    panSlider->setValue(newPanValue * 4);
}

void BaseTrackView::setMuteStatus(bool newMuteStatus)
{
    muteButton->setChecked(newMuteStatus);
}

void BaseTrackView::setSoloStatus(bool newSoloStatus)
{
    soloButton->setChecked(newSoloStatus);
}

void BaseTrackView::updateBoostValue(int boostValue)
{
    if (mainController)
        mainController->setTrackBoost(getTrackID(), boostValue);

}

void BaseTrackView::updateGuiElements()
{
    if (!mainController)
        return;

    auto peak = mainController->getTrackPeak(getTrackID());
    if (peak.getMaxPeak() > maxPeak.getMaxPeak()) {
        maxPeak.update(peak);
    }

    // update the track peaks
    setPeaks(peak.getLeftPeak(), peak.getRightPeak(), peak.getLeftRMS(), peak.getRightRMS());

    // update the track processors. In this moment the VST plugins GUI are updated. Some plugins need this to run your animations (see Ez Drummer, for example);
    auto trackNode = mainController->getTrackNode(getTrackID());
    if (trackNode)
        trackNode->updateProcessorsGui();  // call idle in VST plugins
}

QSize BaseTrackView::sizeHint() const
{
    if (narrowed)
        return QSize(NARROW_WIDTH, height());
    return QSize(WIDE_WIDTH, height());
}

QSize BaseTrackView::minimumSizeHint() const
{
    return sizeHint();
}

void BaseTrackView::setToNarrow()
{
    if (!this->narrowed) {
        this->narrowed = true;
        updateGeometry();
    }
}

void BaseTrackView::setToWide()
{
    if (narrowed) {
        this->narrowed = false;
        updateGeometry();
    }
}

void BaseTrackView::updateStyleSheet()
{
    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(levelSlider);
    style()->polish(levelSlider);

    style()->unpolish(panSlider);
    style()->polish(panSlider);

    peakMeter->updateStyleSheet();

    style()->unpolish(muteButton);
    style()->polish(muteButton);

    style()->unpolish(soloButton);
    style()->polish(soloButton);

    boostSpinBox->updateStyleSheet();

    update();
}

void BaseTrackView::setActivatedStatus(bool deactivated)
{
    setProperty("unlighted", QVariant(deactivated));
    this->activated = !deactivated;
    updateStyleSheet();
}

bool BaseTrackView::isActivated() const
{
    return activated;
}

BaseTrackView *BaseTrackView::getTrackViewByID(long trackID)
{
    if (trackViews.contains(trackID))
        return trackViews[trackID];
    return nullptr;
}

void BaseTrackView::setPeaks(float peakLeft, float peakRight, float rmsLeft, float rmsRight)
{
    peakMeter->setPeak(peakLeft, peakRight, rmsLeft, rmsRight);
}

BaseTrackView::~BaseTrackView()
{
    trackViews.remove(this->getTrackID()); // remove from static map
}

void BaseTrackView::setPan(int value)
{
    float sliderValue = value/(float)panSlider->maximum();
    mainController->setTrackPan(this->trackID, sliderValue, true);
}

void BaseTrackView::setGain(int value)
{
    // signals are blocked [the third parameter] to avoid a loop in signal/slot scheme.
    mainController->setTrackGain(this->trackID, value/100.0, true);
}

void BaseTrackView::toggleMuteStatus()
{
    mainController->setTrackMute(this->trackID, !mainController->trackIsMuted(trackID), true);
}

void BaseTrackView::toggleSoloStatus()
{
    mainController->setTrackSolo(this->trackID, !mainController->trackIsSoloed(this->trackID),
                                 true);
}

// little to allow stylesheet in custom widget
void BaseTrackView::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QPoint BaseTrackView::getDbValuePosition(const QString &dbValueText,
                                         const QFontMetrics &fontMetrics) const
{
    int textWidth = fontMetrics.width(dbValueText);
    int textX = levelSlider->x() + levelSlider->width()/2 - textWidth - ((!narrowed) ? 14 : 11);
    float sliderPosition = (double)levelSlider->value()/levelSlider->maximum();
    int offset = levelSlider->y() + fontMetrics.height();
    int textY = (1 - sliderPosition) * levelSlider->height() + offset;
    return QPoint(textX, textY);
}

void BaseTrackView::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(e);
}
