#include "NinjamPanel.h"
#include "ui_ninjampanel.h"
#include <QPainter>
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
}
//++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::comboAccentsChanged(int index){
    ui->intervalPanel->setShowAccents(index > 0);
    if(ui->intervalPanel->isShowingAccents()){
        ui->intervalPanel->setBeatsPerAccent(ui->comboBeatsPerAccent->currentData().toInt());
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++
QStringList NinjamPanel::getDividers(int bpi){
    QStringList foundedDividers;
    int divider = 2;
    while (divider <= bpi / 2) {
        if (bpi % divider == 0) {
            foundedDividers.append(QString::number(divider));
        }
        divider++;
    }

    if (foundedDividers.isEmpty()) {
        foundedDividers.append(QString::number(bpi));//uso o próprio bpi
    }
    qSort(foundedDividers.begin(), foundedDividers.end(), compareBpis);
    return foundedDividers;
}

bool NinjamPanel::compareBpis(const QString &s1, const QString &s2){
    return s1.toInt() < s2.toInt();
}

void NinjamPanel::buildAccentsdModel(int bpi){
    ui->comboBeatsPerAccent->clear();
    ui->comboBeatsPerAccent->addItem("off", 0);
    QStringList bpiDividers = getDividers(bpi);
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
    ui->comboBpi->setCurrentText(QString::number(bpi));
    ui->intervalPanel->setBeatsPerInterval(bpi);
    buildAccentsdModel(bpi);

    if(ui->intervalPanel->isShowingAccents()){
        if(ui->comboBeatsPerAccent->count() > 1){
            ui->comboBeatsPerAccent->setCurrentIndex(1);//auto choose the first available beats per accent value
        }
    }
}

void NinjamPanel::setBpm(int bpm){
    ui->comboBpm->setCurrentText(QString::number(bpm));
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
