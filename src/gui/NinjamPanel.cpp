#include "NinjamPanel.h"
#include "ui_NinjamPanel.h"
#include <QPainter>
#include <QDebug>
#include <QtAlgorithms>
#include <QtMath>
#include <QFormLayout>
#include "../log/logging.h"
#include "BpiUtils.h"

NinjamPanel::NinjamPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NinjamPanel),
    hostSyncButton(nullptr)
{
    ui->setupUi(this);


    //initialize combos
    for (int bpm = 40; bpm <= 400; bpm += 10) {
        ui->comboBpm->addItem(QString::number(bpm), bpm);
    }
    int bpis[] = {8, 16, 32, 48, 64};
    for (int i = 0; i < 4; ++i) {
        ui->comboBpi->addItem(QString::number(bpis[i]), bpis[i]);
    }

    ui->comboBpm->setValidator(new QIntValidator(40, 400, ui->comboBpm));
    ui->comboBpi->setValidator(new QIntValidator(3, 64, ui->comboBpi));

    ui->comboBpi->setCompleter(0);//disabling completer
    ui->comboBpm->setCompleter(0);//disabling completer

    QObject::connect( ui->comboBeatsPerAccent, SIGNAL(currentIndexChanged(int)), this, SLOT(comboAccentsChanged(int)));
    QObject::connect(ui->comboShape, SIGNAL(currentIndexChanged(int)), this, SLOT(comboShapeChanged(int)));

    buildShapeModel();

    ui->levelSlider->installEventFilter(this);
    ui->panSlider->installEventFilter(this);

    QObject::connect(ui->comboBpi, SIGNAL(activated(QString)), this, SIGNAL(bpiComboActivated(QString)));
    QObject::connect(ui->comboBpm, SIGNAL(activated(QString)), this, SIGNAL(bpmComboActivated(QString)));
    QObject::connect(ui->comboBeatsPerAccent, SIGNAL(currentIndexChanged(int)), SIGNAL(accentsComboChanged(int)));
    QObject::connect(ui->levelSlider, SIGNAL(valueChanged(int)), this, SIGNAL(gainSliderChanged(int)));
    QObject::connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SIGNAL(panSliderChanged(int)));
    QObject::connect(ui->muteButton, SIGNAL(clicked(bool)), this, SIGNAL(muteButtonClicked()));
    QObject::connect(ui->soloButton, SIGNAL(clicked(bool)), this, SIGNAL(soloButtonClicked()));

    ui->peakMeterLeft->setOrientation(PeakMeter::HORIZONTAL);
    ui->peakMeterRight->setOrientation(PeakMeter::HORIZONTAL);
}
//++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::setFullViewStatus(bool fullView){
    ui->horizontalLayout->setSpacing(fullView ? 6 : 2);
    ui->intervalPanel->setMinimumHeight(fullView ? 120 : (hostSyncButton ? 110 : 90));
    ui->intervalPanel->setMaximumWidth(fullView ? 32768 : 300 );
}

//++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::addMasterControls(QWidget *masterControlsPanel){
    this->layout()->addWidget(masterControlsPanel);

}

//++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::createHostSyncButton(QString buttonText){
    if(!hostSyncButton){//just in case
        hostSyncButton = new QPushButton(buttonText);
        QGridLayout* layout = dynamic_cast<QGridLayout*>(ui->panelCombos->layout());
        layout->addWidget(hostSyncButton, layout->rowCount(), 0, 1, 2);

        QObject::connect(hostSyncButton, SIGNAL(clicked(bool)), this, SIGNAL(hostSyncButtonClicked()));
    }
}

void NinjamPanel::setHostSyncButtonAvailability(bool enabled){
    if(hostSyncButton){
        hostSyncButton->setEnabled(enabled);
    }
}

void NinjamPanel::setBpiComboText(QString text){
    ui->comboBpi->blockSignals(true);
    ui->comboBpi->setCurrentText(text);
    ui->comboBpi->blockSignals(false);
}

