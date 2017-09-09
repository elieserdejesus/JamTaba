#include "LooperWindow.h"
#include "ui_LooperWindow.h"
#include "NinjamController.h"
#include "gui/GuiUtils.h"
#include "Utils.h"
#include "MainController.h"
#include "persistence/Settings.h"
#include "file/FileUtils.h"
#include "log/Logging.h"

#include <QGridLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QKeyEvent>
#include <QInputDialog>
#include <QMenu>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

using namespace Controller;
using namespace Audio;

LooperWindow::LooperWindow(QWidget *parent, Controller::MainController *mainController) :
    QDialog(parent),
    ui(new Ui::LooperWindow),
    mainController(mainController),
    looper(nullptr),
    currentBeat(-1)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // remove help/question marker

#ifdef Q_OS_MAC
    setWindowFlags(windowFlags() | Qt::Popup);
#endif

    ui->setupUi(this);

    ui->mainLevelSlider->setSliderType(Slider::AudioSlider);

    QGridLayout *layout = new QGridLayout();
    layout->setSpacing(12);
    layout->setContentsMargins(6, 6, 6, 6);
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


    QMenu *loadMenu = new QMenu();
    ui->loadButton->setMenu(loadMenu);
    connect(loadMenu, &QMenu::aboutToShow, this, &LooperWindow::showLoadMenu);

    ui->peakMeter->setOrientation(Qt::Vertical);

    connect(ui->mainLevelSlider, &QSlider::valueChanged, [=](int value){
        if (!looper)
            return;

        float gain = Utils::linearGainToPower(value/100.0);
        looper->setMainGain(gain);
    });
}

void LooperWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::LanguageChange) {

        // translate strings typed directly in LooperWindow.ui file
        ui->retranslateUi(this);

        // translate option checkboxes strings
        translateOptions<Looper::PlayingOption>(ui->groupBoxPlaying->layout());
        translateOptions<Looper::RecordingOption>(ui->groupBoxRecording->layout());

        // translate mode combo box
        for (int i = 0; i < ui->comboBoxPlayMode->count(); ++i) {
            Looper::Mode playMode = static_cast<Looper::Mode>(ui->comboBoxPlayMode->itemData(i).toInt());
            ui->comboBoxPlayMode->setItemText(i, Looper::getModeString(playMode));
        }

        // translate layers pan labels (L & R)
        for (uint l = 0; l < MAX_LOOP_LAYERS; ++l) {
            if (layerViews.contains(l)) {
                layerViews[l].controlsLayout->labelPanL->setText(tr("L"));
                layerViews[l].controlsLayout->labelPanR->setText(tr("R"));
            }
        }
    }

    QDialog::changeEvent(ev);
}

void LooperWindow::showResetMenu()
{
    QMenu *resetMenu = ui->resetButton->menu();
    resetMenu->clear();
    resetMenu->addAction(tr("Reset layers content"), looper, SLOT(resetLayersContent()));
    resetMenu->addAction(tr("Reset layers controls"), this, SLOT(resetLayersControls()));
    resetMenu->addAction(tr("Reset layers content and controls"), this, SLOT(resetAll()));
}

void LooperWindow::keyPressEvent(QKeyEvent *ev)
{
    if (!looper)
        return;

    int key = ev->key();
    bool pressingNumberKey = key >= Qt::Key_1 && key <= Qt::Key_9;
    if (pressingNumberKey) {
        quint8 layerIndex = (key - Qt::Key_0) - 1; // convert key to zero based layer index
        looper->selectLayer(layerIndex);
    }
    else {
        bool pressingDelete = key == Qt::Key_Delete;
        if (pressingDelete) {
            looper->clearCurrentLayer();
        }
    }
}

