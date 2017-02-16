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

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(12);
    setLayout(layout);

    // set as non resizable
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumSize(size());
    setMaximumSize(size());
    setSizeGripEnabled(false);
}

void LooperWindow::paintEvent(QPaintEvent *ev)
{
    QDialog::paintEvent(ev);

    if (looper && looper->isWaiting()) {
        uint waitBeats = controller->getCurrentBpi() - currentBeat;
        QString text = tr("wait (%1)").arg(QString::number(waitBeats));
        uint textWidth = fontMetrics().width(text);
        QPainter painter(this);
        painter.drawText(width()/2 - textWidth/2, height()/2, text);
    }
}

void LooperWindow::updateDrawings()
{
    uint currentLayer = looper->getCurrentLayerIndex();
    LooperWavePanel *wavePanel = wavePanels[currentLayer];
    if (wavePanel)
        wavePanel->updateDrawings();
    else
        qCritical() << "wave panel null wavePanel keys:" << wavePanels.keys() << " currentLayer:" << currentLayer;

    update();
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

        qDebug() << "Setting looper in LooperWindow";

        deleteWavePanels();

        detachCurrentLooper();

        // create wave panels (layers view)
        for (quint8 layerIndex = 0; layerIndex < Audio::Looper::MAX_LOOP_LAYERS; ++layerIndex) {
            LooperWavePanel *wavePanel = new LooperWavePanel(looper, layerIndex);
            wavePanels.insert(layerIndex, wavePanel);
            layout()->addWidget(wavePanel);
        }

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
}

void LooperWindow::updateCurrentBeat(uint currentIntervalBeat)
{
    if (!looper)
        return;

    quint8 currentLayer = looper->getCurrentLayerIndex();
    LooperWavePanel *wavePanel = wavePanels[currentLayer];
    if (wavePanel) {
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
