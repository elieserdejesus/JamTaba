#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"
#include "gui/GuiUtils.h"
#include "Utils.h"

#include <QGridLayout>
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

    QGridLayout *layout = new QGridLayout();
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
    ui->widgetBottom->layout()->setAlignment(ui->modeControlsLayout, Qt::AlignBottom);
    ui->widgetBottom->layout()->setAlignment(ui->layerControlsLayout, Qt::AlignBottom);
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
        QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->layersWidget->layout());
        for (quint8 layerIndex = 0; layerIndex < Audio::Looper::MAX_LOOP_LAYERS; ++layerIndex) {
            LooperWavePanel *wavePanel = new LooperWavePanel(looper, layerIndex);
            wavePanels.insert(layerIndex, wavePanel);
            gridLayout->addWidget(wavePanel, layerIndex, 0);
            wavePanel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
            gridLayout->addLayout(createLayerControls(looper, layerIndex), layerIndex, 1);
            wavePanel->setVisible(layerIndex < currentLayers);
        }
        gridLayout->setColumnStretch(0, 1);

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

QLayout *LooperWindow::createLayerControls(Looper *looper, quint8 layerIndex)
{
    QLayout *levelFaderLayout = new QHBoxLayout();
    levelFaderLayout->setSpacing(2);
    levelFaderLayout->setContentsMargins(0, 0, 0, 0);

    QSlider *levelSlider = new QSlider();
    levelSlider->setObjectName(QStringLiteral("levelSlider"));
    levelSlider->setOrientation(Qt::Horizontal);
    levelSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    levelSlider->setMaximum(12);
    levelSlider->setValue(10);
    levelSlider->setTickPosition(QSlider::NoTicks);
    levelSlider->setMaximumWidth(80);
    levelSlider->installEventFilter(this);

    connect(levelSlider, &QSlider::valueChanged, [looper, layerIndex](int value){
        float gain = Utils::linearGainToPower(value/10.0);
        looper->setLayerGain(layerIndex, gain);
    });

    QLabel *highLevelIcon = new QLabel();
    QLabel *lowLevelIcon = new QLabel();
    highLevelIcon->setPixmap(QPixmap(":/images/level high.png"));
    lowLevelIcon->setPixmap(QPixmap(":/images/level low.png"));
    highLevelIcon->setAlignment(Qt::AlignCenter);
    lowLevelIcon->setAlignment(Qt::AlignCenter);

    levelFaderLayout->addWidget(lowLevelIcon);
    levelFaderLayout->addWidget(levelSlider);
    levelFaderLayout->addWidget(highLevelIcon);

    QLayout *panFaderLayout = new QHBoxLayout();
    panFaderLayout->setSpacing(0);
    panFaderLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *labelPanL = new QLabel(QStringLiteral("L"));
    labelPanL->setObjectName(QStringLiteral("labelPanL"));

    QLabel *labelPanR = new QLabel(QStringLiteral("R"));
    labelPanR->setObjectName(QStringLiteral("labelPanR"));

    QSlider *panSlider = new QSlider();
    panSlider->setObjectName(QStringLiteral("panSlider"));
    panSlider->setMinimum(-4);
    panSlider->setMaximum(4);
    panSlider->setOrientation(Qt::Horizontal);
    panSlider->setMaximumWidth(50);
    panSlider->installEventFilter(this);
    connect(panSlider, &QSlider::valueChanged, [looper, layerIndex, panSlider](int value){
        float panValue = value/(float)panSlider->maximum();
        looper->setLayerPan(layerIndex, panValue);

    });

    panFaderLayout->addWidget(labelPanL);
    panFaderLayout->addWidget(panSlider);
    panFaderLayout->addWidget(labelPanR);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(6);

    mainLayout->addLayout(levelFaderLayout);
    mainLayout->addLayout(panFaderLayout);

    return mainLayout;
}

// event filter used to handle double clicks
bool LooperWindow::eventFilter(QObject *source, QEvent *ev)
{
    if (ev->type() == QEvent::MouseButtonDblClick) {
        QSlider *slider = qobject_cast<QSlider *>(source);
        if (slider) {
            if (slider->objectName() == "levelSlider")
                 slider->setValue(10); // set level fader to unit gain
            else
                 if (slider->objectName() == "panSlider")
                    slider->setValue(0);// set pan slider to center
        }

        return true;
    }

    return QDialog::eventFilter(source, ev);
}

void LooperWindow::resetAllLayersControls()
{
    QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->layersWidget->layout());
    for (quint8 layerIndex = 0; layerIndex < looper->getLayers(); ++layerIndex) {
        QBoxLayout *layerControlsLayout = qobject_cast<QBoxLayout *>(gridLayout->itemAtPosition(layerIndex, 1)->layout());

        QBoxLayout *faderLayout = qobject_cast<QBoxLayout *>(layerControlsLayout->itemAt(0)->layout());
        QBoxLayout *panLayout = qobject_cast<QBoxLayout *>(layerControlsLayout->itemAt(1)->layout());

        QSlider *faderSlider = qobject_cast<QSlider *>(faderLayout->itemAt(1)->widget());
        QSlider *panSlider = qobject_cast<QSlider *>(panLayout->itemAt(1)->widget());

        panSlider->setValue(0); // center
        faderSlider->setValue(10); // 100%, unit gain
    }
}

