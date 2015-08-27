#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include "../MainController.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

QMap<long, BaseTrackView*> BaseTrackView::trackViews;//static map

BaseTrackView::BaseTrackView(Controller::MainController *mainController, long trackID) :
    ui(new Ui::BaseTrackView),
    mainController(mainController),
    trackID(trackID),
    activated(true),
    narrowed(false)
{
    ui->setupUi(this);
    QObject::connect(ui->muteButton, SIGNAL(clicked()), this, SLOT(onMuteClicked()));
    QObject::connect(ui->soloButton, SIGNAL(clicked()), this, SLOT(onSoloClicked()));
    QObject::connect(ui->levelSlider, SIGNAL(valueChanged(int)), this, SLOT(onFaderMoved(int)));
    QObject::connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SLOT(onPanSliderMoved(int)));

    ui->panSlider->installEventFilter(this);
    ui->levelSlider->installEventFilter(this);

    //add in static map
    trackViews.insert(trackID, this);
}

void BaseTrackView::setFaderOnlyMode(bool faderOnly){
    if(this->faderOnly != faderOnly){
        this->faderOnly = faderOnly;
        //ui->faderPanel->setVisible(!this->faderOnly);
        ui->topPanel->setVisible(!this->faderOnly);
        ui->levelSlider->setVisible(!this->faderOnly);
        ui->panSpacer->changeSize(20, 20, QSizePolicy::Minimum, this->faderOnly ? QSizePolicy::Ignored : QSizePolicy::Fixed);
        ui->panSpacer->invalidate();

        QMargins margins = layout()->contentsMargins();
        margins.setLeft(faderOnly ? 2 : 6);
        margins.setRight(faderOnly ? 2 : 6);
        layout()->setContentsMargins(margins);

        updateGeometry();

        setProperty("faderOnly", QVariant(faderOnly));
        style()->unpolish(this);
        style()->polish(this);

    }
}

void BaseTrackView::toggleFaderOnlyMode(){
    setFaderOnlyMode(!faderOnly);
}


void BaseTrackView::updatePeaks(){
    Audio::AudioPeak peak = mainController->getTrackPeak(getTrackID());
    setPeaks(peak.getLeft(), peak.getRight());
}

QSize BaseTrackView::sizeHint() const{
    if(faderOnly){
        int width = ui->faderPanel->sizeHint().width() + layout()->contentsMargins().left() + layout()->contentsMargins().right();
        return QSize( width, height());
    }
    if(narrowed){
        return QSize(NARROW_WIDTH, height());
    }

    return QSize(WIDE_WIDTH, height());
}

QSize BaseTrackView::minimumSizeHint() const{
    return sizeHint();
}

void BaseTrackView::setToNarrow(){
    //this->setMaximumWidth(NARROW_WIDTH);
    this->narrowed = true;

    ui->soloButton->setText("S");
    ui->soloButton->setToolTip("Solo");

    ui->muteButton->setText("M");
    ui->muteButton->setToolTip("Mute");

    ui->panSlider->setTickInterval(2);
    updateGeometry();
}

void BaseTrackView::setToWide(){
    if(narrowed){
        //setMaximumWidth(QWIDGETSIZE_MAX);
        this->narrowed = false;
        ui->soloButton->setText("SOLO");
        ui->soloButton->setToolTip("Solo");

        ui->muteButton->setText("MUTE");
        ui->muteButton->setToolTip("Mute");

        ui->panSlider->setTickInterval(0);
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
        qWarning() << "picos menores que zero left:" << left << " right:" << right;
    }
    ui->peakMeterLeft->setPeak(left);
    ui->peakMeterRight->setPeak(right);
}

//event filter used to handle double clicks
bool BaseTrackView::eventFilter(QObject *source, QEvent *ev){
    if(ev->type() == QEvent::MouseButtonDblClick){
        if(source == ui->panSlider){

            ui->panSlider->setValue(0);//center
        }
        if(source == ui->levelSlider){
            ui->levelSlider->setValue(100);
        }

        return true;
    }
    return QWidget::eventFilter(source, ev);
}

BaseTrackView::~BaseTrackView()
{
    delete ui;
    trackViews.remove(this->getTrackID());//remove from static map
}

void BaseTrackView::onPanSliderMoved(int value){
    float sliderValue = value/(float)ui->panSlider->maximum();
    mainController->setTrackPan(this->trackID, sliderValue);
}

void BaseTrackView::onFaderMoved(int value){
    mainController->setTrackLevel(this->trackID, value/100.0);
}

void BaseTrackView::onMuteClicked(){
    mainController->setTrackMute(this->trackID, !mainController->trackIsMuted(trackID));
}

void BaseTrackView::onSoloClicked(){
    mainController->setTrackSolo(this->trackID, !mainController->trackIsSoloed(this->trackID));
}

//little to allow stylesheet in custom widget
void BaseTrackView::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
