#include "PreferencesDialogStandalone.h"

#include "audio/core/AudioDriver.h"
#include "midi/MidiDriver.h"
#include "gui/ScanFolderPanel.h"
#include "QFileDialog"
#include "persistence/Settings.h"

/**
 This file contains the common/shared implementation for the Jamtaba plataforms (Win, Mac and Linux) in Standalone. In the Vst Plugin some details are different and implemented in the file VstPreferencesDialog.cpp.
 */

using namespace Audio;
using namespace Midi;
using namespace Controller;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=

StandalonePreferencesDialog::StandalonePreferencesDialog(QWidget *parent,
                                                         bool showAudioControlPanelButton,
                                                         Audio::AudioDriver *audioDriver,
                                                         Midi::MidiDriver *midiDriver) :
    PreferencesDialog(parent),
    audioDriver(audioDriver),
    midiDriver(midiDriver),
    showAudioDriverControlPanelButton(showAudioControlPanelButton)
{
#ifdef Q_OS_MAC
    ui->comboAudioDevice->setVisible(false);
    ui->audioDeviceGroupBox->setVisible(false);
    ui->groupBoxInputs->setVisible(false);
    ui->groupBoxOutputs->setVisible(false);
#endif

    connect(ui->comboSampleRate, SIGNAL(activated(int)), this, SLOT(notifySampleRateChanged()));
    connect(ui->comboBufferSize, SIGNAL(activated(int)), this, SLOT(notifyBufferSizeChanged()));
}

void StandalonePreferencesDialog::notifyBufferSizeChanged()
{
    int newBufferSize = ui->comboBufferSize->currentData().toInt();
    emit bufferSizeChanged(newBufferSize);
}

void StandalonePreferencesDialog::notifySampleRateChanged()
{
    int newSampleRate = ui->comboSampleRate->currentData().toInt();
    emit sampleRateChanged(newSampleRate);
}

void StandalonePreferencesDialog::initialize(int initialTab, const Persistence::Settings *settings)
{
    PreferencesDialog::initialize(initialTab, settings);
    ui->prefsTab->setCurrentIndex(initialTab);
}

void StandalonePreferencesDialog::populateAllTabs()
{
    PreferencesDialog::populateAllTabs();//populate recording and metronome tabs
    populateAudioTab();
    populateMidiTab();
    populateVstTab();
}

void StandalonePreferencesDialog::setupSignals()
{
    PreferencesDialog::setupSignals();

    connect(ui->comboAudioDevice, SIGNAL(activated(int)), this, SLOT(changeAudioDevice(int)));

    connect(ui->comboFirstInput, SIGNAL(currentIndexChanged(int)), this,
            SLOT(populateLastInputCombo()));

    connect(ui->comboFirstOutput, SIGNAL(currentIndexChanged(int)), this,
            SLOT(populateLastOutputCombo()));

    connect(ui->buttonControlPanel, SIGNAL(clicked(bool)), this,
            SIGNAL(openingExternalAudioControlPanel()));

    connect(ui->buttonAddVstScanFolder, SIGNAL(clicked(bool)), this, SLOT(addVstScanFolder()));

    connect(ui->buttonClearVstAndScan, SIGNAL(clicked(bool)), this, SIGNAL(
                startingFullPluginsScan()));

    connect(ui->buttonVstRefresh, SIGNAL(clicked(bool)), this,
            SIGNAL(startingOnlyNewPluginsScan()));

    connect(ui->buttonAddVstToBlackList, SIGNAL(clicked(bool)), this,
            SLOT(addBlackListedPlugins()));

    connect(ui->buttonRemoveVstFromBlackList, SIGNAL(clicked(bool)), this,
            SLOT(removeBlackListedPlugins()));
}

void StandalonePreferencesDialog::addVstScanFolder()
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

void StandalonePreferencesDialog::clearScanFolderWidgets()
{
    QList<ScanFolderPanel *> panels = ui->panelScanFolders->findChildren<ScanFolderPanel *>();
    foreach (ScanFolderPanel *panel, panels) {
        ui->panelScanFolders->layout()->removeWidget(panel);
        panel->deleteLater();
    }
}