void LooperWindow::updateLayersVisibility(quint8 newMaxLayers)
{
    QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->layersWidget->layout());
    for (quint8 layerIndex = 0; layerIndex < Audio::Looper::MAX_LOOP_LAYERS; ++layerIndex) {
        LooperWavePanel *wavePanel = wavePanels[layerIndex];
        bool layerIsVisible = layerIndex < newMaxLayers;
        wavePanel->setVisible(layerIsVisible);
        QBoxLayout *layerControlsLayout = qobject_cast<QBoxLayout *>(gridLayout->itemAtPosition(layerIndex, 1)->layout());
        QBoxLayout *faderLayout = qobject_cast<QBoxLayout *>(layerControlsLayout->itemAt(0)->layout());
        layerControlsLayout->setDirection(newMaxLayers > 2 ? QBoxLayout::LeftToRight: QBoxLayout::BottomToTop);
        faderLayout->setDirection(newMaxLayers > 2 ? QBoxLayout::LeftToRight : QBoxLayout::BottomToTop);
        QSlider *faderSlider = qobject_cast<QSlider *>(faderLayout->itemAt(1)->widget());
        faderSlider->setOrientation(newMaxLayers > 2 ? Qt::Horizontal : Qt::Vertical);
        Gui::setLayoutItemsVisibility(layerControlsLayout, layerIsVisible);
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

        // update playing and recording options
        updateOptions<Looper::PlayingOption>(ui->groupBoxPlaying->layout());
        updateOptions<Looper::RecordingOption>(ui->groupBoxRecording->layout());

        // update more specific checkboxes
        QCheckBox *lockedCheckBox = playingCheckBoxes[Looper::PlayLockedLayers];
        if (lockedCheckBox) {
            lockedCheckBox->setEnabled(lockedCheckBox->isEnabled() && looper->hasLockedLayers());
            lockedCheckBox->setChecked(lockedCheckBox->isEnabled() && lockedCheckBox->isChecked());
        }

        // update locked layers check box
        QCheckBox *randomLayersCheckBox = playingCheckBoxes[Looper::RandomizeLayers];
        if (randomLayersCheckBox) {
            bool canEnableRandom = looper->getLayers() > 1;
            if (looper->getOption(Looper::PlayLockedLayers)) // is playing locked only
                canEnableRandom = canEnableRandom && looper->hasLockedLayers();

            randomLayersCheckBox->setEnabled(randomLayersCheckBox->isEnabled() && canEnableRandom);
            randomLayersCheckBox->setChecked(randomLayersCheckBox->isEnabled() && randomLayersCheckBox->isChecked());
        }

        // update 'hear all' checkbox (in ALL layers mode this checkbox will be checked AND disabled)
        if (looper->getMode() == Looper::ALL_LAYERS) {
            QCheckBox *hearAllCheckBox = recordingCheckBoxes[Looper::HearAllLayers];
            if (hearAllCheckBox) {
                hearAllCheckBox->setChecked(true);
                hearAllCheckBox->setEnabled(false);
            }
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

    createPlayingOptionsCheckBoxes();
    createRecordingOptionsCheckBoxes();

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

    connect(ui->resetButton, &QPushButton::clicked, [=](){
        if (looper) {
            looper->reset();
            resetAllLayersControls();
        }
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
    case Looper::PlayLockedLayers:      return tr("Locked");
    case Looper::PlayNonEmptyLayers:    return tr("Non empty");
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
    createOptionsCheckBoxes(ui->recordingPropertiesLayout, getAllRecordingOptions(), recordingCheckBoxes);
}

void LooperWindow::createPlayingOptionsCheckBoxes()
{
    createOptionsCheckBoxes(ui->playingPropertiesLayout, getAllPlayingOptions(), playingCheckBoxes);
}

QList<Audio::Looper::RecordingOption> LooperWindow::getAllRecordingOptions()
{
    QList<Audio::Looper::RecordingOption> options;
    options << Looper::Overdub;
    options << Looper::HearAllLayers;

    return options;
}

QList<Audio::Looper::PlayingOption> LooperWindow::getAllPlayingOptions()
{
    QList<Audio::Looper::PlayingOption> options;
    options << Looper::RandomizeLayers;
    options << Looper::PlayLockedLayers;
    // options << Looper::PlayNonEmptyLayers; // no implemented yet

    return options;
}

void LooperWindow::clearLayout(QLayout *layout)
{
    if (layout) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }
}
