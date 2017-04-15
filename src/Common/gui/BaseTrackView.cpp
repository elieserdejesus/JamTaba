#include "BaseTrackView.h"
#include "MainController.h"
#include "Utils.h"
#include "PeakMeter.h"

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

const QColor BaseTrackView::DB_TEXT_COLOR = QColor(0, 0, 0, 120);
const int BaseTrackView::FADER_HEIGHT = 12;

const int BaseTrackView::NARROW_WIDTH = 85;
const int BaseTrackView::WIDE_WIDTH = 120;

QMap<long, BaseTrackView *> BaseTrackView::trackViews;// static map to quick lookup the views

BaseTrackView::BaseTrackView(Controller::MainController *mainController, long trackID) :
    mainController(mainController),
    trackID(trackID),
    activated(true),
    narrowed(false),
    drawDbValue(true)
{
    createLayoutStructure();
    setupVerticalLayout();

    connect(muteButton, &QPushButton::clicked, this, &BaseTrackView::toggleMuteStatus);
    connect(soloButton, &QPushButton::clicked, this, &BaseTrackView::toggleSoloStatus);
    connect(levelSlider, &QSlider::valueChanged, this, &BaseTrackView::setGain);
    connect(panSlider, &QSlider::valueChanged, this, &BaseTrackView::setPan);
    connect(buttonBoost, &MultiStateButton::stateChanged, this, &BaseTrackView::updateBoostValue);

    panSlider->installEventFilter(this);
    levelSlider->installEventFilter(this);
    peaksDbLabel->installEventFilter(this);

    // add in static map
    BaseTrackView::trackViews.insert(trackID, this);

    setAttribute(Qt::WA_NoBackground);
}

void BaseTrackView::setupVerticalLayout()
{
    setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

    levelSlider->setOrientation(Qt::Vertical);
    levelSliderLayout->setDirection(QBoxLayout::TopToBottom);

    peakMeterRight->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
    peakMeterLeft->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
    peakMeterLeft->setOrientation(Qt::Vertical);
    peakMeterRight->setOrientation(Qt::Vertical);
    metersLayout->setDirection(QHBoxLayout::LeftToRight);
    meterWidgetsLayout->setDirection(QHBoxLayout::TopToBottom);

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
    QLabel *highLevelIcon = new QLabel();
    QLabel *lowLevelIcon = new QLabel();
    highLevelIcon->setPixmap(QPixmap(":/images/level high.png"));
    lowLevelIcon->setPixmap(QPixmap(":/images/level low.png"));
    highLevelIcon->setAlignment(Qt::AlignCenter);
    lowLevelIcon->setAlignment(Qt::AlignCenter);

    levelSliderLayout->addWidget(highLevelIcon);
    levelSliderLayout->addWidget(levelSlider);
    levelSliderLayout->addWidget(lowLevelIcon);

    peakMeterLeft = new AudioMeter(this);
    peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
    peakMeterRight = new AudioMeter(this);
    peakMeterRight->setObjectName(QStringLiteral("peakMeterRight"));

    metersLayout = new QHBoxLayout();
    metersLayout->setSpacing(1);
    metersLayout->setContentsMargins(0, 0, 0, 0);
    metersLayout->addWidget(peakMeterLeft);
    metersLayout->addWidget(peakMeterRight);

    peaksDbLabel = new QLabel();
    peaksDbLabel->setObjectName(QStringLiteral("peaksDbLabel"));
    peaksDbLabel->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));

    meterWidgetsLayout = new QVBoxLayout();
    meterWidgetsLayout->setSpacing(1);
    meterWidgetsLayout->setContentsMargins(0, 0, 0, 0);
    meterWidgetsLayout->addLayout(metersLayout, 1);
    meterWidgetsLayout->addWidget(peaksDbLabel);

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

    buttonBoost = new MultiStateButton(3, this); // 3 states: OFF, -12 db and +12 db
    buttonBoost->setObjectName(QStringLiteral("buttonBoost"));
    buttonBoost->setCheckable(true);
    buttonBoost->setText("OFF", 0);
    buttonBoost->setText("-12", 1);
    buttonBoost->setText("+12", 2);

    primaryChildsLayout = new QVBoxLayout();
    primaryChildsLayout->setSpacing(12);
    primaryChildsLayout->setContentsMargins(0, 0, 0, 0);

    secondaryChildsLayout = new QVBoxLayout();
    secondaryChildsLayout->setSpacing(6);
    secondaryChildsLayout->setContentsMargins(0, 0, 0, 0);

    primaryChildsLayout->addLayout(panWidgetsLayout);
    primaryChildsLayout->addLayout(levelSliderLayout, 1);

    secondaryChildsLayout->addLayout(meterWidgetsLayout);
    secondaryChildsLayout->addLayout(muteSoloLayout);
    secondaryChildsLayout->addWidget(buttonBoost);

    mainLayout->addLayout(primaryChildsLayout, 0, 0);
    mainLayout->addLayout(secondaryChildsLayout, 0, 1);

    updateBoostButtonToolTip();

    translateUI();
}