void LooperWindow::paintEvent(QPaintEvent *ev)
{
    QDialog::paintEvent(ev);

    auto ninjamController = mainController->getNinjamController();
    if (!looper || !ninjamController)
        return;

    if (looper->isWaitingToRecord()) {

        QPainter painter(this);

        static const QPen pen(QColor(0, 0, 0, 60), 1.0, Qt::DotLine);
        painter.setPen(pen);
        uint bpi = ninjamController->getCurrentBpi();
        LooperWavePanel *wavePanel = layerViews[looper->getCurrentLayerIndex()].wavePanel;
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

            const uint waitBeats = ninjamController->getCurrentBpi() - currentBeat;
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
    if (!looper)
        return;

    if (!looper->isWaitingToRecord()) {
        for (const LayerView &layerView : layerViews.values()) {
            LooperWavePanel *wavePanel = layerView.wavePanel;
            if (wavePanel->isVisible())
                wavePanel->updateDrawings();
        }
    }
    else {
        update(); // paint vertical lines, current beat and wait count
    }

    // update peak meters
    AudioPeak lastPeak = looper->getLastPeak();
    ui->peakMeter->setPeak(lastPeak.getLeftPeak(), lastPeak.getRightPeak(),
                                lastPeak.getLeftRMS(), lastPeak.getRightRMS());
}

void LooperWindow::detachCurrentLooper()
{
    if (looper) {
        disconnectLooperSignals();
        looper = nullptr;
        mainController = nullptr;
    }
}

void LooperWindow::setLooper(Audio::Looper *looper)
{
    Q_ASSERT(looper);
    Q_ASSERT(mainController);

    auto controller = mainController->getNinjamController();
    Q_ASSERT(controller);

    if (looper != this->looper) { // check if user is not just reopening the looper editor

        if (this->looper && this->looper->isRecording()) {
            this->looper->stop();
        }

        deleteWavePanels();

        detachCurrentLooper();

        this->looper = looper;

        QMenu *resetMenu = new QMenu();
        connect(resetMenu, &QMenu::aboutToShow, this, &LooperWindow::showResetMenu);
        ui->resetButton->setMenu(resetMenu);

        // create wave panels and layer controls (layers view)
        quint8 currentLayers = looper->getLayers();
        QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->layersWidget->layout());
        for (quint8 layerIndex = 0; layerIndex < MAX_LOOP_LAYERS; ++layerIndex) {
            auto layerWavePanel = new LooperWavePanel(looper, layerIndex);
            auto layerControlsLayout = new LooperWindow::LayerControlsLayout(looper, layerIndex);

            layerWavePanel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));

            gridLayout->addWidget(layerWavePanel, layerIndex, 0);
            gridLayout->addLayout(layerControlsLayout, layerIndex, 1);

            connect(layerWavePanel, &LooperWavePanel::audioFilesDropped, this, &LooperWindow::loadAudioFilesIntoLayer);

            layerViews.insert(layerIndex, LayerView(layerWavePanel, layerControlsLayout));
        }
        gridLayout->setColumnStretch(0, 1);
        updateLayersVisibility(currentLayers);

        // initial values
        ui->maxLayersComboBox->setCurrentText(QString::number(looper->getLayers()));

        ui->loopNameLabel->setText(looper->getLoopName());

        updateModeComboBox();

        connect(controller, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeatsPerInterval);
        connect(controller, &NinjamController::intervalBeatChanged, this, &LooperWindow::updateCurrentBeat);

        connectLooperSignals();

        connect(playingCheckBoxes[Looper::PlayLockedLayers], &QCheckBox::clicked, this, &LooperWindow::updateControls);
        connect(recordingCheckBoxes[Looper::HearAllLayers], &QCheckBox::clicked, this, &LooperWindow::updateControls);
    }

    updateBeatsPerInterval();
    handleNewMaxLayers(looper->getLayers());
    updateControls();
}

void LooperWindow::connectLooperSignals()
{
    connect(looper, &Looper::stateChanged, this, &LooperWindow::updateControls);
    connect(looper, &Looper::layerChanged, this, &LooperWindow::updateControls);
    connect(looper, &Looper::maxLayersChanged, this, &LooperWindow::handleNewMaxLayers);
    connect(looper, &Looper::modeChanged, this, &LooperWindow::handleModeChanged);
    connect(looper, &Looper::currentLayerChanged, this, &LooperWindow::updateControls);
    connect(looper, &Looper::destroyed, this, &LooperWindow::close);
    connect(looper, &Looper::layerMuteStateChanged, this, &LooperWindow::handleLayerMuteStateChanged);
    connect(looper, &Looper::layersContentErased, this, &LooperWindow::updateControls);
    connect(looper, &Looper::currentLoopNameChanged, ui->loopNameLabel, &QLabel::setText);
}

