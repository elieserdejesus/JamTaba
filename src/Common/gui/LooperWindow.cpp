#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"

#include <QVBoxLayout>

using namespace Controller;

LooperWindow::LooperWindow(Audio::Looper *looper, Controller::NinjamController *controller, const QString &windowTitle) :
    QDialog(nullptr),
    ui(new Ui::LooperWindow),
    looper(looper),
    controller(controller)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle(windowTitle);

    setLayout(new QVBoxLayout());

    connect(looper, &Audio::Looper::bufferedSamplesPeakAvailable, this, &LooperWindow::addSamplesPeak);
    connect(controller, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeats);
    connect(controller, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeats);

    updateBeats(controller->getCurrentBpi());
}

LooperWindow::~LooperWindow()
{
    delete ui;
}

void LooperWindow::updateBeats(int newBpi)
{
    for (LooperWavePanel *wavePanel : wavePanels.values()) {
        uint samplesPerInterval = controller->getSamplesPerInterval();
        wavePanel->setBeatsPerInteval(newBpi, samplesPerInterval);
    }
}

void LooperWindow::addSamplesPeak(float peak, uint samplesCount, quint8 layerIndex)
{
    LooperWavePanel *wavePanel = wavePanels[layerIndex];
    if (!wavePanel) {
        uint currentBpi = controller->getCurrentBpi();
        uint samplesPerInterval = controller->getSamplesPerInterval();
        wavePanel = new LooperWavePanel(currentBpi, samplesPerInterval);
        wavePanels[layerIndex] = wavePanel;
        layout()->addWidget(wavePanel);
    }

    wavePanel->addPeak(peak, samplesCount);
}
