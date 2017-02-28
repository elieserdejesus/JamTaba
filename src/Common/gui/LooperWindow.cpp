#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"
#include "gui/GuiUtils.h"

#include <QVBoxLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QKeyEvent>

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

    // this alignments are not available in QtCreator UI designer
    ui->widget->layout()->setAlignment(ui->modeControlsLayout, Qt::AlignBottom);
    ui->widget->layout()->setAlignment(ui->layerControlsLayout, Qt::AlignBottom);
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
        for (LooperWavePanel *wavePanel : wavePanels.values()) {
            if (wavePanel->isVisible())
                wavePanel->updateDrawings();
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
        quint8 currentLayers = looper->getLayers();
        for (quint8 layerIndex = 0; layerIndex < Audio::Looper::MAX_LOOP_LAYERS; ++layerIndex) {
            LooperWavePanel *wavePanel = new LooperWavePanel(looper, layerIndex);
            wavePanels.insert(layerIndex, wavePanel);
            ui->layersWidget->layout()->addWidget(wavePanel);
            wavePanel->setVisible(layerIndex < currentLayers);
        }

        // initial values
        ui->maxLayersSpinBox->setValue(looper->getLayers());

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

        QList<Looper::RecordingOption> recordingOptions = looper->getRecordingOptions();
        const bool canShowRecordingProperties = !recordingOptions.isEmpty();
        Gui::setLayoutItemsVisibility(ui->recordingPropertiesLayout, canShowRecordingProperties);
        ui->groupBoxRecording->setVisible(canShowRecordingProperties);
        if (canShowRecordingProperties) {
            createRecordingOptionsCheckBoxes();
        }

        QList<Looper::PlayingOption> playingOptions = looper->getPlayingOptions();
        const bool canShowPlayingProperties = !playingOptions.isEmpty();
        Gui::setLayoutItemsVisibility(ui->playingPropertiesLayout, canShowPlayingProperties);
        ui->groupBoxPlaying->setVisible(canShowPlayingProperties);
        if (canShowPlayingProperties) {
            createPlayingOptionsCheckBoxes();
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
}

QString LooperWindow::getOptionName(Looper::RecordingOption option)
{
    switch (option) {
    case Looper::HearAllLayers:   return tr("Hear all");
    case Looper::Overdub:           return tr("Overdub");
    }

    return "Error!";
}

QString LooperWindow::getOptionName(Looper::PlayingOption option)
{
    switch (option) {
    case Looper::RandomizeLayers:       return tr("Random");
    case Looper::PlayLockedLayers:  return tr("Locked");
    }

    return "Error!";
}

QString LooperWindow::getOptionToolTip(Audio::Looper::RecordingOption option)
{
    switch (option) {
    case Looper::HearAllLayers:   return tr("Hear all layers while recording");
    case Looper::Overdub:               return tr("Overdub the current layer until REC button is pressed");
    }

    return QString();
}

QString LooperWindow::getOptionToolTip(Audio::Looper::PlayingOption option)
{
    switch (option) {
    case Looper::RandomizeLayers:       return tr("Randomize layers while playing");
    case Looper::PlayLockedLayers:  return tr("Play locked layers only");
    }

    return QString();
}

void LooperWindow::createRecordingOptionsCheckBoxes()
{
    clearLayout(ui->recordingPropertiesLayout);

    QList<Looper::RecordingOption> recordingOptions = looper->getRecordingOptions();
    for (const Looper::RecordingOption &option : recordingOptions) {
        QCheckBox *newCheckBox = new QCheckBox(getOptionName(option));
        newCheckBox->setToolTip(getOptionToolTip(option));
        newCheckBox->setChecked(looper->getRecordingOption(option));

        ui->recordingPropertiesLayout->addWidget(newCheckBox);

        connect(newCheckBox, &QCheckBox::toggled, [=](bool checked){
            if (looper)
                looper->setRecordingOption(option, checked);
        });
    }
}

void LooperWindow::createPlayingOptionsCheckBoxes()
{
    clearLayout(ui->playingPropertiesLayout);

    QList<Looper::PlayingOption> playingOptions = looper->getPlayingOptions();
    for (const Looper::PlayingOption &option : playingOptions) {
        QCheckBox *newCheckBox = new QCheckBox(getOptionName(option));
        newCheckBox->setToolTip(getOptionToolTip(option));
        newCheckBox->setChecked(looper->getPlayingOption(option));

        ui->playingPropertiesLayout->addWidget(newCheckBox);

        connect(newCheckBox, &QCheckBox::toggled, [=](bool checked){
            if (looper)
                looper->setPlayingOption(option, checked);
        });
    }
}

void LooperWindow::clearLayout(QLayout *layout)
{
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget())
                item->widget()->deleteLater();
            delete item;
        }
    }
}
