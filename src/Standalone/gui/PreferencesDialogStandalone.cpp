#include "PreferencesDialogStandalone.h"

#include "QFileDialog"
#include "persistence/Settings.h"
#include <QDebug>
#include "audio/core/AudioDriver.h"
#include "midi/MidiDriver.h"
#include "gui/ScanFolderPanel.h"

using audio::AudioDriver;
using midi::MidiDriver;
using audio::PluginDescriptor;

/**
 This file contains the common/shared implementation for the Jamtaba plataforms (Win, Mac and Linux) in Standalone. In the Vst Plugin some details are different and implemented in the file VstPreferencesDialog.cpp.
 */

PreferencesDialogStandalone::PreferencesDialogStandalone(QWidget *parent,
                                                         bool showAudioControlPanelButton,
                                                         AudioDriver *audioDriver,
                                                         MidiDriver *midiDriver) :
    PreferencesDialog(parent),
    audioDriver(audioDriver),
    midiDriver(midiDriver),
    showAudioDriverControlPanelButton(showAudioControlPanelButton)
{
    if (UseSingleAudioIODevice)
    {
        ui->comboAudioOutputDevice->setVisible(false);
        ui->comboAudioOutputDeviceLabel->setVisible(false);
        ui->comboAudioInputDeviceLabel->setVisible(false);
    }

    connect(ui->comboSampleRate, SIGNAL(activated(int)), this, SLOT(notifySampleRateChanged()));
    connect(ui->comboBufferSize, SIGNAL(activated(int)), this, SLOT(notifyBufferSizeChanged()));
}

void PreferencesDialogStandalone::notifyBufferSizeChanged()
{
    int newBufferSize = ui->comboBufferSize->currentData().toInt();
    emit bufferSizeChanged(newBufferSize);
}

void PreferencesDialogStandalone::notifySampleRateChanged()
{
    int newSampleRate = ui->comboSampleRate->currentData().toInt();
    emit sampleRateChanged(newSampleRate);
}

void PreferencesDialogStandalone::initialize(PreferencesTab initialTab, const persistence::Settings *settings, const QMap<QString, QString> &jamRecorders)
{
    PreferencesDialog::initialize(initialTab, settings, jamRecorders);
    int tabIndex = static_cast<int>(initialTab);
    ui->prefsTab->setCurrentIndex(tabIndex);
}

void PreferencesDialogStandalone::populateAllTabs()
{
    PreferencesDialog::populateAllTabs(); // populate recording and metronome tabs

    populateAudioTab();

    populateMidiTab();

    populateVstTab();
}

void PreferencesDialogStandalone::setupSignals()
{
    PreferencesDialog::setupSignals();

    connect(ui->comboAudioInputDevice, SIGNAL(activated(int)), this, SLOT(changeAudioInputDevice(int)));
    connect(ui->comboAudioOutputDevice, SIGNAL(activated(int)), this, SLOT(changeAudioOutputDevice(int)));

    connect(ui->comboFirstInput, SIGNAL(currentIndexChanged(int)), this,
            SLOT(populateLastInputCombo()));

    connect(ui->comboFirstOutput, SIGNAL(currentIndexChanged(int)), this,
            SLOT(populateLastOutputCombo()));

    connect(ui->buttonControlPanel, SIGNAL(clicked(bool)), this,
            SIGNAL(openingExternalAudioControlPanel()));

    connect(ui->buttonAddVstScanFolder, SIGNAL(clicked(bool)), this, SLOT(showDialogToAddVstScanFolder()));

    connect(ui->buttonClearVstAndScan, SIGNAL(clicked(bool)), this, SIGNAL(
                startingFullPluginsScan()));

    connect(ui->buttonVstRefresh, SIGNAL(clicked(bool)), this,
            SIGNAL(startingOnlyNewPluginsScan()));

    connect(ui->buttonAddVstToBlackList, SIGNAL(clicked(bool)), this,
            SLOT(addBlackListedPlugins()));

    connect(ui->buttonRemoveVstFromBlackList, SIGNAL(clicked(bool)), this,
            SLOT(removeBlackListedPlugins()));
}

