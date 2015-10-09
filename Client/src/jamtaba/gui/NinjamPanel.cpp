#include "NinjamPanel.h"
#include "ui_NinjamPanel.h"
#include <QPainter>
#include <QDebug>
#include <QtAlgorithms>

NinjamPanel::NinjamPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NinjamPanel)
{
    ui->setupUi(this);


    //initialize combos
    for (int bpm = 60; bpm < 200; bpm += 10) {
        ui->comboBpm->addItem(QString::number(bpm), bpm);
    }
    int bpis[] = {16, 32, 48, 64};
    for (int i = 0; i < 4; ++i) {
        ui->comboBpi->addItem(QString::number(bpis[i]), bpis[i]);
    }

    ui->comboBpm->setValidator(new QIntValidator(60, 240, ui->comboBpm));
    ui->comboBpi->setValidator(new QIntValidator(4, 64, ui->comboBpi));

    QObject::connect( ui->comboBeatsPerAccent, SIGNAL(currentIndexChanged(int)), this, SLOT(comboAccentsChanged(int)));
    QObject::connect(ui->comboShape, SIGNAL(currentIndexChanged(int)), this, SLOT(comboShapeChanged(int)));

    buildShapeModel();

    ui->levelSlider->installEventFilter(this);
    ui->panSlider->installEventFilter(this);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::setMetronomePeaks(float left, float right){
    ui->peakMeterLeft->setPeak(left);
    ui->peakMeterRight->setPeak(right);
}

//++++++++++++++++++++++++++++++++++++++++++++++++
bool NinjamPanel::eventFilter(QObject *source, QEvent *ev){
    if(ev->type() == QEvent::MouseButtonDblClick){
        if(source == ui->panSlider){
            ui->panSlider->setValue(0);//center
        }
        if(source == ui->levelSlider){
            ui->levelSlider->setValue(100);//unit gain
        }

        return true;
    }
    return QWidget::eventFilter(source, ev);
}
//++++++++++++++++++++++++++++++++++++
void NinjamPanel::comboAccentsChanged(int index){
    ui->intervalPanel->setShowAccents(index > 0);
    if(ui->intervalPanel->isShowingAccents()){
        ui->intervalPanel->setBeatsPerAccent(ui->comboBeatsPerAccent->currentData().toInt());
    }
}

void NinjamPanel::comboShapeChanged(int index){
    Q_UNUSED(index);
    qDebug() << index;
    ui->intervalPanel->setPaintMode((IntervalProgressDisplay::PaintMode)ui->comboShape->currentData().toInt());
}

//++++++++++++++++++++++++++++++++++++++++++++++++
QStringList NinjamPanel::getBpiDividers(int bpi){
    QStringList foundedDividers;
    int divider = 2;
    while (divider <= bpi / 2) {
        if (bpi % divider == 0) {
            foundedDividers.append(QString::number(divider));
        }
        divider++;
    }

    if (foundedDividers.isEmpty()) {
        foundedDividers.append(QString::number(bpi));//using bpi as default
    }
    qSort(foundedDividers.begin(), foundedDividers.end(), compareBpis);
    return foundedDividers;
}

bool NinjamPanel::compareBpis(const QString &s1, const QString &s2){
    return s1.toInt() < s2.toInt();
}

int NinjamPanel::getIntervalShape() const{
    return ui->comboShape->currentData().toInt();
}

void NinjamPanel::setIntervalShape(int shape){
    int index = 0;
    switch ((IntervalProgressDisplay::PaintMode)shape) {

    case IntervalProgressDisplay::PaintMode::CIRCULAR:
        index = 0;
        break;
    case IntervalProgressDisplay::PaintMode::ELLIPTICAL:
        index = 1;
        break;
    case IntervalProgressDisplay::PaintMode::LINEAR:
        index = 2;
        break;
    default:
        break;
    }
    ui->comboShape->setCurrentIndex(index);
}

void NinjamPanel::buildShapeModel(){
    ui->comboShape->clear();
    ui->comboShape->addItem("Circle", IntervalProgressDisplay::PaintMode::CIRCULAR);
    ui->comboShape->addItem("Ellipse", IntervalProgressDisplay::PaintMode::ELLIPTICAL);
    ui->comboShape->addItem("Line", IntervalProgressDisplay::PaintMode::LINEAR);
}

void NinjamPanel::buildAccentsdModel(int bpi){
    ui->comboBeatsPerAccent->clear();
    ui->comboBeatsPerAccent->addItem("off", 0);
    QStringList bpiDividers = getBpiDividers(bpi);
    for (int d = 0; d < bpiDividers.size(); ++d) {
        QString divider = bpiDividers.at(d);
        ui->comboBeatsPerAccent->addItem(divider + " beats", divider.toInt());
    }
}
//++++++++++++++++++++++
void NinjamPanel::setCurrentBeat(int currentBeat){
    ui->intervalPanel->setCurrentBeat(currentBeat);
}

void NinjamPanel::setBpi(int bpi){
    ui->comboBpi->blockSignals(true);
    ui->comboBpi->setCurrentText(QString::number(bpi));
    ui->comboBpi->blockSignals(false);
    ui->intervalPanel->setBeatsPerInterval(bpi);
    buildAccentsdModel(bpi);
    ui->comboBeatsPerAccent->setCurrentIndex(0);//off
    ui->intervalPanel->setShowAccents(false);

}

void NinjamPanel::setBpm(int bpm){
    ui->comboBpm->blockSignals(true);
    ui->comboBpm->setCurrentText(QString::number(bpm));
    ui->comboBpm->blockSignals(false);
}

QComboBox* NinjamPanel::getBpiCombo() const{
    return ui->comboBpi;
}

QComboBox* NinjamPanel::getBpmCombo() const{
    return ui->comboBpm;
}

QComboBox* NinjamPanel::getAccentsCombo() const{
    return ui->comboBeatsPerAccent;
}

QAbstractSlider *NinjamPanel::getGainSlider() const{
    return ui->levelSlider;
}
QAbstractSlider* NinjamPanel::getPanSlider() const{
    return ui->panSlider;
}

QPushButton* NinjamPanel::getMuteButton() const{
    return ui->muteButton;
}

QPushButton* NinjamPanel::getSoloButton() const{
    return ui->soloButton;
}

NinjamPanel::~NinjamPanel()
{
    delete ui;
}

//little to allow stylesheet in custom widget
void NinjamPanel::paintEvent(QPaintEvent* ){
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
