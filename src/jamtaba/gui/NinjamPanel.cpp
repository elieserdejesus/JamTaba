#include "NinjamPanel.h"
#include "ui_ninjampanel.h"
#include <QPainter>

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
}

void NinjamPanel::setCurrentBeat(int currentBeat){
    ui->intervalPanel->setCurrentBeat(currentBeat);
}

void NinjamPanel::setBpi(int bpi){
    ui->comboBpi->setCurrentText(QString::number(bpi));
    ui->intervalPanel->setBeatsPerInterval(bpi);
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