void LooperWindow::disconnectLooperSignals()
{
    disconnect(looper, &Looper::stateChanged, this, &LooperWindow::updateControls);
    disconnect(looper, &Looper::layerChanged, this, &LooperWindow::updateControls);
    disconnect(looper, &Looper::maxLayersChanged, this, &LooperWindow::handleNewMaxLayers);
    disconnect(looper, &Looper::modeChanged, this, &LooperWindow::handleModeChanged);
    disconnect(looper, &Looper::currentLayerChanged, this, &LooperWindow::updateControls);
    disconnect(looper, &Looper::destroyed, this, &LooperWindow::close);
    disconnect(looper, &Looper::layerMuteStateChanged, this, &LooperWindow::handleLayerMuteStateChanged);
    disconnect(looper, &Looper::layersContentErased, this, &LooperWindow::updateControls);
    disconnect(looper, &Looper::currentLoopNameChanged, ui->loopNameLabel, &QLabel::setText);
}

void LooperWindow::handleLayerMuteStateChanged(quint8 layer, quint8 state)
{
    if (looper->getMode() != Looper::AllLayers)
        return;

    auto muteButton = layerViews[layer].controlsLayout->muteButton;
    bool waiting = state == LooperLayer::WaitingToMute || state == LooperLayer::WaitingToUnmute;

    bool muted = state == LooperLayer::Muted;
    muteButton->setCheckable(muted);
    muteButton->setChecked(muted);

    if (waiting)
        muteButton->startBlink();
    else
        muteButton->stopBlink();
}

void LooperWindow::updateModeComboBox()
{
    QString selectedMode = looper->getModeString(looper->getMode());
    QSignalBlocker blocker(ui->comboBoxPlayMode);
    for (int i = 0; i < ui->comboBoxPlayMode->count(); ++i) {
        if (ui->comboBoxPlayMode->itemText(i) == selectedMode) {
            ui->comboBoxPlayMode->setCurrentIndex(i);
            break;
        }
    }
}

void LooperWindow::handleModeChanged()
{
    updateControls();

    Q_ASSERT(mainController);
    Q_ASSERT(looper);
    mainController->storeLooperPreferredMode(static_cast<quint8>(looper->getMode()));
}

void LooperWindow::handleNewMaxLayers(quint8 newMaxLayers)
{
    updateControls();
    updateLayersVisibility(newMaxLayers);

    const static int minHeight = 150; // one layer
    const static int maxHeight = 600; // 8 layers
    const static int range = maxHeight - minHeight;
    int newHeight = static_cast<float>(newMaxLayers)/MAX_LOOP_LAYERS * range + minHeight;
    setMinimumHeight(newHeight);
    setMaximumHeight(newHeight);

    Q_ASSERT(mainController);
    mainController->storeLooperPreferredLayerCount(newMaxLayers);
}

void LooperWindow::LayerControlsLayout::enableMuteButton(bool enabled)
{
    muteButton->setEnabled(enabled);
}

void LooperWindow::LayerControlsLayout::setMuteButtonVisibility(bool show)
{
    if (show) {
        addWidget(muteButton);
        muteButton->show();
    }
    else{
        removeWidget(muteButton);
        muteButton->setChecked(false);
        muteButton->hide();
    }
}