void PreferencesDialogStandalone::showDialogToAddVstScanFolder()
{
    QFileDialog fileDialog(this, tr("Adding VST path ..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (fileDialog.exec()) {
        QDir dir = fileDialog.directory();
        QString newFolder = dir.absolutePath();
        createWidgetsToNewFolder(newFolder);
        emit vstScanDirAdded(newFolder);
    }
}

void PreferencesDialogStandalone::clearScanFolderWidgets()
{
    QList<ScanFolderPanel *> panels = ui->panelScanFolders->findChildren<ScanFolderPanel *>();
    for (ScanFolderPanel *panel : panels) {
        ui->panelScanFolders->layout()->removeWidget(panel);
        panel->deleteLater();
    }
}

void PreferencesDialogStandalone::removeSelectedVstScanFolder()
{
    QPushButton *buttonClicked = qobject_cast<QPushButton *>(sender());

    Q_ASSERT(buttonClicked);

    // search the ScanFolderPanel containing the clicked button
    auto panels = ui->panelScanFolders->findChildren<ScanFolderPanel *>();
    ScanFolderPanel *panelToDelete = nullptr;
    for (auto panel : panels) {
        if (panel->getRemoveButton() == buttonClicked) {
            panelToDelete = panel;
            break;
        }
    }
    if (panelToDelete) {
        ui->panelScanFolders->layout()->removeWidget(panelToDelete);
        emit vstScanDirRemoved(panelToDelete->getScanFolder());
        panelToDelete->deleteLater();
    }
}

void PreferencesDialogStandalone::addFoundedVstPlugin(const QString &pluginName)
{
    ui->vstListWidget->appendPlainText(pluginName);
}

void PreferencesDialogStandalone::setCurrentScannedVstPlugin(const QString &pluginPath)
{
    ui->labelVstScanStatus->setText(tr("scanning %1").arg(pluginPath));
}

void PreferencesDialogStandalone::updateBlackBox()
{
    ui->blackListWidget->clear();
    QStringList badPlugins = settings->getBlackListedPlugins();
    for (const QString &badPlugin : badPlugins)
        ui->blackListWidget->appendPlainText(badPlugin);
}

void PreferencesDialogStandalone::createWidgetsToNewFolder(const QString &path)
{
    ScanFolderPanel *panel = new ScanFolderPanel(path);
    connect(panel->getRemoveButton(), SIGNAL(clicked(bool)), this, SLOT(removeSelectedVstScanFolder()));
    ui->panelScanFolders->layout()->addWidget(panel);
    ui->panelScanFolders->layout()->setAlignment(panel, Qt::AlignTop);
}

void PreferencesDialogStandalone::clearVstList()
{
    ui->vstListWidget->clear();
}

// open a dialog to add a vst in the blacklist
void PreferencesDialogStandalone::addBlackListedPlugins()
{
    QFileDialog vstDialog(this, tr("Add Vst(s) to Black list ..."));
    vstDialog.setNameFilter("Dll(*.dll)"); // TODO in mac the extension is .vst

    if (!settings->getVstScanFolders().isEmpty())
        vstDialog.setDirectory(settings->getVstScanFolders().first());

    vstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    vstDialog.setFileMode(QFileDialog::ExistingFiles);

    if (vstDialog.exec()) {
        QStringList vstNames = vstDialog.selectedFiles();
        foreach (const QString &string, vstNames) {
            emit vstPluginAddedInBlackList(string);
            updateBlackBox();
        }
    }
}

void PreferencesDialogStandalone::removeBlackListedPlugins()
{
    QFileDialog vstDialog(this, tr("Remove Vst(s) from Black List ..."));
    vstDialog.setNameFilter("Dll(*.dll)"); // TODO mac extension is .vst
    QStringList foldersToScan = settings->getVstScanFolders();

    if (!foldersToScan.isEmpty())
        vstDialog.setDirectory(foldersToScan.first());

    vstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    vstDialog.setFileMode(QFileDialog::ExistingFiles);

    if (vstDialog.exec()) {
        QStringList vstNames = vstDialog.selectedFiles();
        for (const QString &string : vstNames) {
            emit vstPluginRemovedFromBlackList(string);
            updateBlackBox();
        }
    }
}

void PreferencesDialogStandalone::selectAudioTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabAudio);
}