void NinjamPanel::setBpmComboText(QString text){
    ui->comboBpm->blockSignals(true);
    ui->comboBpm->setCurrentText(text);
    ui->comboBpm->blockSignals(false);
}

int NinjamPanel::getPanSliderMaximumValue() const{
    return ui->panSlider->maximum();
}

int NinjamPanel::getCurrentBeatsPerAccent() const{
    return ui->comboBeatsPerAccent->currentData().toInt();
}

int NinjamPanel::getGainSliderMaximumValue() const{
    return ui->levelSlider->maximum();
}

void NinjamPanel::setMuteButtonStatus(bool checked){
    ui->muteButton->setChecked(checked);
}

void NinjamPanel::setPanSliderValue(int value){
    ui->panSlider->setValue(value);
}

void NinjamPanel::setGainSliderValue(int value){
    ui->levelSlider->setValue(value);
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
    qCDebug(jtNinjamGUI) << index;
    ui->intervalPanel->setPaintMode((IntervalProgressDisplay::PaintMode)ui->comboShape->currentData().toInt());
}

//++++++++++++++++++++++++++++++++++++++++++++++++
QStringList NinjamPanel::getBpiDividers(int bpi){
    QStringList foundedDividers;
    QList<int> intDividers = BpiUtils::getBpiDividers(bpi);
    foreach (int divider, intDividers) {
        foundedDividers.append(QString::number(divider));
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

void NinjamPanel::selectClosestBeatsPerAccentInCombo(int currentBeatsPerAccent){
    int minorDifference = INT_MAX;
    int closestIndex = -1;
    for(int i=1; i < ui->comboBeatsPerAccent->count(); ++i){
        int difference = qFabs( currentBeatsPerAccent - ui->comboBeatsPerAccent->itemData(i).toInt() );
        if( difference < minorDifference ){
            minorDifference = difference;
            closestIndex = i;
            if(minorDifference <= 1){
                break;
            }
        }
    }
    if(closestIndex >= 0){
        ui->comboBeatsPerAccent->setCurrentIndex(closestIndex);
    }
}

//auto select the last beatsPerAccentValue in combo
void NinjamPanel::selectBeatsPerAccentInCombo(int beatsPerAccent){
    for(int i=0; i < ui->comboBeatsPerAccent->count(); ++i){
        if( beatsPerAccent == ui->comboBeatsPerAccent->itemData(i).toInt() ){
            ui->comboBeatsPerAccent->setCurrentIndex(i);
            break;
        }
    }
}

void NinjamPanel::setBpi(int bpi){
    ui->comboBpi->blockSignals(true);
    ui->comboBpi->setCurrentText(QString::number(bpi));
    ui->comboBpi->blockSignals(false);
    ui->intervalPanel->setBeatsPerInterval(bpi);
    bool showingAccents = ui->intervalPanel->isShowingAccents();
    buildAccentsdModel(bpi);
    if(showingAccents){
        //find the closest possible accent value for the new bpi
        int currentBeatsPerAccent = ui->intervalPanel->getBeatsPerAccent();
        if( bpi % currentBeatsPerAccent == 0 ){//new bpi is compatible with last bpi value?
            selectBeatsPerAccentInCombo(currentBeatsPerAccent);
        }
        else{//new bpi is incompatible with last bpi value. For example, bpi change from 16 to 13
            selectClosestBeatsPerAccentInCombo(currentBeatsPerAccent);
        }
    }
    else{
        ui->comboBeatsPerAccent->setCurrentIndex(0);//off
        ui->intervalPanel->setShowAccents(false);
    }
}

void NinjamPanel::setBpm(int bpm){
    ui->comboBpm->blockSignals(true);
    ui->comboBpm->setCurrentText(QString::number(bpm));
    ui->comboBpm->blockSignals(false);
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