LooperWindow::LayerControlsLayout::LayerControlsLayout(Looper *looper, quint8 layerIndex)
    : QHBoxLayout()
{
    const int mainLayoutSpacing = 12;
    setSpacing(mainLayoutSpacing);

    QLayout *levelFaderLayout = new QHBoxLayout();
    levelFaderLayout->setSpacing(2);
    levelFaderLayout->setContentsMargins(0, 0, 0, 0);

    gainSlider = new Slider();
    gainSlider->setObjectName(QStringLiteral("levelSlider"));
    gainSlider->setOrientation(Qt::Horizontal);
    gainSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    gainSlider->setMaximum(120);
    gainSlider->setValue(Utils::poweredGainToLinear(looper->getLayerGain(layerIndex)) * 100);
    gainSlider->setTickPosition(QSlider::NoTicks);
    gainSlider->setMaximumWidth(80);
    gainSlider->setSliderType(Slider::AudioSlider);

    connect(gainSlider, &QSlider::valueChanged, [looper, layerIndex](int value){
        float gain = Utils::linearGainToPower(value/100.0);
        looper->setLayerGain(layerIndex, gain);
    });

    QLabel *highLevelIcon = new QLabel();
    QLabel *lowLevelIcon = new QLabel();
    highLevelIcon->setPixmap(QPixmap(":/images/level high.png"));
    lowLevelIcon->setPixmap(QPixmap(":/images/level low.png"));
    highLevelIcon->setAlignment(Qt::AlignCenter);
    lowLevelIcon->setAlignment(Qt::AlignCenter);

    levelFaderLayout->addWidget(lowLevelIcon);
    levelFaderLayout->addWidget(gainSlider);
    levelFaderLayout->addWidget(highLevelIcon);

    QHBoxLayout *panFaderLayout = new QHBoxLayout();
    panFaderLayout->setSpacing(0);
    panFaderLayout->setContentsMargins(0, 0, 0, 0);

    labelPanL = new QLabel(tr("L"));
    labelPanL->setObjectName(QStringLiteral("labelPanL"));

    labelPanR = new QLabel(tr("R"));
    labelPanR->setObjectName(QStringLiteral("labelPanR"));

    panSlider = new Slider();
    panSlider->setObjectName(QStringLiteral("panSlider"));
    panSlider->setMinimum(-4);
    panSlider->setMaximum(4);
    panSlider->setOrientation(Qt::Horizontal);
    panSlider->setMaximumWidth(50);
    panSlider->setValue(looper->getLayerPan(layerIndex) * panSlider->maximum());
    panSlider->setSliderType(Slider::PanSlider);

    connect(panSlider, &QSlider::valueChanged, [looper, layerIndex, this](int value){
        float panValue = value/(float)panSlider->maximum();
        looper->setLayerPan(layerIndex, panValue);

    });

    muteButton = new BlinkableButton(QStringLiteral("M"));
    muteButton->setObjectName("muteButton");
    connect(muteButton, &BlinkableButton::clicked, [looper, layerIndex](){
        looper->nextMuteState(layerIndex);
    });

    panFaderLayout->addWidget(labelPanL);
    panFaderLayout->addWidget(panSlider);
    panFaderLayout->addWidget(labelPanR);

    addLayout(levelFaderLayout);
    addLayout(panFaderLayout);
}

void LooperWindow::resetAll()
{
    looper->resetLayersContent();
    resetLayersControls();

    looper->setLoopName("");
}

void LooperWindow::resetLayersControls()
{
    for (quint8 layerIndex = 0; layerIndex < looper->getLayers(); ++layerIndex) {
        auto view = layerViews[layerIndex];
        QSlider *faderSlider = view.controlsLayout->gainSlider;
        QSlider *panSlider = view.controlsLayout->panSlider;
        QPushButton *muteButton = view.controlsLayout->muteButton;

        panSlider->setValue(0); // center
        faderSlider->setValue(100); // 100%, unit gain
        muteButton->setChecked(false); // unmuted
    }

    setMaxLayerComboBoxValuesAvailability(0);
}

void LooperWindow::updateLayersVisibility(quint8 newMaxLayers)
{
    for (quint8 layerIndex = 0; layerIndex < MAX_LOOP_LAYERS; ++layerIndex) {
        LooperWavePanel *wavePanel = layerViews[layerIndex].wavePanel;
        bool layerIsVisible = layerIndex < newMaxLayers;
        wavePanel->setVisible(layerIsVisible);
        Gui::setLayoutItemsVisibility(layerViews[layerIndex].controlsLayout, layerIsVisible);
    }
}

