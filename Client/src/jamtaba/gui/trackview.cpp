#include "TrackView.h"
#include "ui_trackview.h"
#include "../MainController.h"
#include <QStyleOption>
#include <QPainter>

TrackView::TrackView(QWidget *parent, Controller::MainController *mainController, int trackID) :
    QWidget(parent),
    ui(new Ui::TrackView),
    mainController(mainController),
    trackID(trackID)
{
    ui->setupUi(this);
    QObject::connect(ui->muteButton, SIGNAL(clicked()), this, SLOT(onMuteClicked()));
    QObject::connect(ui->soloButton, SIGNAL(clicked()), this, SLOT(onSoloClicked()));
    QObject::connect(ui->levelSlider, SIGNAL(valueChanged(int)), this, SLOT(onFaderMoved(int)));
    QObject::connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SLOT(onPanSliderMoved(int)));

    ui->panSlider->installEventFilter(this);
    ui->levelSlider->installEventFilter(this);
    ui->transmitGainSlider->installEventFilter(this);
}

void TrackView::setPeaks(float left, float right){
    ui->peakMeterLeft->setPeak(left);
    ui->peakMeterRight->setPeak(right);
}

//event filter used to handle double clicks
bool TrackView::eventFilter(QObject *source, QEvent *ev){
    if(ev->type() == QEvent::MouseButtonDblClick){
        if(source == ui->panSlider){

            ui->panSlider->setValue(0);//center
        }
        if(source == ui->levelSlider){
            ui->levelSlider->setValue(100);
        }
        if(source == ui->transmitGainSlider){
            ui->transmitGainSlider->setValue(100);
        }
    }
    else{
        return QWidget::eventFilter(source, ev);
    }
}

TrackView::~TrackView()
{
    delete ui;
}

void TrackView::onPanSliderMoved(int value){
    float sliderValue = value/(float)ui->panSlider->maximum();
    mainController->setTrackPan(this->trackID, sliderValue);
}

void TrackView::onFaderMoved(int value){
    mainController->setTrackLevel(this->trackID, value/100.0);
}

void TrackView::onMuteClicked(){
    mainController->setTrackMute(this->trackID, !mainController->trackIsMuted(trackID));
}

void TrackView::onSoloClicked(){

}

//little to allow stylesheet in custom widget
void TrackView::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