void StandalonePreferencesDialog::removeVstscanFolder()
{
    QPushButton *buttonClicked = qobject_cast<QPushButton *>(sender());

    Q_ASSERT(buttonClicked);

    // search the ScanFolderPanel containing the clicked button
    QList<ScanFolderPanel *> panels = ui->panelScanFolders->findChildren<ScanFolderPanel *>();
    ScanFolderPanel *panelToDelete = nullptr;
    foreach (ScanFolderPanel *panel, panels) {
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

void StandalonePreferencesDialog::updateVstList(QString path)
{
    ui->vstListWidget->appendPlainText(path);
}

void StandalonePreferencesDialog::updateBlackBox()
{
    ui->blackListWidget->clear();
    QStringList badPlugins = settings->getBlackListedPlugins();
    foreach (const QString &badPlugin, badPlugins)
        ui->blackListWidget->appendPlainText(badPlugin);
}

void StandalonePreferencesDialog::createWidgetsToNewFolder(QString path)
{
    ScanFolderPanel *panel = new ScanFolderPanel(path);
    connect(panel->getRemoveButton(), SIGNAL(clicked(bool)), this, SLOT(removeVstscanFolder()));
    ui->panelScanFolders->layout()->addWidget(panel);
    ui->panelScanFolders->layout()->setAlignment(panel, Qt::AlignTop);
}

void StandalonePreferencesDialog::clearVstList()
{
    ui->vstListWidget->clear();
}

// open a dialog to add a vst in the blacklist
void StandalonePreferencesDialog::addBlackListedPlugins()
{
    QFileDialog vstDialog(this, tr("Add Vst(s) to Black list ..."));
    vstDialog.setNameFilter("Dll(*.dll)");// TODO in mac the extension is .vst
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

void StandalonePreferencesDialog::removeBlackListedPlugins()
{
    QFileDialog vstDialog(this, tr("Remove Vst(s) from Black List ..."));
    vstDialog.setNameFilter("Dll(*.dll)");// TODO mac extension is .vst
    QStringList foldersToScan = settings->getVstScanFolders();
    if (!foldersToScan.isEmpty())
        vstDialog.setDirectory(foldersToScan.first());
    vstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    vstDialog.setFileMode(QFileDialog::ExistingFiles);
    if (vstDialog.exec()) {
        QStringList vstNames = vstDialog.selectedFiles();
        foreach (const QString &string, vstNames) {
            emit vstPluginRemovedFromBlackList(string);
            updateBlackBox();
        }
    }
}

void StandalonePreferencesDialog::selectAudioTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabAudio);
}

void StandalonePreferencesDialog::selectMidiTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabMidi);
}

void StandalonePreferencesDialog::selectVstPluginsTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabVST);
}

void StandalonePreferencesDialog::clearWidgetLayout(QWidget *widget)
{
    QLayoutItem *item;
    while ((item = widget->layout()->takeAt(0)) != 0) {
        if (item->widget())
            delete item->widget();
        delete item;
    }
}

void StandalonePreferencesDialog::populateMidiTab()
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
    } else {// no devices detected
        QLabel *label = new QLabel(tr("No midi input device detected!"));
        ui->midiContentPanel->layout()->addWidget(label);
        ui->midiContentPanel->layout()->setAlignment(label, Qt::AlignCenter);
    }
}

void StandalonePreferencesDialog::populateAudioTab()
{
    populateAsioDriverCombo();
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();

    ui->buttonControlPanel->setVisible(showAudioDriverControlPanelButton);
}

void StandalonePreferencesDialog::populateAsioDriverCombo()
{
    int devices = audioDriver->getDevicesCount();
    ui->comboAudioDevice->clear();
    for (int d = 0; d < devices; d++) {
        ui->comboAudioDevice->addItem(audioDriver->getAudioDeviceName(d), d);// using device index as userData in comboBox
    }
    ui->comboAudioDevice->setCurrentIndex(audioDriver->getAudioDeviceIndex());
}

void StandalonePreferencesDialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    int maxInputs = audioDriver->getMaxInputs();
    for (int i = 0; i < maxInputs; i++)
        ui->comboFirstInput->addItem(audioDriver->getInputChannelName(i), i);
    int firstInputIndex = audioDriver->getFirstSelectedInput();
    if (firstInputIndex < maxInputs)
        ui->comboFirstInput->setCurrentIndex(firstInputIndex);
    else
        ui->comboFirstInput->setCurrentIndex(0);
}