void LooperWindow::updateControls()
{
    if (looper) {
        ui->buttonRec->setChecked(looper->isRecording() || looper->isWaitingToRecord());
        ui->buttonRec->setEnabled(looper->canRecord());

        ui->buttonPlay->setChecked(looper->isPlaying());
        bool canEnablePlayButton = !looper->isWaitingToRecord() && !looper->isRecording();
        ui->buttonPlay->setEnabled(canEnablePlayButton);

        ui->comboBoxPlayMode->setEnabled(looper->isPlaying() || looper->isStopped());
        ui->labelPlayMode->setEnabled(ui->comboBoxPlayMode->isEnabled());

        updateMaxLayersControls();

        ui->saveButton->setEnabled(looper->canSave());
        ui->loadButton->setEnabled(looper->isStopped());

        ui->resetButton->setEnabled(looper->isStopped() || looper->isPlaying());

        // update playing and recording options
        updateOptions<Looper::PlayingOption>(ui->groupBoxPlaying->layout());
        updateOptions<Looper::RecordingOption>(ui->groupBoxRecording->layout());

        // update locked layers check box
        QCheckBox *lockedCheckBox = playingCheckBoxes[Looper::PlayLockedLayers];
        if (lockedCheckBox) {
            bool canEnable = lockedCheckBox->isEnabled() && looper->hasLockedLayers();
            if (looper->getMode() == Looper::SelectedLayer) {
                QCheckBox *hearAllCheckBox = recordingCheckBoxes[Looper::HearAllLayers];
                if (hearAllCheckBox) {
                    canEnable &= hearAllCheckBox->isEnabled() && hearAllCheckBox->isChecked();
                }
            }
            lockedCheckBox->setEnabled(canEnable);
            lockedCheckBox->setChecked(canEnable && lockedCheckBox->isChecked());
        }

        // update random layer check box
        QCheckBox *randomLayersCheckBox = playingCheckBoxes[Looper::RandomizeLayers];
        if (randomLayersCheckBox) {
            bool canEnableRandom = looper->getLayers() > 1;
            if (looper->getOption(Looper::PlayLockedLayers)) // is playing locked only
                canEnableRandom = canEnableRandom && looper->hasLockedLayers();

            randomLayersCheckBox->setEnabled(randomLayersCheckBox->isEnabled() && canEnableRandom);
            randomLayersCheckBox->setChecked(randomLayersCheckBox->isEnabled() && randomLayersCheckBox->isChecked());
        }

        // update 'hear all' checkbox (in ALL layers mode this checkbox will be checked AND disabled)
        if (looper->getMode() == Looper::AllLayers) {
            QCheckBox *hearAllCheckBox = recordingCheckBoxes[Looper::HearAllLayers];
            if (hearAllCheckBox) {
                hearAllCheckBox->setChecked(true);
                hearAllCheckBox->setEnabled(false);
            }
        }

        // update looper name
        QString currentLooperName = ui->loopNameLabel->text();
        if (looper->isChanged() && !currentLooperName.isEmpty() && !currentLooperName.endsWith("*"))
            ui->loopNameLabel->setText(currentLooperName + "*");

        updateButtons();

        // main level
        float mainGain = looper->getMainGain();
        ui->mainLevelSlider->setValue(Utils::poweredGainToLinear(mainGain) * 100);
    }

    update();
}

void LooperWindow::updateButtons()
{
    // update mute buttons
    const bool canShowMuteButtons = looper->getMode() == Looper::AllLayers;
    for (uint l = 0; l < looper->getLayers(); ++l) {
        auto layerView = layerViews[l];

        // disable/hide mute button if looper mode is not AllLayers
        layerView.controlsLayout->setMuteButtonVisibility(canShowMuteButtons);

        // disable mute buttons for non-locked layers
        bool canDisableMuteButton = looper->getOption(Looper::PlayLockedLayers) && !looper->layerIsLocked(l);
        layerView.controlsLayout->enableMuteButton(!canDisableMuteButton);

        auto *muteButton = layerView.controlsLayout->muteButton;
        if (muteButton->isBlinking() && !looper->isPlaying()) {
            looper->nextMuteState(l);
            muteButton->stopBlink();
        }
    }

    // update play button
    if (!looper->isWaitingToStopInNextInterval()) {
        ui->buttonPlay->stopBlink();
    }

    // update rec button
    if (!looper->isWaitingToRecord())
        ui->buttonRec->stopBlink();
}

void LooperWindow::setMaxLayerComboBoxValuesAvailability(int valuesToDisable)
{
    // disable the values before last valid (non empty) layers
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->maxLayersComboBox->model());
    const QColor disabledColor = ui->maxLayersComboBox->palette().color(QPalette::Disabled, QPalette::Text);
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        QStandardItem* item = model->item(l);
        bool disable = l < valuesToDisable;
        item->setFlags(disable ? item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled) : Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        // visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
        item->setData(disable ? disabledColor : QVariant(), // clear item data in order to use default color
                                Qt::TextColorRole);
    }
}

void LooperWindow::updateMaxLayersControls()
{
    ui->maxLayersComboBox->setEnabled(looper->isStopped() || looper->isPlaying());
    ui->labelMaxLayers->setEnabled(ui->maxLayersComboBox->isEnabled());
    int currentMaxLayersValue = (ui->maxLayersComboBox->currentIndex() + 1);
    if (currentMaxLayersValue != looper->getLayers()) {
        QSignalBlocker signalBlocker(ui->maxLayersComboBox);
        ui->maxLayersComboBox->setCurrentIndex(looper->getLayers() - 1);
    }

    int lastValidLayerIndex = looper->getLastValidLayer();
    uint layersToDisable = 0;
    if (lastValidLayerIndex >= 0)
        layersToDisable = lastValidLayerIndex;

    setMaxLayerComboBoxValuesAvailability(layersToDisable);
}

