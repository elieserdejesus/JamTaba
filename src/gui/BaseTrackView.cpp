#include "BaseTrackView.h"
#include "ui_BaseTrackView.h"
#include "../MainController.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include "../Utils.h"

QMap<long, BaseTrackView*> BaseTrackView::trackViews;//static map

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
    QObject::connect(ui->panSlider, SIGNAL(sliderMoved(int)), this, SLOT(onPanSliderMoved(int)));
    QObject::connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SLOT(onPanSliderMoved(int)));


    ui->panSlider->installEventFilter(this);
    ui->levelSlider->installEventFilter(this);
    ui->peaksDbLabel->installEventFilter(this);

    //add in static map
    trackViews.insert(trackID, this);

    QButtonGroup* boostButtonGroup = new QButtonGroup(this);
    boostButtonGroup->addButton(ui->buttonBoostMinus12);
    boostButtonGroup->addButton(ui->buttonBoostZero);
    boostButtonGroup->addButton(ui->buttonBoostPlus12);
}



void BaseTrackView::updateGuiElements(){
    if(!mainController){
        return;
    }
    Audio::AudioPeak peak = mainController->getTrackPeak(getTrackID());
    if(peak.getMax() > maxPeak.getMax()){
        maxPeak.update(peak);
        double db = 20 * std::log10(maxPeak.getMax());
        ui->peaksDbLabel->setText(QString::number(db, 'f', 0));
//        setProperty("clip", QVariant(db >= 0));
//        style()->unpolish(this);
//        style()->polish(this);
    }
    setPeaks( peak.getLeft(), peak.getRight());
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
    //this->setMaximumWidth(NARROW_WIDTH);
    this->narrowed = true;

    //ui->soloButton->setText("S");
    //ui->soloButton->setToolTip("Solo");

    //ui->muteButton->setText("M");
    //ui->muteButton->setToolTip("Mute");

    //ui->panSlider->setTickInterval(2);
    updateGeometry();
}

void BaseTrackView::setToWide(){
    if(narrowed){
        //setMaximumWidth(QWIDGETSIZE_MAX);
        this->narrowed = false;
//        ui->soloButton->setText("SOLO");
//        ui->soloButton->setToolTip("Solo");

//        ui->muteButton->setText("MUTE");
//        ui->muteButton->setToolTip("Mute");

        //ui->panSlider->setTickInterval(0);
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
    if(source == ui->peaksDbLabel && ev->type() == QEvent::MouseButtonRelease){
        maxPeak.zero();
        ui->peaksDbLabel->setText("");
        return true;
    }
    //--------------

    //--------------
    if(ev->type() == QEvent::MouseButtonDblClick){
//        if(source == ui->panSlider){
//            ui->panSlider->setValue(0);//center
//        }
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


//    drawFaderDbMarks(p);
    if(drawDbValue){
        drawFaderDbValue(p);
    }

}

//void BaseTrackView::drawFaderDbMarks(QPainter &p){


//    double poweredGain = Utils::linearGainToPower(ui->levelSlider->value()/100.0);

//    QString text = QString::number(faderDb, 'f', 1);
//    int textWidth = p.fontMetrics().width(text);
//    int textX = ui->faderPanel->x() + ui->levelSlider->x() + ui->levelSlider->width()/2 - textWidth  - 14;
//    int textY =  (1 - ((double)ui->levelSlider->value()/ui->levelSlider->maximum())) * (ui->levelSlider->height() - FADER_ICON_HEIGHT) + ui->faderPanel->y() + ui->levelSlider->y();

//}

void BaseTrackView::drawFaderDbValue(QPainter &p){
    static QColor textColor(0,0,0, 70);
    static int FADER_ICON_HEIGHT = 48;

    p.setPen(textColor);
    double poweredGain = Utils::linearGainToPower(ui->levelSlider->value()/100.0);
    double faderDb = 20 * std::log10(poweredGain);
    QString text = QString::number(faderDb, 'f', 1);
    int textWidth = p.fontMetrics().width(text);

    int textX = ui->faderPanel->x() + ui->levelSlider->x() + ui->levelSlider->width()/2 - textWidth  - ((!narrowed) ? 14 : 11);
    int textY =  (1 - ((double)ui->levelSlider->value()/ui->levelSlider->maximum())) * (ui->levelSlider->height() - FADER_ICON_HEIGHT) + ui->faderPanel->y() + ui->levelSlider->y();

    textY += FADER_ICON_HEIGHT/2;//icon height
    p.drawText(textX, textY, text);
}
