#include "NinjamPanel.h"
#include "ui_NinjamPanel.h"
#include "log/Logging.h"
#include "BpiUtils.h"
#include "intervalProgress/IntervalProgressWindow.h"
#include "TextEditorModifier.h"
#include "MetronomeUtils.h"

#include <QDebug>
#include <QtAlgorithms>
#include <QtMath>
#include <QFormLayout>

NinjamPanel::NinjamPanel(TextEditorModifier *bpiComboModifier, TextEditorModifier *bpmComboModifier, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NinjamPanel),
    hostSyncButton(nullptr),
    metronomeFloatingWindow(nullptr)
{
    qCDebug(jtNinjamGUI) << "NinjamPanel::NinjamPanel ctor";
    ui->setupUi(this);

    ui->levelSlider->setSliderType(Slider::AudioSlider);
    ui->panSlider->setSliderType(Slider::PanSlider);

    initializeCombos(bpiComboModifier, bpmComboModifier);

    ui->peakMeter->setOrientation(Qt::Horizontal);

    maximizeControlsWidget(true);

    setupSignals();

    translate();
    qCDebug(jtNinjamGUI) << "NinjamPanel::NinjamPanel done";
}

void NinjamPanel::updateStyleSheet()
{
    ui->peakMeter->updateStyleSheet();
}

void NinjamPanel::initializeCombos(TextEditorModifier *bpiModifier, TextEditorModifier *bpmModifier)
{
    // initialize combos
    const quint16 MIN_BPM = 40;
    for (quint16 bpm = MIN_BPM; bpm <= 200; bpm += 5)
        ui->comboBpm->addItem(QString::number(bpm), bpm);

    int bpis[] = {8, 12, 16, 24, 32, 48, 64};
    for (int bpi : bpis)
        ui->comboBpi->addItem(QString::number(bpi), bpi);

    ui->comboBpm->setValidator(new QIntValidator(MIN_BPM, 400, ui->comboBpm));
    ui->comboBpi->setValidator(new QIntValidator(2, 64, ui->comboBpi));

    ui->comboBpi->setCompleter(nullptr); // disabling completer
    ui->comboBpm->setCompleter(nullptr); // disabling completer

    if (bpiModifier) // just to be sure
        bpiModifier->modify(ui->comboBpi); // modify the comboBox QLineEdit to work in plugins (AU/VST)

    if (bpmModifier)
        bpmModifier->modify(ui->comboBpm);

}

void NinjamPanel::maximizeControlsWidget(bool maximize)
{
     ui->intervalPanel->setMaximumWidth(maximize ? 32767 : 300);
}

void NinjamPanel::setBpiComboPendingStatus(bool enabled)
{
    ui->comboBpi->setProperty("pending", enabled);
    style()->unpolish(ui->comboBpi);
    style()->polish(ui->comboBpi);
}

void NinjamPanel::setBpmComboPendingStatus(bool enabled)
{
    ui->comboBpm->setProperty("pending", enabled);
    style()->unpolish(ui->comboBpm);
    style()->polish(ui->comboBpm);
}

void NinjamPanel::translate()
{
    ui->retranslateUi(this);

    // rebuild the accents and shape combos to show translated strings. The signals are blocked to avoid reset the combos and loose the user selections.
    QSignalBlocker comboShapeBlocker(ui->comboShape);
    QSignalBlocker comboAccentsBlocker(ui->comboAccentBeats);

    // save the current indexes before rebuild the combos
    int currentShape = ui->comboShape->currentIndex();
    int currentAccent = ui->comboAccentBeats->currentIndex();

    buildAccentsdModel(ui->intervalPanel->getBeatsPerInterval());
    buildShapeModel();

    // restore the selected indexes
    ui->comboShape->setCurrentIndex(currentShape);
    ui->comboAccentBeats->setCurrentIndex(currentAccent);

    // compute the max width string in combo and set the combobox list items width to match
    int items = ui->comboShape->count();
    QFontMetrics fontMetrics = ui->comboShape->fontMetrics();
    int maxComboItemWidth = 0;
    for (int i = 0; i < items; ++i) {
        int itemTextWidth = fontMetrics.width(ui->comboShape->itemText(i));
        if (itemTextWidth > maxComboItemWidth)
            maxComboItemWidth = itemTextWidth;
    }
    ui->comboShape->view()->setMinimumWidth(qMax(ui->comboShape->width(), maxComboItemWidth + 20));

    // translate sync button
    if (hostSyncButton) {
        hostSyncButton->setText(tr("Sync with %1").arg(hostName));
    }

}