void LooperWindow::deleteWavePanels()
{
    for (quint8 key : layerViews.keys()) {
        layerViews[key].wavePanel->deleteLater();
        layerViews[key].controlsLayout->deleteLater();
    }

    layerViews.clear();
}

LooperWindow::~LooperWindow()
{
    delete ui;

    deleteWavePanels();

    if (mainController) {
        auto ninjamController = mainController->getNinjamController();
        if (ninjamController) {
            disconnect(ninjamController, &NinjamController::currentBpiChanged, this, &LooperWindow::updateBeatsPerInterval);
            disconnect(ninjamController, &NinjamController::currentBpmChanged, this, &LooperWindow::updateBeatsPerInterval);
            disconnect(ninjamController, &NinjamController::intervalBeatChanged, this, &LooperWindow::updateCurrentBeat);
        }
    }
}

void LooperWindow::updateCurrentBeat(uint currentIntervalBeat)
{
    if (!looper)
        return;

    for (const LayerView &layerView : layerViews.values()) {
        auto wavePanel = layerView.wavePanel;
        wavePanel->setCurrentBeat(currentIntervalBeat);
    }

    currentBeat = currentIntervalBeat;
}

void LooperWindow::updateBeatsPerInterval()
{
    qCDebug(jtGUI) << "LooperWindow::updateBeatsPerInterval";
    if (!mainController || !mainController->getNinjamController())
        return;

    auto ninjamController = mainController->getNinjamController();

    uint samplesPerInterval = ninjamController->getSamplesPerInterval();
    uint beatsPerInterval = ninjamController->getCurrentBpi();

    for (const LayerView &layerView : layerViews.values()) {
        auto wavePanel = layerView.wavePanel;
        wavePanel->setBeatsPerInteval(beatsPerInterval, samplesPerInterval);
    }
    qCDebug(jtGUI) << "LooperWindow::updateBeatsPerInterval ...done";
}

void LooperWindow::initializeControls()
{
    // play modes combo
    ui->comboBoxPlayMode->clear();

    std::vector<Looper::Mode> playModes;
    playModes.push_back(Looper::Sequence);
    playModes.push_back(Looper::AllLayers);
    playModes.push_back(Looper::SelectedLayer);

    for (uint i = 0; i < playModes.size(); ++i) {
        Looper::Mode playMode = playModes[i];
        ui->comboBoxPlayMode->addItem(Looper::getModeString(playMode), qVariantFromValue(playMode));
    }

    createPlayingOptionsCheckBoxes();
    createRecordingOptionsCheckBoxes();

    // max layer combobox
    ui->maxLayersComboBox->clear();
    for (quint8 l = 1; l <= MAX_LOOP_LAYERS; ++l) {
        ui->maxLayersComboBox->addItem(QString::number(l), QVariant::fromValue(l));
    }

    // wire signals/slots
    connect(ui->buttonRec, &QPushButton::clicked, [=] {
        if (looper) {
            looper->toggleRecording();
            if (looper->isWaitingToRecord()) {
                 if(!ui->buttonRec->isBlinking())
                     ui->buttonRec->startBlink();
            }
            else {
                ui->buttonRec->stopBlink();
            }
        }
    });

    connect(ui->buttonPlay, &QPushButton::clicked, [=] {
        if (looper) {
            looper->togglePlay();
            if (looper->isPlaying()) {
                if (looper->isWaitingToStopInNextInterval()) {
                    ui->buttonPlay->startBlink();
                }
            }
            else {
                ui->buttonPlay->stopBlink();
            }
        }
    });

    connect(ui->comboBoxPlayMode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
        if (index >= 0 && looper) {
            looper->setMode(ui->comboBoxPlayMode->currentData().value<Looper::Mode>());
            ui->comboBoxPlayMode->clearFocus();
        }
    });

    connect(ui->maxLayersComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index) {
        if (looper) {
            uint layers = index + 1;
            looper->setLayers(layers);
            ui->maxLayersComboBox->clearFocus();
        }
    });

    connect(ui->saveButton, &QPushButton::clicked, this, &LooperWindow::showSaveDialogs);
}

