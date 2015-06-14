#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include "../MainController.h"
#include <QStyleOption>
#include <QPainter>

QMap<long, BaseTrackView*> BaseTrackView::trackViews;//static map

BaseTrackView::BaseTrackView(QWidget *parent, Controller::MainController *mainController, long trackID) :
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

    //add in static map
    trackViews.insert(trackID, this);
}

BaseTrackView* BaseTrackView::getTrackViewByID(long trackID){
    if(trackViews.contains(trackID)){
        return trackViews[trackID];
    }
    return nullptr;
}

void BaseTrackView::setPeaks(float left, float right){
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

}

//little to allow stylesheet in custom widget
void BaseTrackView::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
