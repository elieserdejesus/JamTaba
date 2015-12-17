#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include "MainController.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include "Utils.h"

const QColor BaseTrackView::DB_TEXT_COLOR = QColor(0, 0, 0, 120);
const int BaseTrackView::FADER_HEIGHT = 12;

const int BaseTrackView::NARROW_WIDTH = 80;
const int BaseTrackView::WIDE_WIDTH = 120;

QMap<long, BaseTrackView*> BaseTrackView::trackViews;//static map to quick lookup the views

BaseTrackView::BaseTrackView(Controller::MainController *mainController, long trackID) :
    ui(new Ui::BaseTrackView),
    mainController(mainController),
    trackID(trackID),
    activated(true),
    narrowed(false),
    drawDbValue(true)
{
    ui->setupUi(this);
    QObject::connect(ui->muteButton, SIGNAL(clicked()), this, SLOT(onMuteClicked()));
    QObject::connect(ui->soloButton, SIGNAL(clicked()), this, SLOT(onSoloClicked()));
    QObject::connect(ui->levelSlider, SIGNAL(valueChanged(int)), this, SLOT(onFaderMoved(int)));
    QObject::connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SLOT(onPanSliderMoved(int)));
    QObject::connect(ui->buttonBoostZero, SIGNAL(clicked(bool)), this, SLOT(onBoostButtonClicked()));
    QObject::connect(ui->buttonBoostMinus12, SIGNAL(clicked(bool)), this, SLOT(onBoostButtonClicked()));
    QObject::connect(ui->buttonBoostPlus12, SIGNAL(clicked(bool)), this, SLOT(onBoostButtonClicked()));

    ui->panSlider->installEventFilter(this);
    ui->levelSlider->installEventFilter(this);
    ui->peaksDbLabel->installEventFilter(this);

    //add in static map
    trackViews.insert(trackID, this);

    QButtonGroup* boostButtonGroup = new QButtonGroup(this);
    boostButtonGroup->addButton(ui->buttonBoostMinus12);
    boostButtonGroup->addButton(ui->buttonBoostZero);
    boostButtonGroup->addButton(ui->buttonBoostPlus12);

    ui->buttonBoostZero->setChecked(true);

    setAttribute(Qt::WA_NoBackground);
}

void BaseTrackView::bindThisViewWithTrackNodeSignals(){
    Audio::AudioNode* trackNode = mainController->getTrackNode(trackID);
    Q_ASSERT(trackNode);
    QObject::connect(trackNode, SIGNAL(gainChanged(float)), this, SLOT(onAudioNodeGainChanged(float)));
    QObject::connect(trackNode, SIGNAL(panChanged(float)), this, SLOT(onAudioNodePanChanged(float)));
    QObject::connect(trackNode, SIGNAL(muteChanged(bool)), this, SLOT(onAudioNodeMuteChanged(bool)));
    QObject::connect(trackNode, SIGNAL(soloChanged(bool)), this, SLOT(onAudioNodeSoloChanged(bool)));
}

//++++++  signals emitted by Audio Node +++++++
/*
    The values are changed in the model, so the view (this class) need
react and update. This changes in the model can done in initialization (when
Jamtaba is opened and the last gain, pan values, etc. are loaded) or by
a midi message for example. So we can't expect the gain and pan values are
changed only by user mouse interaction, these values can be changed using another
methods (like midi messages).

*/

void BaseTrackView::onAudioNodeGainChanged(float newGainValue){
    ui->levelSlider->setValue(newGainValue * 100);
}

void BaseTrackView::onAudioNodePanChanged(float newPanValue){
    //pan range is[-4,4], zero is center
    ui->panSlider->setValue(newPanValue * 4);
}

void BaseTrackView::onAudioNodeMuteChanged(bool newMuteStatus){
    ui->muteButton->setChecked(newMuteStatus);
}

void BaseTrackView::onAudioNodeSoloChanged(bool newSoloStatus){
    ui->soloButton->setChecked(newSoloStatus);
}
//+++++++++

void BaseTrackView::onBoostButtonClicked(){
    float boostValue = 0;
    if(ui->buttonBoostMinus12->isChecked()){
        boostValue = -12;
    }
    else if(ui->buttonBoostPlus12->isChecked()){
        boostValue = 12;
    }
    if(mainController){
        mainController->setTrackBoost(getTrackID(), boostValue);
    }
}

void BaseTrackView::updateGuiElements(){
    if(!mainController){
        return;
    }
    Audio::AudioPeak peak = mainController->getTrackPeak(getTrackID());
    if(peak.getMax() > maxPeak.getMax()){
        maxPeak.update(peak);
        double db = Utils::linearToDb(maxPeak.getMax());
        ui->peaksDbLabel->setText(QString::number(db, 'f', 0));
    }
    //update the track peaks
    setPeaks( peak.getLeft(), peak.getRight());

    //update the track processors. In this moment the VST plugins GUI are updated. Some plugins need this to run your animations (see Ez Drummer, for example);
    mainController->getTrackNode(getTrackID())->updateProcessorsGui();//call idle in VST plugins
}