void PreferencesDialogStandalone::selectMidiTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabMidi);
}

void PreferencesDialogStandalone::selectVstPluginsTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabVST);
}

void PreferencesDialogStandalone::clearWidgetLayout(QWidget *widget)
{
    QLayoutItem *item;
    while ((item = widget->layout()->takeAt(0)) != 0) {
        if (item->widget())
            delete item->widget();
        delete item;
    }
}

void PreferencesDialogStandalone::populateMidiTab()
{
    clearWidgetLayout(ui->midiContentPanel);
    int maxInputDevices = midiDriver->getMaxInputDevices();
    if (maxInputDevices > 0) {
        QList<bool> midiInputsStatus = settings->getMidiInputDevicesStatus();
        for (int i = 0; i < maxInputDevices; ++i) {
            QString midiDeviceName = midiDriver->getInputDeviceName(i);
            if (!midiDeviceName.isEmpty()) {
                QCheckBox *checkBox = new QCheckBox(midiDeviceName);
                ui->midiContentPanel->layout()->addWidget(checkBox);
                bool deviceIsSelected = i < midiInputsStatus.size() && midiInputsStatus.at(i);
                bool isNewDevice = i >= midiInputsStatus.size();
                checkBox->setChecked(midiInputsStatus.isEmpty() || deviceIsSelected || isNewDevice);
            }
        }
        QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->midiContentPanel->layout()->addItem(spacer);
    }
    else { // no devices detected
        QLabel *label = new QLabel(tr("No midi input device detected!"));
        ui->midiContentPanel->layout()->addWidget(label);
        ui->midiContentPanel->layout()->setAlignment(label, Qt::AlignCenter);
    }
}

void PreferencesDialogStandalone::populateSyncTab() {
    clearWidgetLayout(ui->syncContentPanel);
    int maxOutputDevices = midiDriver->getMaxOutputDevices();
    if (maxOutputDevices > 0) {
        QList<bool> syncOutputsStatus = settings->getSyncOutputDevicesStatus();
        for (int i = 0; i < maxOutputDevices; ++i) {
            QString midiDeviceName = midiDriver->getOutputDeviceName(i);
            if (!midiDeviceName.isEmpty()) {
                QCheckBox *checkBox = new QCheckBox(midiDeviceName);
                ui->syncContentPanel->layout()->addWidget(checkBox);
                bool deviceIsSelected = i < syncOutputsStatus.size() && syncOutputsStatus.at(i);
                bool isNewDevice = i >= syncOutputsStatus.size();
                checkBox->setChecked(syncOutputsStatus.isEmpty() || deviceIsSelected || isNewDevice);
            }
        }
        QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->syncContentPanel->layout()->addItem(spacer);
    }
    else { // no devices detected
        QLabel *label = new QLabel(tr("No midi output device detected!"));
        ui->syncContentPanel->layout()->addWidget(label);
        ui->syncContentPanel->layout()->setAlignment(label, Qt::AlignCenter);
    }
}

void PreferencesDialogStandalone::populateAudioTab()
{
    populateAsioDriverCombo();
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();

    ui->buttonControlPanel->setVisible(showAudioDriverControlPanelButton);
}