void LooperWindow::showSaveDialogs()
{
    if(!looper || !mainController->isPlayingInNinjamRoom())
        return;

    QString loopFileName = looper->getLoopName();
    QString originalLoopName = loopFileName;

    // show the save dialog
    const QString title = tr("Saving looper layers ...");
    bool dialogAccepted;
    loopFileName = QInputDialog::getText(this, title, tr("Loop file name:"), QLineEdit::Normal, QString(loopFileName), &dialogAccepted);

    if (!dialogAccepted || loopFileName.isEmpty()) {
        qDebug() << "Save looper dialog canceled";
        return; // user canceled or deleted the loop file name
    }

    // user is overwriting?
    if (loopFileName == originalLoopName) {
        QString text = tr("Loop file already exists. Do you want to overwrite?");
        int button = QMessageBox::question(this, title, text,  QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape);
        if (button != QMessageBox::Yes) {
            qDebug() << "Overwrite looper dialog canceled";
            return; // user canceled the overwrite dialog
        }
    }

    auto settings = mainController->getSettings();
    auto ninjamController = mainController->getNinjamController();
    QString savePath = settings.getLooperSavePath();
    bool encodeInOggVorbis = mainController->getLooperAudioEncodingFlag();
    float vorbisQuality = settings.getEncodingQuality();
    uint sampleRate = mainController->getSampleRate();
    uint bpm = ninjamController->getCurrentBpm();
    uint bpi = ninjamController->getCurrentBpi();
    quint8 bitDepth = mainController->getLooperBitDepth();

    LoopSaver loopSaver(savePath, looper);

    loopFileName = file::sanitizeFileName(loopFileName);
    loopSaver.save(loopFileName, bpm, bpi, encodeInOggVorbis, vorbisQuality, sampleRate, bitDepth);

    looper->setLoopName(loopFileName);

    updateControls();
}

QString LooperWindow::getOptionName(Looper::RecordingOption option)
{
    switch (option) {
    case Looper::HearAllLayers:   return tr("Hear all/Locked");
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
    case Looper::Overdub:         return tr("Overdub the current layer until REC button is pressed");
    }

    return QString();
}

