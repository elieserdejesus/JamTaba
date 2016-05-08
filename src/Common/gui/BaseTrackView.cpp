#include "BaseTrackView.h"
#include "MainController.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include "Utils.h"
#include "PeakMeter.h"
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

    QObject::connect(muteButton, SIGNAL(clicked()), this, SLOT(toggleMuteStatus()));
    QObject::connect(soloButton, SIGNAL(clicked()), this, SLOT(toggleSoloStatus()));
    QObject::connect(levelSlider, SIGNAL(valueChanged(int)), this, SLOT(setGain(int)));
    QObject::connect(panSlider, SIGNAL(valueChanged(int)), this, SLOT(setPan(int)));
    QObject::connect(buttonBoostZero, SIGNAL(clicked(bool)), this,
                     SLOT(updateBoostValue()));
    QObject::connect(buttonBoostMinus12, SIGNAL(clicked(bool)), this,
                     SLOT(updateBoostValue()));
    QObject::connect(buttonBoostPlus12, SIGNAL(clicked(bool)), this, SLOT(
                         updateBoostValue()));

    panSlider->installEventFilter(this);
    levelSlider->installEventFilter(this);
    peaksDbLabel->installEventFilter(this);

    // add in static map
    trackViews.insert(trackID, this);

    QButtonGroup *boostButtonGroup = new QButtonGroup(this);
    boostButtonGroup->addButton(buttonBoostMinus12);
    boostButtonGroup->addButton(buttonBoostZero);
    boostButtonGroup->addButton(buttonBoostPlus12);

    buttonBoostZero->setChecked(true);

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
    boostWidgetsLayout->setDirection(QBoxLayout::TopToBottom);

    mainLayout->setVerticalSpacing(12);
    mainLayout->setContentsMargins(3, 12, 3, 3);
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

    panSlider = new QSlider();
    panSlider->setObjectName(QStringLiteral("panSlider"));
    panSlider->setMinimum(-4);
    panSlider->setMaximum(4);
    panSlider->setOrientation(Qt::Horizontal);

    panWidgetsLayout->addWidget(labelPanL);
    panWidgetsLayout->addWidget(panSlider);
    panWidgetsLayout->addWidget(labelPanR);

    levelSlider = new QSlider();
    levelSlider->setObjectName(QStringLiteral("levelSlider"));
    levelSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    levelSlider->setMaximum(120);
    levelSlider->setValue(100);
    levelSlider->setTickPosition(QSlider::NoTicks);

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

    peakMeterLeft = new PeakMeter();
    peakMeterLeft->setObjectName(QStringLiteral("peakMeterLeft"));
    peakMeterRight = new PeakMeter();
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
    muteSoloLayout->addWidget(soloButton);
    muteSoloLayout->addWidget(muteButton);

    boostWidgetsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    boostWidgetsLayout->setSpacing(0);
    boostWidgetsLayout->setContentsMargins(0, 0, 0, 0);

    buttonBoostPlus12 = new QPushButton();
    buttonBoostPlus12->setObjectName(QStringLiteral("buttonBoostPlus12"));
    buttonBoostPlus12->setCheckable(true);
    buttonBoostPlus12->setText("+12");

    buttonBoostZero = new QPushButton();
    buttonBoostZero->setObjectName(QStringLiteral("buttonBoostZero"));
    buttonBoostZero->setCheckable(true);
    buttonBoostZero->setText("0");

    buttonBoostMinus12 = new QPushButton();
    buttonBoostMinus12->setObjectName(QStringLiteral("buttonBoostMinus12"));
    buttonBoostMinus12->setCheckable(true);
    buttonBoostMinus12->setText("-12");

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(buttonBoostMinus12);
    buttonGroup->addButton(buttonBoostPlus12);
    buttonGroup->addButton(buttonBoostZero);

    boostWidgetsLayout->addWidget(buttonBoostMinus12);
    boostWidgetsLayout->addWidget(buttonBoostZero);    
    boostWidgetsLayout->addWidget(buttonBoostPlus12);



    primaryChildsLayout = new QVBoxLayout();
    primaryChildsLayout->setSpacing(12);
    primaryChildsLayout->setContentsMargins(0, 0, 0, 0);

    secondaryChildsLayout = new QVBoxLayout();
    secondaryChildsLayout->setSpacing(12);
    secondaryChildsLayout->setContentsMargins(0, 0, 0, 0);

    primaryChildsLayout->addLayout(panWidgetsLayout);
    primaryChildsLayout->addLayout(levelSliderLayout, 1);

    secondaryChildsLayout->addLayout(meterWidgetsLayout);
    secondaryChildsLayout->addLayout(boostWidgetsLayout);
    secondaryChildsLayout->addLayout(muteSoloLayout);

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
}

