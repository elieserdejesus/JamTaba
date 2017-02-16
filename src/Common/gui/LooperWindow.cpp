#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"

#include <QVBoxLayout>

using namespace Controller;

LooperWindow::LooperWindow(const QString &windowTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LooperWindow),
    looper(nullptr),
    controller(nullptr)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // remove help/question marker

    ui->setupUi(this);

    setWindowTitle(windowTitle);

    setLayout(new QVBoxLayout());

    // set as non resizable
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumSize(size());
    setMaximumSize(size());
    setSizeGripEnabled(false);
}

void LooperWindow::detachCurrentLooper()
{
    deleteWavePanels();

    if (this->looper) {
        this->looper->disconnect();
        this->controller = nullptr;
    }
}

void LooperWindow::setLooper(Audio::Looper *looper, Controller::NinjamController *controller)
{
    Q_ASSERT(looper);
    Q_ASSERT(controller);

    if (looper != this->looper) { // check if user is not just reopening the looper editor
        detachCurrentLooper();

        connect(looper, &Audio::Looper::samplesPeakAvailable, this, &LooperWindow::addSamplesPeak);
        connect(controller, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::intervalBeatChanged, this, &LooperWindow::updateCurrentBeat);

        this->looper = looper;
        this->controller = controller;
    }

    updateBeatsPerInterval();
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

    if (looper) {
        looper->disconnect();
    }
}

void LooperWindow::updateCurrentBeat(uint currentIntervalBeat)
{
    if (!looper)
        return;

    quint8 currentLayer = looper->getCurrentLayerIndex();
    LooperWavePanel *wavePanel = wavePanels[currentLayer];
    if (wavePanel) {
        wavePanel->setCurrentIntervalBeat(currentIntervalBeat);
    }
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

void LooperWindow::addSamplesPeak(float peak, uint samplesCount, quint8 layerIndex)
{
    if (!controller || !looper)
        return;

    LooperWavePanel *wavePanel = wavePanels[layerIndex];
    if (!wavePanel) {
        uint currentBpi = controller->getCurrentBpi();
        uint samplesPerInterval = controller->getSamplesPerInterval();
        wavePanel = new LooperWavePanel(currentBpi, samplesPerInterval, looper);
        wavePanels[layerIndex] = wavePanel;
        layout()->addWidget(wavePanel);
    }

    wavePanel->addPeak(peak, samplesCount);
}
