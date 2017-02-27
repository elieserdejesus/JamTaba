#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"

#include <QVBoxLayout>
#include <QSpinBox>
#include <QKeyEvent>
#include "gui/GuiUtils.h"

using namespace Controller;
using namespace Audio;

LooperWindow::LooperWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LooperWindow),
    looper(nullptr),
    controller(nullptr),
    currentBeat(-1)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // remove help/question marker

    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(12);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->layersWidget->setLayout(layout);

    // set as non resizable
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumSize(size());
    setMaximumSize(size());
    setSizeGripEnabled(false);

    initializeControls();
}

void LooperWindow::keyPressEvent(QKeyEvent *ev)
{
    if (!looper)
        return;

    int key = ev->key();
    bool pressingNumberKey = key >= Qt::Key_1 && key <= Qt::Key_9;
    bool pressingDelete = key == Qt::Key_Delete;
    if (pressingNumberKey) {
        quint8 layerIndex = (key - Qt::Key_0) - 1; // convert key to zero based layer index
        looper->selectLayer(layerIndex);
    }
    else if (pressingDelete) {
        looper->clearCurrentLayer();
    }
}

void LooperWindow::paintEvent(QPaintEvent *ev)
{
    QDialog::paintEvent(ev);

    if (!looper)
        return;

    if (looper->isWaiting()) {

        QPainter painter(this);

        static const QPen pen(QColor(0, 0, 0, 60), 1.0, Qt::DotLine);
        painter.setPen(pen);
        uint bpi = controller->getCurrentBpi();
        LooperWavePanel *wavePanel = wavePanels[looper->getCurrentLayerIndex()];
        if (!wavePanel)
            return;

        QPointF rectTopLeft = wavePanel->mapTo(this, wavePanel->rect().topLeft());
        QSizeF rectSize = wavePanel->size();
        qreal pixelsPerBeat = (rectSize.width()/static_cast<qreal>(bpi));
        for (uint beat = 0; beat < bpi; ++beat) {
            const qreal x = rectTopLeft.x() + beat * pixelsPerBeat;
            painter.drawLine(QPointF(x, rectTopLeft.y()), QPointF(x, rectTopLeft.y() + rectSize.height()));
        }

        // draw a transparent red rect in current beat when waiting
        if (currentBeat) {
            static const QColor redColor(255, 0, 0, 25);
            const qreal x = rectTopLeft.x() + currentBeat * pixelsPerBeat;
            painter.fillRect(QRectF(x, rectTopLeft.y(), pixelsPerBeat, rectSize.height()), redColor);

            const uint waitBeats = controller->getCurrentBpi() - currentBeat;
            QString text = tr("wait (%1)").arg(QString::number(waitBeats));
            painter.drawText(QRectF(rectTopLeft, rectSize), text, QTextOption(Qt::AlignCenter));
        }

        // draw a red border in current layer
        static const QColor redColor(255, 0, 0, 30);
        painter.setPen(redColor);
        painter.drawRect(QRectF(rectTopLeft, QSizeF(wavePanel->width()-1, wavePanel->height()-1)));
    }
}

void LooperWindow::updateDrawings()
{
    if (!looper->isWaiting()) {
        bool drawLayersNumbers = looper->getMaxLayers() > 1 && looper->getMode() != Looper::ALL_LAYERS;
        for (LooperWavePanel *wavePanel : wavePanels.values()) {
            if (wavePanel->isVisible())
                wavePanel->updateDrawings(drawLayersNumbers);
        }
    }
    else {
        update(); // paint vertical lines, current beat and wait count
    }
}

void LooperWindow::detachCurrentLooper()
{
    if (this->looper) {
        this->looper = nullptr;
        this->controller = nullptr;
    }
}

