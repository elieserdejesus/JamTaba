#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"

#include <QVBoxLayout>

using namespace Controller;
using namespace Audio;

LooperWindow::LooperWindow(const QString &windowTitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LooperWindow),
    looper(nullptr),
    controller(nullptr),
    currentBeat(-1)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // remove help/question marker

    ui->setupUi(this);

    setWindowTitle(windowTitle);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(12);
    layout->setContentsMargins(0, 0, 0, 0);
    ui->layersWidget->setLayout(layout);

    // set as non resizable
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    setMinimumSize(size());
    setMaximumSize(size());
    setSizeGripEnabled(false);

    initializePlayModesComboBox();
    initializeStateControls();
}

void LooperWindow::initializeStateControls()
{
    connect(ui->buttonRec, &QPushButton::clicked, [=]{
        if (looper)
            looper->setState(Looper::WAITING);
    });

    connect(ui->buttonStop, &QPushButton::clicked, [=]{
        if (looper)
            looper->setState(Looper::STOPPED);
    });

    connect(ui->buttonPlay, &QPushButton::clicked, [=]{
        if (looper)
            looper->setState(Looper::PLAYING);
    });
}

void LooperWindow::paintEvent(QPaintEvent *ev)
{
    QDialog::paintEvent(ev);

    if (looper && looper->isWaiting()) {
        QPainter painter(this);

        static const QPen pen(QColor(0, 0, 0, 60), 1.0, Qt::DotLine);
        painter.setPen(pen);
        uint bpi = controller->getCurrentBpi();
        qreal pixelsPerBeat = (width()/static_cast<qreal>(bpi));
        QPointF rectTop = ui->layersWidget->mapToParent(ui->layersWidget->rect().topLeft());
        QSizeF rectSize = ui->layersWidget->size();
        for (uint beat = 0; beat < bpi; ++beat) {
            const qreal x = rectTop.x() + beat * pixelsPerBeat;
            painter.drawLine(QPointF(x, rectTop.y()), QPointF(x, rectTop.y() + rectSize.height()));
        }

        // draw a transparent red rect in current beat when waiting
        if (currentBeat) {
            static const QColor redColor(255, 0, 0, 25);
            qreal x = rectTop.x() + currentBeat * pixelsPerBeat;
            painter.fillRect(QRectF(x, rectTop.y(), pixelsPerBeat, rectSize.height()), redColor);

            uint waitBeats = controller->getCurrentBpi() - currentBeat;
            QString text = tr("wait (%1)").arg(QString::number(waitBeats));
            qreal textWidth = fontMetrics().width(text);
            painter.drawText(QPointF(rectTop.x() + rectSize.width()/2.0 - textWidth/2.0, rectTop.y() + rectSize.height()/2.0), text);
        }
    }
}

void LooperWindow::updateDrawings()
{
    quint8 currentLayer = looper->getCurrentLayerIndex();
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

        connect(controller, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::intervalBeatChanged, this, &LooperWindow::updateCurrentBeat);
        connect(looper, &Audio::Looper::stateChanged, this, &LooperWindow::updateStateControls);

        this->looper = looper;
        this->controller = controller;
    }

    updateBeatsPerInterval();
    updateStateControls();
}

void LooperWindow::updateStateControls()
{
    if (looper) {
        ui->buttonRec->setChecked(looper->isRecording() || looper->isWaiting());
        ui->buttonStop->setEnabled(!looper->isStopped());
        ui->buttonPlay->setEnabled(looper->isStopped());
    }
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

void LooperWindow::initializePlayModesComboBox()
{
    ui->comboBoxPlayMode->clear();

    std::vector<Looper::PlayMode> playModes;
    playModes.push_back(Looper::SEQUENCE);
    playModes.push_back(Looper::ALL_LAYERS);
    playModes.push_back(Looper::RANDOM_LAYERS);
    playModes.push_back(Looper::SELECTED_LAYER_ONLY);

    for (int i = 0; i < 4; ++i) {
        Looper::PlayMode playMode = playModes[i];
        ui->comboBoxPlayMode->addItem(Looper::getPlayModeString(playMode), playMode);
    }
}