void PreferencesDialogStandalone::populateAsioDriverCombo()
{
    int devices = audioDriver->getDevicesCount();
    ui->comboAudioInputDevice->clear();
    ui->comboAudioOutputDevice->clear();

    unsigned nIn=0, nOut=0;

    auto currentInputDeviceIndex = audioDriver->getAudioInputDeviceIndex();
    auto currentOutputDeviceIndex = audioDriver->getAudioOutputDeviceIndex();
    int posIn=0, posOut=0, markIn=0, markOut=0;

    for (int d = 0; d < devices; d++) {
        auto name = audioDriver->getAudioDeviceInfo(d, nIn, nOut);
        if (nIn>0) {
            ui->comboAudioInputDevice->addItem(name, d); // using device index as userData in comboBox
            if (currentInputDeviceIndex==d) markIn=posIn; else posIn++;
        }
        if (nOut>0) {
            ui->comboAudioOutputDevice->addItem(name, d); // using device index as userData in comboBox
            if (currentOutputDeviceIndex==d) markOut=posOut; else posOut++;
        }
    }

    ui->comboAudioInputDevice->setCurrentIndex(markIn);
    ui->comboAudioOutputDevice->setCurrentIndex(markOut);
}

void PreferencesDialogStandalone::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    int maxInputs = audioDriver->getMaxInputs();
    for (int i = 0; i < maxInputs; i++)
    {
        auto cName = audioDriver->getInputChannelName(i);
        ui->comboFirstInput->addItem(cName, i);
    }

    int firstInputIndex = audioDriver->getFirstSelectedInput();
    if (firstInputIndex < maxInputs)
        ui->comboFirstInput->setCurrentIndex(firstInputIndex);
    else
        ui->comboFirstInput->setCurrentIndex(0);
}

void PreferencesDialogStandalone::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    int maxInputs = audioDriver->getMaxInputs();
    int currentFirstInput = ui->comboFirstInput->currentData().toInt();
    int items = 0;
    const int MAX_ITEMS = maxInputs - currentFirstInput;
    for (int i = currentFirstInput; items < MAX_ITEMS; i++, items++)
        ui->comboLastInput->addItem(audioDriver->getInputChannelName(i), i);

    int lastInputIndex = audioDriver->getFirstSelectedInput() + MAX_ITEMS - 1;
    if (lastInputIndex < ui->comboLastInput->count())
        ui->comboLastInput->setCurrentIndex(lastInputIndex);
    else
        ui->comboLastInput->setCurrentIndex(ui->comboLastInput->count()-1);

}

void PreferencesDialogStandalone::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    int maxOuts = audioDriver->getMaxOutputs();
    for (int i = 0; i < maxOuts; i++)
        ui->comboFirstOutput->addItem(audioDriver->getOutputChannelName(i), i);
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getFirstSelectedOutput());
}

void PreferencesDialogStandalone::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = audioDriver->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if (currentFirstOut + 1 < maxOuts)
        currentFirstOut++; // to avoid 1 channel output

    int items = 0;
    const int MAX_ITEMS = 1; // std::min( maxOuts - currentFirstOut, 2);
    for (int i = currentFirstOut; items < MAX_ITEMS; i++, items++)
        ui->comboLastOutput->addItem(audioDriver->getOutputChannelName(i), i);
    int lastOutputIndex = audioDriver->getFirstSelectedOutput() + audioDriver->getOutputsCount();
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex(index >= 0 ? index : 0);

    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void PreferencesDialogStandalone::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();

    QList<int> sampleRates = audioDriver->getValidSampleRates(audioDriver->getAudioOutputDeviceIndex());
    for (int sampleRate : sampleRates)
        ui->comboSampleRate->addItem(QString::number(sampleRate), sampleRate);

    ui->comboSampleRate->setCurrentText(QString::number(audioDriver->getSampleRate()));
    ui->comboSampleRate->setEnabled(!sampleRates.isEmpty());
}

void PreferencesDialogStandalone::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    QList<int> bufferSizes = audioDriver->getValidBufferSizes(audioDriver->getAudioOutputDeviceIndex());
    for (int size : bufferSizes)
        ui->comboBufferSize->addItem(QString::number(size), size);

    ui->comboBufferSize->setCurrentText(QString::number(audioDriver->getBufferSize()));
    ui->comboBufferSize->setEnabled(!bufferSizes.isEmpty());
}