void BaseTrackView::bindThisViewWithTrackNodeSignals()
{
    Audio::AudioNode *trackNode = mainController->getTrackNode(trackID);
    Q_ASSERT(trackNode);
    QObject::connect(trackNode, SIGNAL(gainChanged(float)), this, SLOT(setGainSliderPosition(
                                                                           float)));
    QObject::connect(trackNode, SIGNAL(panChanged(float)), this,
                     SLOT(setPanKnobPosition(float)));
    QObject::connect(trackNode, SIGNAL(muteChanged(bool)), this,
                     SLOT(setMuteStatus(bool)));
    QObject::connect(trackNode, SIGNAL(soloChanged(bool)), this,
                     SLOT(setSoloStatus(bool)));
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
    if (buttonBoostMinus12->isChecked())
        boostValue = -12;
    else if (buttonBoostPlus12->isChecked())
        boostValue = 12;
    if (mainController)
        mainController->setTrackBoost(getTrackID(), boostValue);
}

void BaseTrackView::updateGuiElements()
{
    if (!mainController)
        return;

    Audio::AudioPeak peak = mainController->getTrackPeak(getTrackID());
    if (peak.getMax() > maxPeak.getMax()) {
        maxPeak.update(peak);
        double db = Utils::linearToDb(maxPeak.getMax());
        peaksDbLabel->setText(QString::number(db, 'f', 0));
    }
    // update the track peaks
    setPeaks(peak.getLeft(), peak.getRight());

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
        // mainLayout->setContentsMargins(3, 3, 3, 3);
        updateGeometry();
    }
}

void BaseTrackView::setToWide()
{
    if (narrowed) {
        this->narrowed = false;
        // mainLayout->setContentsMargins(3, 3, 3, 3);
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

    style()->unpolish(buttonBoostMinus12);
    style()->polish(buttonBoostMinus12);

    style()->unpolish(buttonBoostPlus12);
    style()->polish(buttonBoostPlus12);

    style()->unpolish(buttonBoostZero);
    style()->polish(buttonBoostZero);

    update();
}

void BaseTrackView::setUnlightStatus(bool unlighted)
{
    setProperty("unlighted", QVariant(unlighted));
    refreshStyleSheet();
}

BaseTrackView *BaseTrackView::getTrackViewByID(long trackID)
{
    if (trackViews.contains(trackID))
        return trackViews[trackID];
    return nullptr;
}

void BaseTrackView::setPeaks(float left, float right)
{
    if (left < 0 || right < 0)
        qWarning() << "Invalid peak values left:" << left << " right:" << right;
    peakMeterLeft->setPeak(left);
    peakMeterRight->setPeak(right);
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

void BaseTrackView::setLayoutWidgetsVisibility(QLayout *layout, bool visible)
{
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);
        if (item->layout()) {
            setLayoutWidgetsVisibility(item->layout(), visible);
        } else {
            QWidget *widget = item->widget();
            if (widget)
                widget->setVisible(visible);
        }
    }
}

void BaseTrackView::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(e);
}
