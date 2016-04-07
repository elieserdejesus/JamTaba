#include "NinjamPanel.h"
#include "ui_NinjamPanel.h"
#include "log/Logging.h"
#include "BpiUtils.h"
#include "intervalProgress/IntervalProgressWindow.h"

#include <QDebug>
#include <QtAlgorithms>
#include <QtMath>
#include <QFormLayout>

NinjamPanel::NinjamPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NinjamPanel),
    hostSyncButton(nullptr),
    metronomeFloatingWindow(nullptr)
{
    ui->setupUi(this);

    // initialize combos
    for (int bpm = 40; bpm <= 200; bpm += 5)
        ui->comboBpm->addItem(QString::number(bpm), bpm);
    int bpis[] = {8, 12, 16, 24, 32, 48, 64};
    for (int i = 0; i < 7; ++i)
        ui->comboBpi->addItem(QString::number(bpis[i]), bpis[i]);

    ui->comboBpm->setValidator(new QIntValidator(40, 400, ui->comboBpm));
    ui->comboBpi->setValidator(new QIntValidator(3, 64, ui->comboBpi));

    ui->comboBpi->setCompleter(0);// disabling completer
    ui->comboBpm->setCompleter(0);// disabling completer

    buildShapeModel();

    ui->levelSlider->installEventFilter(this);
    ui->panSlider->installEventFilter(this);

    ui->peakMeterLeft->setOrientation(Qt::Horizontal);
    ui->peakMeterRight->setOrientation(Qt::Horizontal);

    setupSignals();
}

void NinjamPanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);

        //rebuild the accents and shape combos to show translated strings. The signals are blocked to avoid reset the combos and loose the user selections.
        QSignalBlocker comboShapeBlocker(ui->comboShape);
        QSignalBlocker comboAccentsBlocker(ui->comboBeatsPerAccent);

        //save the current indexes before rebuild the combos
        int currentShape = ui->comboShape->currentIndex();
        int currentAccent = ui->comboBeatsPerAccent->currentIndex();

        buildAccentsdModel(ui->intervalPanel->getBeatsPerInterval());
        buildShapeModel();

        //restore the selected indexes
        ui->comboShape->setCurrentIndex(currentShape);
        ui->comboBeatsPerAccent->setCurrentIndex(currentAccent);
    }
    QWidget::changeEvent(e);
}

void NinjamPanel::setupSignals()
{
    connect(ui->comboBeatsPerAccent, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAccentsStatus(int)));
    connect(ui->comboShape, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIntervalProgressShape(int)));

    connect(ui->comboBpi, SIGNAL(activated(QString)), this, SIGNAL(bpiComboActivated(QString)));
    connect(ui->comboBpm, SIGNAL(activated(QString)), this, SIGNAL(bpmComboActivated(QString)));
    connect(ui->comboBeatsPerAccent, SIGNAL(currentIndexChanged(int)), SIGNAL(accentsComboChanged(int)));
    connect(ui->levelSlider, SIGNAL(valueChanged(int)), this, SIGNAL(gainSliderChanged(
                                                                                  int)));
    connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SIGNAL(panSliderChanged(int)));
    connect(ui->muteButton, SIGNAL(clicked(bool)), this, SIGNAL(muteButtonClicked()));
    connect(ui->soloButton, SIGNAL(clicked(bool)), this, SIGNAL(soloButtonClicked()));
    connect(ui->preferencesButton, SIGNAL(clicked(bool)), this, SIGNAL(preferencesButtonClicked()));

    connect(ui->floatingWindowButton, SIGNAL(toggled(bool)), this, SLOT(toggleMetronomeFloatingWindowVisibility(bool)));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::toggleMetronomeFloatingWindowVisibility(bool showFloatingWindow)
{
    if (showFloatingWindow){
        if (!metronomeFloatingWindow) {
            IntervalProgressDisplay::PaintShape paintMode = ui->intervalPanel->getPaintMode();
            int beatsPerInterval = ui->intervalPanel->getBeatsPerInterval();
            int beatsPerAccent = ui->intervalPanel->getBeatsPerAccent();
            bool showingAccents = ui->intervalPanel->isShowingAccents();
            metronomeFloatingWindow = new IntervalProgressWindow(this, paintMode, beatsPerInterval, beatsPerAccent, showingAccents);
            connect(metronomeFloatingWindow, SIGNAL(windowClosed()), this, SLOT(deleteFloatWindow()));
        }
        metronomeFloatingWindow->setVisible(true);
        metronomeFloatingWindow->raise();
    }
    else{
        if (metronomeFloatingWindow) {
            metronomeFloatingWindow->setVisible(false);
            deleteFloatWindow();
        }
    }
    ui->floatingWindowButton->setChecked(showFloatingWindow);
}