QString LooperWindow::getOptionToolTip(Audio::Looper::PlayingOption option)
{
    switch (option) {
    case Looper::RandomizeLayers:   return tr("Randomize layers while playing");
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

void LooperWindow::showLoadMenu()
{
    QMenu *menu = ui->loadButton->menu();
    menu->clear();

    auto ninjamController = mainController->getNinjamController();
    quint16 currentBpm = ninjamController->getCurrentBpm();

    QString loopsDir = mainController->getSettings().getLooperSavePath();
    QList<LoopInfo> loopsInfos = LoopLoader::loadLoopsInfo(loopsDir, currentBpm);

    QString matchedMenuText = (!loopsInfos.isEmpty()) ? (tr("%1 BPM loops").arg(currentBpm)) : (tr("No loops for %1 BPM").arg(currentBpm));
    QMenu *bpmMatchedMenu = new QMenu(matchedMenuText);
    menu->addMenu(bpmMatchedMenu);
    for (const LoopInfo &loopInfo : loopsInfos) {
        QString loopString = loopInfo.toString();
        QAction *action = bpmMatchedMenu->addAction(loopString);
        connect(action, &QAction::triggered, [=](){
            loadLoopInfo(loopsDir, loopInfo);
        });
    }

    // load audio files
    menu->addSeparator();
    QAction *loadAudioFilesAction = menu->addAction(tr("Import audio files ..."));
    connect(loadAudioFilesAction, &QAction::triggered, [=](){
        QString filter = tr("Audio files") + " (*.wav *.ogg *.mp3)";
        QFileDialog audioFilesDialog(this, tr("Importing audio files ..."), loopsDir, filter);
        audioFilesDialog.setAcceptMode(QFileDialog::AcceptOpen);
        audioFilesDialog.setFileMode(QFileDialog::ExistingFiles);
        if (audioFilesDialog.exec()) {
            loadAudioFiles(audioFilesDialog.selectedFiles());
        }
    });


    // browse
    menu->addSeparator();
    QAction *browseAction = menu->addAction(tr("Browse JamTaba loops..."));
    connect(browseAction, &QAction::triggered, [=](){
        QString fileDialogTitle = tr("Open loop file");
        QString filter = tr("JamTaba Loop Files") + " (*.json)";
        QString loopFilePath = QFileDialog::getOpenFileName(this, fileDialogTitle, loopsDir, filter);
        if (!loopFilePath.isEmpty()) {
            QString loopDir = QFileInfo(loopFilePath).dir().absolutePath();
            loadLoopInfo(loopDir, LoopLoader::loadLoopInfo(loopFilePath));
        }
    });

    menu->show();
}

void LooperWindow::loadAudioFiles(const QStringList &audioFilePaths)
{
    if (audioFilePaths.isEmpty())
        return;

    if (audioFilePaths.size() == 1) { // loading just one file?
        loadAudioFilesIntoLayer(audioFilePaths, looper->getFocusedLayerIndex());
        return;
    }

    // loading more than one file
    bool canLoad = looper->getLastValidLayer() < (MAX_LOOP_LAYERS - 1);
    if (canLoad) {
        quint8 firstLayerIndex = looper->getLastValidLayer() + 1;
        if (looper->isEmpty())
            firstLayerIndex = 0;
        else if (firstLayerIndex >= looper->getLayers()) {
            firstLayerIndex = looper->getLayers(); // last layer
            const quint8 newLayersCount = qMin(static_cast<quint8>(MAX_LOOP_LAYERS), static_cast<quint8>(audioFilePaths.size()));
            looper->setLayers(newLayersCount); // expand layer count before add loaded samples
        }
        loadAudioFilesIntoLayer(audioFilePaths, firstLayerIndex);
    }
    else {
        qCritical() << "Can't load the audio files because all looper layers are filled!";
    }
}

void LooperWindow::loadAudioFilesIntoLayer(const QStringList &audioFilePaths, qint8 firstLayerIndex)
{
    if (firstLayerIndex < 0)
        firstLayerIndex = 0;

    if (looper->layerIsLocked(firstLayerIndex) && !audioFilePaths.isEmpty()) {
        QMessageBox::warning(this, tr("Error loading audio file!"), tr("Can't load the file '%1'").arg(QFileInfo(audioFilePaths.first()).fileName()));
        return;
    }

    const uint currentSampleRate = mainController->getSampleRate();
    const uint samplesPerInterval = mainController->getNinjamController()->getSamplesPerInterval();
    quint8 layerIndex = firstLayerIndex;
    for (const QString &audioFilePath : audioFilePaths) {
        if (looper->getLayers() <= layerIndex)
            looper->setLayers(layerIndex + 1); // expand looper layers

        SamplesBuffer samples(2, samplesPerInterval);
        bool loadResult = LoopLoader::loadAudioFile(audioFilePath, currentSampleRate, samples);
        if (loadResult) {
            looper->setLayerSamples(layerIndex, samples);
            layerIndex++;
        }
        else {
            QMessageBox::warning(this, tr("Error loading audio file!"), tr("Can't load the file '%1'").arg(QFileInfo(audioFilePath).baseName()));
        }
    }
}

void LooperWindow::updateLayersControls()
{
    for (quint8 layerIndex = 0; layerIndex < looper->getLayers(); ++layerIndex) {
        LayerControlsLayout *layerControlsLayout = layerViews[layerIndex].controlsLayout;

        QSlider *gainSlider = layerControlsLayout->gainSlider;
        QSlider *panSlider = layerControlsLayout->panSlider;

        QSignalBlocker gainBlocker(gainSlider);
        QSignalBlocker panBlocker(panSlider);

        float gain = Utils::poweredGainToLinear(looper->getLayerGain(layerIndex));
        float pan = looper->getLayerPan(layerIndex);
        gainSlider->setValue(gain * 100);
        panSlider->setValue(pan * panSlider->maximum());
    }
}

void LooperWindow::loadLoopInfo(const QString &loopDir, const LoopInfo &loopInfo)
{
    if (loopInfo.isValid()) {

        ui->loopNameLabel->setText("");

        LoopLoader loader(loopDir);
        uint currentSampleRate = mainController->getSampleRate();
        quint32 samplesPerInterval = mainController->getNinjamController()->getSamplesPerInterval();
        loader.load(loopInfo, looper, currentSampleRate, samplesPerInterval);

        updateLayersControls(); // update layers pan and gain after loading a loop

        updateModeComboBox();

        ui->loopNameLabel->setText(loopInfo.getName());

        update();
    }
    else {
        qCritical() << "Can't load loop " << loopInfo.getName() << " in " << loopDir;
    }
}