void PreferencesDialogStandalone::changeAudioInputDevice(int index)
{
    int deviceIndex = ui->comboAudioInputDevice->itemData(index).toInt();
    audioDriver->setAudioInputDeviceIndex(deviceIndex);
    populateFirstInputCombo();
    // On some platforms that don't support well seperate i/o devices yet,
    // we may use only one device selection so auto-update output
    if  (UseSingleAudioIODevice) {
        changeAudioOutputDevice(deviceIndex); // also sets buffer and sample size
    }
    else {
        populateSampleRateCombo();
        populateBufferSizeCombo();
    }
}

void PreferencesDialogStandalone::changeAudioOutputDevice(int index)
{
    int deviceIndex = UseSingleAudioIODevice ?
                ui->comboAudioInputDevice->itemData(index).toInt() :
                ui->comboAudioOutputDevice->itemData(index).toInt();

    audioDriver->setAudioOutputDeviceIndex(deviceIndex);

    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();
}

void PreferencesDialogStandalone::accept()
{
    auto indexIn = ui->comboAudioInputDevice->currentIndex();
    auto selectedAudioInputDevice = ui->comboAudioInputDevice->itemText(indexIn);
    auto firstIn = ui->comboFirstInput->currentData().toInt();
    auto lastIn = ui->comboLastInput->currentData().toInt();

    auto indexOut = ui->comboAudioOutputDevice->currentIndex();
    auto selectedAudioOutputDevice = ui->comboAudioOutputDevice->itemText(indexOut);
    if (!ui->comboAudioOutputDevice->isVisible()) { // not using separarated input & output devices
        indexOut = indexIn;
        selectedAudioOutputDevice = selectedAudioInputDevice;
    }

    auto firstOut = ui->comboFirstOutput->currentData().toInt();
    auto lastOut = ui->comboLastOutput->currentData().toInt();

    QList<bool> midiInputsStatus;
    // build midi inputs devices status
    auto midiBoxes = ui->midiContentPanel->findChildren<QCheckBox *>();
    for (auto check : midiBoxes)
        midiInputsStatus.append(check->isChecked());

    QList<bool> syncOutputsStatus;
    auto syncBoxes = ui->syncContentPanel->findChildren<QCheckBox *>();
    for (auto check : syncBoxes)
        syncOutputsStatus.append(check->isChecked());

    PreferencesDialog::accept();

    emit ioPreferencesChanged(midiInputsStatus, syncOutputsStatus,
                              selectedAudioInputDevice, selectedAudioOutputDevice,
                              firstIn, lastIn, firstOut, lastOut);
}

void PreferencesDialogStandalone::populateVstTab()
{
    clearScanFolderWidgets(); // remove all widgets before add the paths

    // populate the paths
    for (const QString &scanFolder : settings->getVstScanFolders())
        createWidgetsToNewFolder(scanFolder);

    // populate the VST list
    ui->vstListWidget->clear();
    ui->labelVstScanStatus->clear();
    for (const QString &path : settings->getVstPluginsPaths()) {
        QString pluginName = PluginDescriptor::getVstPluginNameFromPath(path);
        addFoundedVstPlugin(pluginName);
    }

    // update black listed plugins
    updateBlackBox();
}

void PreferencesDialogStandalone::selectTab(int index)
{
    switch (static_cast<PreferencesTab>(index)) {
    case PreferencesTab::TabAudio:
        populateAudioTab();
        break;
    case PreferencesTab::TabMidi:
        populateMidiTab();
        break;
    case PreferencesTab::TabSync:
        populateSyncTab();
        break;
    case PreferencesTab::TabVST:
        populateVstTab();
        break;
    case PreferencesTab::TabMultiTrackRecording:
        populateMultiTrackRecordingTab();
        break;
    case PreferencesTab::TabLooper:
        populateLooperTab();
        break;
    case PreferencesTab::TabMetronome:
        populateMetronomeTab();
        break;
    }
}