QSize BaseTrackView::sizeHint() const{
    if(narrowed){
        return QSize(NARROW_WIDTH, height());
    }
    return QSize(WIDE_WIDTH, height());
}

QSize BaseTrackView::minimumSizeHint() const{
    return sizeHint();
}

void BaseTrackView::setToNarrow(){
    if(!this->narrowed){
        this->narrowed = true;
        ui->mainLayout->setContentsMargins(3, 3, 3, 3);
        updateGeometry();
    }
}

void BaseTrackView::setToWide(){
    if(narrowed){
        this->narrowed = false;
        ui->mainLayout->setContentsMargins(3, 3, 3, 3);
        updateGeometry();
    }
}

void BaseTrackView::setUnlightStatus(bool unlighted){
    setProperty("unlighted", QVariant(unlighted));

    style()->unpolish(this);
    style()->polish(this);

    style()->unpolish(ui->levelSlider);
    style()->polish(ui->levelSlider);

    style()->unpolish(ui->panSlider);
    style()->polish(ui->panSlider);

    style()->unpolish(ui->peakMeterLeft);
    style()->polish(ui->peakMeterLeft);

    style()->unpolish(ui->peakMeterRight);
    style()->polish(ui->peakMeterRight);

    style()->unpolish(ui->peaksDbLabel);
    style()->polish(ui->peaksDbLabel);

    style()->unpolish(ui->muteButton);
    style()->polish(ui->muteButton);

    style()->unpolish(ui->soloButton);
    style()->polish(ui->soloButton);

    style()->unpolish(ui->buttonBoostMinus12);
    style()->polish(ui->buttonBoostMinus12);

    style()->unpolish(ui->buttonBoostPlus12);
    style()->polish(ui->buttonBoostPlus12);

    style()->unpolish(ui->buttonBoostZero);
    style()->polish(ui->buttonBoostZero);

    update();
}

BaseTrackView* BaseTrackView::getTrackViewByID(long trackID){
    if(trackViews.contains(trackID)){
        return trackViews[trackID];
    }
    return nullptr;
}

void BaseTrackView::setPeaks(float left, float right){
    if(left < 0 || right < 0){
        qWarning() << "Invalid peak values left:" << left << " right:" << right;
    }
    ui->peakMeterLeft->setPeak(left);
    ui->peakMeterRight->setPeak(right);
}

//event filter used to handle double clicks
bool BaseTrackView::eventFilter(QObject *source, QEvent *ev){
    if(source == ui->peaksDbLabel && ev->type() == QEvent::MouseButtonRelease){
        maxPeak.zero();
        ui->peaksDbLabel->setText("");
        return true;
    }
    //--------------
    if(ev->type() == QEvent::MouseButtonDblClick){
        if(source == ui->levelSlider){
            ui->levelSlider->setValue(100);
            update();
        }

        return true;
    }
    if(ev->type() == QEvent::MouseMove && source == ui->levelSlider){
        update();
    }
    return QWidget::eventFilter(source, ev);
}

BaseTrackView::~BaseTrackView(){
    delete ui;
    trackViews.remove(this->getTrackID());//remove from static map
}

void BaseTrackView::onPanSliderMoved(int value){
    float sliderValue = value/(float)ui->panSlider->maximum();
    mainController->setTrackPan(this->trackID, sliderValue, true);
}

void BaseTrackView::onFaderMoved(int value){
    //signals are blocked [the third parameter] to avoid a loop in signal/slot scheme.
    mainController->setTrackGain(this->trackID, value/100.0, true);
}

void BaseTrackView::onMuteClicked(){
    mainController->setTrackMute(this->trackID, !mainController->trackIsMuted(trackID), true);
}

void BaseTrackView::onSoloClicked(){
    mainController->setTrackSolo(this->trackID, !mainController->trackIsSoloed(this->trackID), true);
}

//little to allow stylesheet in custom widget
void BaseTrackView::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if(drawDbValue){
        drawFaderDbValue(p);
    }
}

void BaseTrackView::drawFaderDbValue(QPainter &p){
    p.setPen(DB_TEXT_COLOR);

    double poweredGain = Utils::linearGainToPower(ui->levelSlider->value()/100.0);
    double faderDb = Utils::linearToDb(poweredGain);

    QString text = QString::number(faderDb, 'f', 1);
    int textWidth = p.fontMetrics().width(text);

    int textX = ui->mainWidget->x() + ui->levelSlider->x() + ui->levelSlider->width()/2 - textWidth  - ((!narrowed) ? 14 : 11);
    float sliderPosition = (double)ui->levelSlider->value()/ui->levelSlider->maximum();
    int offset = ui->mainWidget->y() + ui->levelSlider->y() + FADER_HEIGHT + p.fontMetrics().height();
    int textY =  (1 - sliderPosition) * ui->levelSlider->height() + offset;

    p.drawText(textX, textY, text);
}