void NinjamPanel::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void NinjamPanel::setupSignals()
{
    connect(ui->comboAccentBeats, SIGNAL(currentIndexChanged(int)), this, SLOT(handleAccentBeatsIndexChanged(int)));
    connect(ui->comboAccentBeats, SIGNAL(currentIndexChanged(int)), SIGNAL(accentsComboChanged(int)));
    connect(ui->lineEditAccentBeats, SIGNAL(&QLineEdit::returnPressed), this, SLOT(handleAccentBeatsTextEdited()));
    connect(ui->lineEditAccentBeats, SIGNAL(textChanged(QString)), SIGNAL(accentsTextChanged(QString)));
    connect(ui->comboShape, SIGNAL(currentIndexChanged(int)), this, SLOT(updateIntervalProgressShape(int)));

    connect(ui->comboBpi, SIGNAL(activated(QString)), this, SLOT(handleBpiComboActication(QString)));
    connect(ui->comboBpm, SIGNAL(activated(QString)), this, SLOT(handleBpmComboActication(QString)));
    connect(ui->levelSlider, SIGNAL(valueChanged(int)), this, SIGNAL(gainSliderChanged(
                                                                                  int)));
    connect(ui->panSlider, SIGNAL(valueChanged(int)), this, SIGNAL(panSliderChanged(int)));
    connect(ui->muteButton, SIGNAL(clicked(bool)), this, SIGNAL(muteButtonClicked()));
    connect(ui->soloButton, SIGNAL(clicked(bool)), this, SIGNAL(soloButtonClicked()));
    connect(ui->preferencesButton, SIGNAL(clicked(bool)), this, SIGNAL(preferencesButtonClicked()));

    connect(ui->floatingWindowButton, SIGNAL(toggled(bool)), this, SLOT(setMetronomeFloatingWindowVisibility(bool)));
}

void NinjamPanel::handleBpiComboActication(const QString &newBpi)
{
    ui->comboBpi->clearFocus();
    emit bpiComboActivated(newBpi);
}

void NinjamPanel::handleBpmComboActication(const QString &newBpm)
{
    ui->comboBpm->clearFocus();
    emit bpmComboActivated(newBpm);
}

bool NinjamPanel::metronomeFloatingWindowIsVisible() const
{
    return metronomeFloatingWindow && metronomeFloatingWindow->isVisible();
}

void NinjamPanel::setMetronomeFloatingWindowVisibility(bool showFloatingWindow)
{
    if (showFloatingWindow){
        if (!metronomeFloatingWindow) {
            IntervalProgressDisplay::PaintShape paintMode = ui->intervalPanel->getPaintMode();
            int beatsPerInterval = ui->intervalPanel->getBeatsPerInterval();
            QList<int> accentBeats = ui->intervalPanel->getAccentBeats();
            bool showingAccents = ui->intervalPanel->isShowingAccents();
            metronomeFloatingWindow = new IntervalProgressWindow(nullptr, paintMode, beatsPerInterval, accentBeats, showingAccents);
            connect(metronomeFloatingWindow, SIGNAL(windowClosed()), this, SLOT(deleteFloatWindow()));
        }
        
        metronomeFloatingWindow->move(10, 10); // top left
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
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->deleteLater();
        metronomeFloatingWindow = nullptr;
        ui->floatingWindowButton->setChecked(false);
    }
}

void NinjamPanel::setLowContrastPaintInIntervalPanel(bool useLowContrastColors)
{
    ui->intervalPanel->setPaintUsingLowContrastColors(useLowContrastColors);
}

void NinjamPanel::addMasterControls(QWidget *masterControlsPanel)
{
    this->layout()->addWidget(masterControlsPanel);
}