void NinjamPanel::deleteFloatWindow()
{
    metronomeFloatingWindow->deleteLater();
    metronomeFloatingWindow = nullptr;
    ui->floatingWindowButton->setChecked(false);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::setLowContrastPaintInIntervalPanel(bool useLowContrastColors)
{
    ui->intervalPanel->setPaintUsingLowContrastColors(useLowContrastColors);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::setFullViewStatus(bool fullView)
{
    ui->horizontalLayout->setSpacing(fullView ? 6 : 2);
    ui->intervalPanel->setMinimumHeight(fullView ? 120 : (hostSyncButton ? 110 : 90));
    ui->intervalPanel->setMaximumWidth(fullView ? 32768 : 300);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::addMasterControls(QWidget *masterControlsPanel)
{
    this->layout()->addWidget(masterControlsPanel);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
//TODO this function and the hostSyncButton are used only in VST plugin. Create a NinjamPanelVST inheriting from NinjamPanel
void NinjamPanel::createHostSyncButton(const QString &buttonText)
{
    if (!hostSyncButton) {// just in case
        hostSyncButton = new QPushButton(buttonText);
        hostSyncButton->setCheckable(true);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(ui->panelCombos->layout());
        layout->addWidget(hostSyncButton, layout->rowCount(), 0, 1, 2);

        connect(hostSyncButton, SIGNAL(clicked(bool)), this, SIGNAL(hostSyncStateChanged(bool)));
    }
}

void NinjamPanel::uncheckHostSyncButton()
{
    if(hostSyncButton)
        hostSyncButton->setChecked(false);
}

void NinjamPanel::setBpiComboText(const QString &text)
{
    ui->comboBpi->blockSignals(true);
    ui->comboBpi->setCurrentText(text);
    ui->comboBpi->blockSignals(false);
}

void NinjamPanel::setBpmComboText(const QString &text)
{
    ui->comboBpm->blockSignals(true);
    ui->comboBpm->setCurrentText(text);
    ui->comboBpm->blockSignals(false);
}

int NinjamPanel::getPanSliderMaximumValue() const
{
    return ui->panSlider->maximum();
}

int NinjamPanel::getCurrentBeatsPerAccent() const
{
    return ui->comboBeatsPerAccent->currentData().toInt();
}

int NinjamPanel::getGainSliderMaximumValue() const
{
    return ui->levelSlider->maximum();
}

void NinjamPanel::setMuteButtonStatus(bool checked)
{
    ui->muteButton->setChecked(checked);
}

void NinjamPanel::setPanSliderValue(int value)
{
    ui->panSlider->setValue(value);
}

void NinjamPanel::setGainSliderValue(int value)
{
    ui->levelSlider->setValue(value);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
void NinjamPanel::setMetronomePeaks(float left, float right)
{
    ui->peakMeterLeft->setPeak(left);
    ui->peakMeterRight->setPeak(right);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
bool NinjamPanel::eventFilter(QObject *source, QEvent *ev)
{
    if (ev->type() == QEvent::MouseButtonDblClick) {
        if (source == ui->panSlider)
            ui->panSlider->setValue(0);// center
        if (source == ui->levelSlider)
            ui->levelSlider->setValue(100);// unit gain

        return true;
    }
    return QWidget::eventFilter(source, ev);
}

// ++++++++++++++++++++++++++++++++++++
void NinjamPanel::updateAccentsStatus(int index)
{
    bool showingAccents = index > 0;
    ui->intervalPanel->setShowAccents(showingAccents);
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setShowAccents(showingAccents);
    }

    if (showingAccents){
        int beatsPerAccent = ui->comboBeatsPerAccent->currentData().toInt();
        ui->intervalPanel->setBeatsPerAccent(beatsPerAccent);
        if (metronomeFloatingWindow) {
            metronomeFloatingWindow->setBeatsPerAccent(beatsPerAccent);
        }
    }
}

void NinjamPanel::updateIntervalProgressShape(int index)
{
    Q_UNUSED(index);
    IntervalProgressDisplay::PaintShape selectedShape = (IntervalProgressDisplay::PaintShape)ui->comboShape->currentData().toInt();
    ui->intervalPanel->setPaintMode(selectedShape);
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setIntervalProgressShape(selectedShape);
    }
    emit intervalShapeChanged(selectedShape);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++
QStringList NinjamPanel::getBpiDividers(int bpi)
{
    QStringList foundedDividers;
    QList<int> intDividers = BpiUtils::getBpiDividers(bpi);
    foreach (int divider, intDividers)
        foundedDividers.append(QString::number(divider));
    qSort(foundedDividers.begin(), foundedDividers.end(), compareBpis);
    return foundedDividers;
}

bool NinjamPanel::compareBpis(const QString &s1, const QString &s2)
{
    return s1.toInt() < s2.toInt();
}

int NinjamPanel::getIntervalShape() const
{
    return ui->comboShape->currentData().toInt();
}

void NinjamPanel::setIntervalShape(int shape)
{
    int index = 0;
    switch ((IntervalProgressDisplay::PaintShape)shape) {
    case IntervalProgressDisplay::PaintShape::CIRCULAR:
        index = 0;
        break;
    case IntervalProgressDisplay::PaintShape::ELLIPTICAL:
        index = 1;
        break;
    case IntervalProgressDisplay::PaintShape::LINEAR:
        index = 2;
        break;
    case IntervalProgressDisplay::PaintShape::PIE:
        index = 3;
        break;
    }
    ui->comboShape->setCurrentIndex(index);
    updateIntervalProgressShape(index);
}

void NinjamPanel::buildShapeModel()
{
    ui->comboShape->clear();
    ui->comboShape->addItem(tr("Circle"), IntervalProgressDisplay::PaintShape::CIRCULAR);
    ui->comboShape->addItem(tr("Ellipse"), IntervalProgressDisplay::PaintShape::ELLIPTICAL);
    ui->comboShape->addItem(tr("Line"), IntervalProgressDisplay::PaintShape::LINEAR);
    ui->comboShape->addItem(tr("Pie"), IntervalProgressDisplay::PaintShape::PIE);
}

void NinjamPanel::buildAccentsdModel(int bpi)
{
    ui->comboBeatsPerAccent->clear();
    ui->comboBeatsPerAccent->addItem(tr("off"), 0);
    QStringList bpiDividers = getBpiDividers(bpi);
    for (int d = 0; d < bpiDividers.size(); ++d) {
        QString divider = bpiDividers.at(d);
        ui->comboBeatsPerAccent->addItem(tr("%1 beats").arg(divider), divider);
    }
}

// ++++++++++++++++++++++
void NinjamPanel::setCurrentBeat(int currentBeat)
{
    ui->intervalPanel->setCurrentBeat(currentBeat);
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setCurrentBeat(currentBeat);
    }
}

void NinjamPanel::selectClosestBeatsPerAccentInCombo(int currentBeatsPerAccent)
{
    int minorDifference = INT_MAX;
    int closestIndex = -1;
    for (int i = 1; i < ui->comboBeatsPerAccent->count(); ++i) {
        int difference
            = qFabs(currentBeatsPerAccent - ui->comboBeatsPerAccent->itemData(i).toInt());
        if (difference < minorDifference) {
            minorDifference = difference;
            closestIndex = i;
            if (minorDifference <= 1)
                break;
        }
    }
    if (closestIndex >= 0)
        ui->comboBeatsPerAccent->setCurrentIndex(closestIndex);
}

// auto select the last beatsPerAccentValue in combo
void NinjamPanel::selectBeatsPerAccentInCombo(int beatsPerAccent)
{
    for (int i = 0; i < ui->comboBeatsPerAccent->count(); ++i) {
        if (beatsPerAccent == ui->comboBeatsPerAccent->itemData(i).toInt()) {
            ui->comboBeatsPerAccent->setCurrentIndex(i);
            break;
        }
    }
}

void NinjamPanel::setBpi(int bpi)
{
    ui->comboBpi->blockSignals(true);
    ui->comboBpi->setCurrentText(QString::number(bpi));
    ui->comboBpi->blockSignals(false);
    ui->intervalPanel->setBeatsPerInterval(bpi);
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setBeatsPerInterval(bpi);
    }
    bool showingAccents = ui->intervalPanel->isShowingAccents();
    buildAccentsdModel(bpi);
    if (showingAccents) {
        // find the closest possible accent value for the new bpi
        int currentBeatsPerAccent = ui->intervalPanel->getBeatsPerAccent();
        if (bpi % currentBeatsPerAccent == 0)  // new bpi is compatible with last bpi value?
            selectBeatsPerAccentInCombo(currentBeatsPerAccent);
        else // new bpi is incompatible with last bpi value. For example, bpi change from 16 to 13
            selectClosestBeatsPerAccentInCombo(currentBeatsPerAccent);
    } else {
        ui->comboBeatsPerAccent->setCurrentIndex(0);// off
        ui->intervalPanel->setShowAccents(false);
        if (metronomeFloatingWindow) {
            metronomeFloatingWindow->setShowAccents(false);
        }
    }
}

void NinjamPanel::setBpm(int bpm)
{
    ui->comboBpm->blockSignals(true);
    ui->comboBpm->setCurrentText(QString::number(bpm));
    ui->comboBpm->blockSignals(false);
}

NinjamPanel::~NinjamPanel()
{
    delete ui;
}