void BaseTrackView::updateBoostButtonToolTip()
{
    QString boostText = " OFF [0 dB]";
    if (buttonBoost->getCurrentState() > 0) {
        boostText = " (" + buttonBoost->text() + " dB)";
    }

    QString toolTipText = tr("Boost") + boostText;
    buttonBoost->setToolTip(toolTipText);
}

void BaseTrackView::translateUI()
{
    labelPanL->setText(tr("L"));
    labelPanR->setText(tr("R"));

    muteButton->setText(tr("M"));
    soloButton->setText(tr("S"));

    updateBoostButtonToolTip();
}

void BaseTrackView::bindThisViewWithTrackNodeSignals()
{
    Audio::AudioNode *trackNode = mainController->getTrackNode(trackID);
    Q_ASSERT(trackNode);
    connect(trackNode, &Audio::AudioNode::gainChanged, this, &BaseTrackView::setGainSliderPosition);
    connect(trackNode, &Audio::AudioNode::panChanged, this, &BaseTrackView::setPanKnobPosition);
    connect(trackNode, &Audio::AudioNode::muteChanged, this, &BaseTrackView::setMuteStatus);
    connect(trackNode, &Audio::AudioNode::soloChanged, this, &BaseTrackView::setSoloStatus);
    connect(trackNode, &Audio::AudioNode::boostChanged, this, &BaseTrackView::setBoostStatus);
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
        buttonBoost->setState(2); // +12 dB
    else if (newBoostValue < 1.0)
        buttonBoost->setState(1); // -12 dB
    else
        buttonBoost->setState(0); // 0 dB - OFF
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

// +++++++++

void BaseTrackView::updateBoostValue()
{
    float boostValue = 0;
    if (buttonBoost->getCurrentState() == 1)
        boostValue = -12;
    else if (buttonBoost->getCurrentState() == 2)
        boostValue = 12;
    if (mainController)
        mainController->setTrackBoost(getTrackID(), boostValue);

    updateBoostButtonToolTip();
}

void BaseTrackView::updateGuiElements()
{
    if (!mainController)
        return;

    Audio::AudioPeak peak = mainController->getTrackPeak(getTrackID());
    if (peak.getMaxPeak() > maxPeak.getMaxPeak()) {
        maxPeak.update(peak);
        double db = Utils::linearToDb(maxPeak.getMaxPeak());
        peaksDbLabel->setText(QString::number(db, 'f', 0));
    }
    // update the track peaks
    setPeaks(peak.getLeftPeak(), peak.getRightPeak(), peak.getLeftRMS(), peak.getRightRMS());

    // update the track processors. In this moment the VST plugins GUI are updated. Some plugins need this to run your animations (see Ez Drummer, for example);
    Audio::AudioNode *trackNode = mainController->getTrackNode(getTrackID());
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

void BaseTrackView::refreshStyleSheet()
{
    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(levelSlider);
    style()->polish(levelSlider);

    style()->unpolish(panSlider);
    style()->polish(panSlider);

    style()->unpolish(peakMeterLeft);
    style()->polish(peakMeterLeft);

    style()->unpolish(peakMeterRight);
    style()->polish(peakMeterRight);

    style()->unpolish(peaksDbLabel);
    style()->polish(peaksDbLabel);

    style()->unpolish(muteButton);
    style()->polish(muteButton);

    style()->unpolish(soloButton);
    style()->polish(soloButton);

    style()->unpolish(buttonBoost);
    style()->polish(buttonBoost);

    update();
}

void BaseTrackView::setActivatedStatus(bool deactivated)
{
    setProperty("unlighted", QVariant(deactivated));
    this->activated = !deactivated;
    refreshStyleSheet();
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
    peakMeterLeft->setPeak(peakLeft, rmsLeft);
    peakMeterRight->setPeak(peakRight, rmsRight);

}

// event filter used to handle double clicks
bool BaseTrackView::eventFilter(QObject *source, QEvent *ev)
{
    if (source == peaksDbLabel && ev->type() == QEvent::MouseButtonRelease) {
        maxPeak.zero();
        peaksDbLabel->setText("");
        return true;
    }
    // --------------
    if (ev->type() == QEvent::MouseButtonDblClick) {
        if (source == levelSlider || source == panSlider) {
            if (source == levelSlider)
                levelSlider->setValue(100); // set level fader to unit gain
            else
                panSlider->setValue(0);// set pan slider to center
            update();
        }

        return true;
    }
    if (ev->type() == QEvent::MouseMove && source == levelSlider)
        update();
    return QWidget::eventFilter(source, ev);
}

BaseTrackView::~BaseTrackView()
{
    // qDeleteAll(children());// delete ui;
    trackViews.remove(this->getTrackID());// remove from static map
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

    if (drawDbValue)
        drawFaderDbValue(p);
}

void BaseTrackView::drawFaderDbValue(QPainter &p)
{
    p.setPen(DB_TEXT_COLOR);

    double poweredGain = Utils::linearGainToPower(levelSlider->value()/100.0);
    double faderDb = Utils::linearToDb(poweredGain);
    int precision = narrowed ? 0 : 1;
    QString text = QString::number(faderDb, 'f', precision);

    QPoint textPosition = getDbValuePosition(text, p.fontMetrics());
    p.drawText(textPosition.x(), textPosition.y(), text);
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