void StandalonePreferencesDialog::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    int maxInputs = audioDriver->getMaxInputs();
    int currentFirstInput = ui->comboFirstInput->currentData().toInt();
    int items = 0;
    const int MAX_ITEMS = maxInputs - currentFirstInput;
    for (int i = currentFirstInput; items < MAX_ITEMS; i++, items++)
        ui->comboLastInput->addItem(audioDriver->getInputChannelName(i), i);

    int lastInputIndex = audioDriver->getFirstSelectedInput() + (audioDriver->getInputsCount() - 1);
    if (lastInputIndex < ui->comboLastInput->count())
        ui->comboLastInput->setCurrentIndex(audioDriver->getInputsCount() - 1);
    else
        ui->comboLastInput->setCurrentIndex(ui->comboLastInput->count()-1);

}

void StandalonePreferencesDialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    int maxOuts = audioDriver->getMaxOutputs();
    for (int i = 0; i < maxOuts; i++)
        ui->comboFirstOutput->addItem(audioDriver->getOutputChannelName(i), i);
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getFirstSelectedOutput());
}

void StandalonePreferencesDialog::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = audioDriver->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if (currentFirstOut + 1 < maxOuts)
        currentFirstOut++;// to avoid 1 channel output

    int items = 0;
    const int MAX_ITEMS = 1;// std::min( maxOuts - currentFirstOut, 2);
    for (int i = currentFirstOut; items < MAX_ITEMS; i++, items++)
        ui->comboLastOutput->addItem(audioDriver->getOutputChannelName(i), i);
    int lastOutputIndex = audioDriver->getFirstSelectedOutput() + audioDriver->getOutputsCount();
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex(index >= 0 ? index : 0);

    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void StandalonePreferencesDialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();

    QList<int> sampleRates = audioDriver->getValidSampleRates(audioDriver->getAudioDeviceIndex());
    foreach (int sampleRate, sampleRates)
        ui->comboSampleRate->addItem(QString::number(sampleRate), sampleRate);

    ui->comboSampleRate->setCurrentText(QString::number(audioDriver->getSampleRate()));
    ui->comboSampleRate->setEnabled(!sampleRates.isEmpty());
}

void StandalonePreferencesDialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    QList<int> bufferSizes = audioDriver->getValidBufferSizes(audioDriver->getAudioDeviceIndex());
    foreach (int size, bufferSizes)
        ui->comboBufferSize->addItem(QString::number(size), size);

    ui->comboBufferSize->setCurrentText(QString::number(audioDriver->getBufferSize()));
    ui->comboBufferSize->setEnabled(!bufferSizes.isEmpty());
}

// ++++++++++++
void StandalonePreferencesDialog::changeAudioDevice(int index)
{
    int deviceIndex = ui->comboAudioDevice->itemData(index).toInt();
    audioDriver->setAudioDeviceIndex(deviceIndex);

    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
}

void StandalonePreferencesDialog::accept()
{
    int selectedAudioDevice = ui->comboAudioDevice->currentIndex();
    int firstIn = ui->comboFirstInput->currentData().toInt();
    int lastIn = ui->comboLastInput->currentData().toInt();
    int firstOut = ui->comboFirstOutput->currentData().toInt();
    int lastOut = ui->comboLastOutput->currentData().toInt();

    // build midi inputs devices status
    QList<bool> midiInputsStatus;
    QList<QCheckBox *> boxes = ui->midiContentPanel->findChildren<QCheckBox *>();
    foreach (QCheckBox *check, boxes)
        midiInputsStatus.append(check->isChecked());

    PreferencesDialog::accept();

    emit ioPreferencesChanged(midiInputsStatus, selectedAudioDevice, firstIn, lastIn, firstOut,
                              lastOut);
}

void StandalonePreferencesDialog::populateVstTab()
{
    clearScanFolderWidgets();// remove all widgets before add the paths

    // populate the paths
    foreach (const QString &scanFolder, settings->getVstScanFolders())
        createWidgetsToNewFolder(scanFolder);

    // populate the VST list
    ui->vstListWidget->clear();
    foreach (const QString &path, settings->getVstPluginsPaths())
        updateVstList(path);

    updateBlackBox();
}

void StandalonePreferencesDialog::selectTab(int index)
{
    switch (index) {
    case 0:
        populateAudioTab();
        break;
    case 1:
        populateMidiTab();
        break;
    case 2:
        populateVstTab();
        break;
    case 3:
        populateRecordingTab();
        break;
    }
}