void LooperWindow::setLooper(Audio::Looper *looper, Controller::NinjamController *controller)
{
    Q_ASSERT(looper);
    Q_ASSERT(controller);

    if (looper != this->looper) { // check if user is not just reopening the looper editor

        if (this->looper && this->looper->isRecording()) {
            this->looper->stop();
        }

        deleteWavePanels();

        detachCurrentLooper();

        // create wave panels (layers view)
        quint8 currentMaxLayers = looper->getMaxLayers();
        for (quint8 layerIndex = 0; layerIndex < Audio::Looper::MAX_LOOP_LAYERS; ++layerIndex) {
            LooperWavePanel *wavePanel = new LooperWavePanel(looper, layerIndex);
            wavePanels.insert(layerIndex, wavePanel);
            ui->layersWidget->layout()->addWidget(wavePanel);
            wavePanel->setVisible(layerIndex < currentMaxLayers);
        }

        // initial values
        ui->maxLayersSpinBox->setValue(looper->getMaxLayers());

        ui->checkBoxHearLayersWhileRecording->setChecked(looper->isHearingLayersWhileRecording());
        //ui->checkBoxHearLayersWhileRecording->setVisible(looper->getMode() == Looper::ALL_LAYERS);

        //ui->checkBoxOverdub->setVisible(looper->getMode() == Looper::SELECTED_LAYER);
        ui->checkBoxOverdub->setChecked(looper->isOverdubbing());

        QString selectedPlayMode = looper->getModeString(looper->getMode());
        for (int i = 0; i < ui->comboBoxPlayMode->count(); ++i) {
            if (ui->comboBoxPlayMode->itemText(i) == selectedPlayMode) {
                ui->comboBoxPlayMode->setCurrentIndex(i);
                break;
            }
        }

        connect(controller, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::intervalBeatChanged, this, &LooperWindow::updateCurrentBeat);
        connect(looper, &Looper::stateChanged, this, &LooperWindow::updateControls);
        connect(looper, &Looper::layerLockedStateChanged, this, &LooperWindow::updateControls);
        connect(looper, &Looper::maxLayersChanged, this, &LooperWindow::updateLayersVisibility);
        connect(looper, &Looper::maxLayersChanged, this, &LooperWindow::updateControls);
        connect(looper, &Looper::modeChanged, this, &LooperWindow::updateControls);
        connect(looper, &Looper::currentLayerChanged, this, &LooperWindow::updateControls);

        this->looper = looper;
        this->controller = controller;
    }

    updateBeatsPerInterval();
    updateControls();
}

void LooperWindow::updateLayersVisibility(quint8 newMaxLayers)
{
    for (quint8 layerIndex = 0; layerIndex < Audio::Looper::MAX_LOOP_LAYERS; ++layerIndex) {
        LooperWavePanel *wavePanel = wavePanels[layerIndex];
        wavePanel->setVisible(layerIndex < newMaxLayers);
    }
}

void LooperWindow::updateControls()
{
    if (looper && controller) {
        ui->buttonRec->setChecked(looper->isRecording() || looper->isWaiting());
        ui->buttonRec->setProperty("waiting", looper->isWaiting());
        ui->buttonRec->style()->unpolish(ui->buttonRec);
        ui->buttonRec->style()->polish(ui->buttonRec);
        ui->buttonRec->setEnabled(looper->canRecord());

        ui->buttonPlay->setChecked(looper->isPlaying());
        bool canEnablePlayButton = !looper->isWaiting() && !looper->isRecording();
        ui->buttonPlay->setEnabled(canEnablePlayButton);

        ui->comboBoxPlayMode->setEnabled(looper->isPlaying() || looper->isStopped());
        ui->labelPlayMode->setEnabled(ui->comboBoxPlayMode->isEnabled());

        ui->maxLayersSpinBox->setEnabled(looper->isStopped());
        ui->labelMaxLayers->setEnabled(ui->maxLayersSpinBox->isEnabled());

        const bool canShowRecordingProperties = looper->currentModeHasRecordingProperties();
        Gui::setLayoutItemsVisibility(ui->recordingPropertiesLayout, canShowRecordingProperties);
        if (canShowRecordingProperties) {
            bool canEnableHearLayerCheckBox = looper->getMaxLayers() > 1;
            ui->checkBoxHearLayersWhileRecording->setEnabled(canEnableHearLayerCheckBox);
            ui->checkBoxHearLayersWhileRecording->setChecked(looper->isHearingLayersWhileRecording());

            bool canShowOverdubCheckBox = looper->getMode() == Looper::SELECTED_LAYER;
            ui->checkBoxOverdub->setVisible(canShowOverdubCheckBox);
            ui->checkBoxOverdub->setEnabled(!looper->isRecording());
            ui->checkBoxOverdub->setChecked(looper->isOverdubbing());

            ui->labelRecording->setEnabled(ui->checkBoxOverdub->isEnabled() || ui->checkBoxHearLayersWhileRecording->isEnabled());
        }
    }

    update();
}

