#include "LooperWindow.h"
#include "ui_LooperWindow.h"

#include <QVBoxLayout>

LooperWindow::LooperWindow(Audio::Looper *looper) :
    QDialog(nullptr),
    ui(new Ui::LooperWindow),
    looper(looper)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    setLayout(new QVBoxLayout());

    connect(looper, &Audio::Looper::bufferedSamplesPeakAvailable, this, &LooperWindow::drawNewBufferedSamplesPeak);
}

LooperWindow::~LooperWindow()
{
    delete ui;
}

void LooperWindow::drawNewBufferedSamplesPeak(float peak, quint8 layerIndex)
{
    WavePeakPanel *wavePanel = wavePanels[layerIndex];
    if (!wavePanel) {
        wavePanel = new WavePeakPanel();
        wavePanel->setDrawingMode(WavePeakPanel::SOUND_WAVE);
        wavePanels[layerIndex] = wavePanel;
        layout()->addWidget(wavePanel);
    }

    wavePanel->addPeak(peak);
}