// TODO this function and the hostSyncButton are used only in VST plugin. Create a NinjamPanelVST inheriting from NinjamPanel
void NinjamPanel::createHostSyncButton(const QString &hostName)
{
    this->hostName = hostName;

    if (!hostSyncButton) {// just in case
        //: The '%1' marker will be replaced by the host name when Jamtaba is running.
        hostSyncButton = new QPushButton(tr("Sync with %1").arg(hostName));
        hostSyncButton->setCheckable(true);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(ui->panelCombos->layout());
        layout->addWidget(hostSyncButton, layout->rowCount(), 0, 1, 2);

        connect(hostSyncButton, &QPushButton::clicked, this, &NinjamPanel::hostSyncStateChanged);
    }
}

bool NinjamPanel::hostSyncButtonIsChecked() const
{
    if (hostSyncButton)
        return hostSyncButton->isChecked();

    return false;
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

int NinjamPanel::getAccentBeatsComboValue() const
{
    return ui->comboAccentBeats->currentData().toInt();
}

bool NinjamPanel::isAccentBeatsEnabled() const
{
    return ui->lineEditAccentBeats->isEnabled();
}

void NinjamPanel::setAccentBeatsTextEnabled(bool value)
{
    qCDebug(jtNinjamGUI) << "NinjamPanel::setAccentBeatsTextEnabled " << value;
    ui->lineEditAccentBeats->setEnabled(value);
}

QString NinjamPanel::getAccentBeatsText() const
{
    return ui->lineEditAccentBeats->text();
}

void NinjamPanel::setAccentBeatsText(QString value)
{
    qCDebug(jtNinjamGUI) << "NinjamPanel::setAccentBeatsText " << value;
    ui->lineEditAccentBeats->blockSignals(false);
    ui->lineEditAccentBeats->setText(value);
    ui->lineEditAccentBeats->blockSignals(true);
    qCDebug(jtNinjamGUI) << "NinjamPanel::setAccentBeatsText done";
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

void NinjamPanel::setMetronomePeaks(float left, float right, float rmsLeft, float rmsRight)
{
    ui->peakMeter->setPeak(left, right, rmsLeft, rmsRight);
}

void NinjamPanel::handleAccentBeatsIndexChanged(int index)
{
    Q_UNUSED(index);
    qCDebug(jtNinjamGUI) << "NinjamPanel::handleAccentBeatsIndexChanged " << index;
    NinjamPanel::updateAccentsStatus();
}

void NinjamPanel::handleAccentBeatsTextEdited() {
    qCDebug(jtNinjamGUI) << "NinjamPanel::handleAccentBeatsTextEdited";
    NinjamPanel::updateAccentsStatus();
}

void NinjamPanel::updateAccentsStatus()
{
    int accentBeatsCb = ui->comboAccentBeats->currentData().toInt();
    qCDebug(jtNinjamGUI) << "NinjamPanel::updateAccentsStatus " << accentBeatsCb;

    QList<int> accentBeats;
    if (accentBeatsCb != 0) {
        if (accentBeatsCb > 0) {
            int bpi = ui->intervalPanel->getBeatsPerInterval();
            accentBeats = Audio::MetronomeUtils::getAccentBeats(accentBeatsCb, bpi);
        } else {
            if (getAccentBeatsText().isEmpty()) {
                QList<QString> strs;
                foreach (int b, ui->intervalPanel->getAccentBeats()) {
                    strs.append(QString::number(b + 1));
                }
                // OK, now we're happy the accent beats text is correct
                setAccentBeatsText(strs.join(" "));
            }
            accentBeats = Audio::MetronomeUtils::getAccentBeatsFromString(getAccentBeatsText());
            // We are resposible for forcing this:
            emit accentsTextChanged("");
        }
        ui->intervalPanel->setAccentBeats(accentBeats);
        if (metronomeFloatingWindow) {
            metronomeFloatingWindow->setAccentBeats(accentBeats);
        }
    }

    ui->intervalPanel->setShowAccents(!accentBeats.isEmpty());
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setShowAccents(!accentBeats.isEmpty());
    }
}

void NinjamPanel::updateIntervalProgressShape(int index)
{
    Q_UNUSED(index);
    auto selectedShape = (IntervalProgressDisplay::PaintShape)ui->comboShape->currentData().toInt();
    ui->intervalPanel->setPaintMode(selectedShape);
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setIntervalProgressShape(selectedShape);
    }

    emit intervalShapeChanged(selectedShape);
}