void LooperWindow::deleteWavePanels()
{
    for (quint8 key : wavePanels.keys())
        wavePanels[key]->deleteLater();

    wavePanels.clear();
}

LooperWindow::~LooperWindow()
{
    delete ui;

    deleteWavePanels();

    if (controller) {
        disconnect(controller, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeatsPerInterval);
        disconnect(controller, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeatsPerInterval);
        disconnect(controller, &NinjamController::intervalBeatChanged, this, &LooperWindow::updateCurrentBeat);
    }
}

void LooperWindow::updateCurrentBeat(uint currentIntervalBeat)
{
    if (!looper)
        return;

    for (LooperWavePanel *wavePanel : wavePanels.values()) {
        wavePanel->setCurrentBeat(currentIntervalBeat);
    }

    currentBeat = currentIntervalBeat;
}

void LooperWindow::updateBeatsPerInterval()
{
    if (!controller)
        return;

    uint samplesPerInterval = controller->getSamplesPerInterval();
    uint beatsPerInterval = controller->getCurrentBpi();

    for (LooperWavePanel *wavePanel : wavePanels.values()) {
        wavePanel->setBeatsPerInteval(beatsPerInterval, samplesPerInterval);
    }
}

void LooperWindow::initializeControls()
{
    // play modes combo
    ui->comboBoxPlayMode->clear();

    std::vector<Looper::Mode> playModes;
    playModes.push_back(Looper::SEQUENCE);
    playModes.push_back(Looper::ALL_LAYERS);
    playModes.push_back(Looper::SELECTED_LAYER);
    playModes.push_back(Looper::RANDOM_LAYERS);

    for (uint i = 0; i < playModes.size(); ++i) {
        Looper::Mode playMode = playModes[i];
        ui->comboBoxPlayMode->addItem(Looper::getModeString(playMode), qVariantFromValue(playMode));
    }

    // max layer spinbox
    ui->maxLayersSpinBox->setMinimum(1);
    ui->maxLayersSpinBox->setMaximum(Looper::MAX_LOOP_LAYERS);

    // wire signals/slots
    connect(ui->buttonRec, &QPushButton::clicked, [=]
    {
        if (looper)
            looper->toggleRecording();
    });

    connect(ui->buttonPlay, &QPushButton::clicked, [=]
    {
        if (looper)
            looper->togglePlay();
    });

    connect(ui->comboBoxPlayMode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index)
    {
        if (index >= 0 && looper) {
            looper->setMode(ui->comboBoxPlayMode->currentData().value<Looper::Mode>());
        }
    });

    connect(ui->maxLayersSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int newMaxLayers)
    {
        if (looper)
            looper->setLayers(newMaxLayers);
    });

    connect(ui->checkBoxHearLayersWhileRecording, &QCheckBox::toggled, [=](bool checked){
        if (looper)
            looper->setHearingOtherLayersWhileRecording(checked);
    });

    connect(ui->checkBoxOverdub, &QCheckBox::toggled, [=](bool checked){
        if (looper)
            looper->setOverdubbing(checked);
    });
}