QStringList NinjamPanel::getBpiDividers(int bpi)
{
    QStringList foundedDividers;
    QList<uint> intDividers = BpiUtils::getBpiDividers(bpi);
    for (uint divider : intDividers)
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
    ui->comboShape->blockSignals(true);
    ui->comboShape->clear();
    ui->comboShape->addItem(tr("Circle"), IntervalProgressDisplay::PaintShape::CIRCULAR);
    ui->comboShape->addItem(tr("Ellipse"), IntervalProgressDisplay::PaintShape::ELLIPTICAL);
    ui->comboShape->addItem(tr("Line"), IntervalProgressDisplay::PaintShape::LINEAR);
    ui->comboShape->addItem(tr("Pie"), IntervalProgressDisplay::PaintShape::PIE);
    ui->comboShape->blockSignals(false);
}

void NinjamPanel::buildAccentsdModel(int bpi)
{
    ui->comboAccentBeats->blockSignals(true);
    ui->comboAccentBeats->clear();
    ui->comboAccentBeats->addItem(tr("off"), 0);
    QStringList bpiDividers = getBpiDividers(bpi);
    for (int d = 0; d < bpiDividers.size(); ++d) {
        QString divider = bpiDividers.at(d);
        ui->comboAccentBeats->addItem(tr("%1 beats").arg(divider), divider);
    }
    ui->comboAccentBeats->addItem(tr("Custom..."), -1);
    ui->comboAccentBeats->blockSignals(false);
}

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
    for (int i = 1; i < ui->comboAccentBeats->count(); ++i) {
        int difference
            = qFabs(currentBeatsPerAccent - ui->comboAccentBeats->itemData(i).toInt());
        if (difference < minorDifference) {
            minorDifference = difference;
            closestIndex = i;
            if (minorDifference <= 1)
                break;
        }
    }
    if (closestIndex >= 0)
        ui->comboAccentBeats->setCurrentIndex(closestIndex);
}

// auto select the last beatsPerAccentValue in combo
void NinjamPanel::selectBeatsPerAccentInCombo(int beatsPerAccent)
{
    for (int i = 0; i < ui->comboAccentBeats->count(); ++i) {
        if (beatsPerAccent == ui->comboAccentBeats->itemData(i).toInt()) {
            ui->comboAccentBeats->setCurrentIndex(i);
            break;
        }
    }
}

void NinjamPanel::setBpi(int bpi)
{
    qCDebug(jtNinjamGUI) << "NinjamPanel::setBpi " << bpi;
    ui->comboBpi->blockSignals(true);
    ui->comboBpi->setCurrentText(QString::number(bpi));
    ui->comboBpi->blockSignals(false);
    ui->intervalPanel->setBeatsPerInterval(bpi);
    if (metronomeFloatingWindow) {
        metronomeFloatingWindow->setBeatsPerInterval(bpi);
    }
    bool showingAccents = ui->intervalPanel->isShowingAccents();
    buildAccentsdModel(bpi);
    int accentBeatsCb = getAccentBeatsComboValue();
    if (showingAccents && accentBeatsCb > 0) {
        // find the closest possible accent value for the new bpi
        if (bpi % accentBeatsCb == 0)  {// new bpi is compatible with last bpi value?
            selectBeatsPerAccentInCombo(accentBeatsCb);
        } else {// new bpi is incompatible with last bpi value. For example, bpi change from 16 to 13
            selectClosestBeatsPerAccentInCombo(accentBeatsCb);
        }
    } else {
        ui->comboAccentBeats->setCurrentIndex(0);// off
        ui->intervalPanel->setShowAccents(false);
        if (metronomeFloatingWindow) {
            metronomeFloatingWindow->setShowAccents(false);
        }
    }
    qCDebug(jtNinjamGUI) << "NinjamPanel::setBpi ...done";
}

void NinjamPanel::setBpm(int bpm)
{
    qCDebug(jtNinjamGUI) << "NinjamPanel::setBpm " << bpm;
    ui->comboBpm->blockSignals(true);
    ui->comboBpm->setCurrentText(QString::number(bpm));
    ui->comboBpm->blockSignals(false);
    qCDebug(jtNinjamGUI) << "NinjamPanel::setBpm ...done";
}

NinjamPanel::~NinjamPanel()
{
    deleteFloatWindow();

    delete ui;
}
