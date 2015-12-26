#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include "audio/core/AudioDriver.h"
#include "MainController.h"

/**
 This file contains the common/shared implementation for the Jamtaba plataforms (Win, Mac and Linux) in Standalone. In the Vst Plugin some details are different and implemented in the file VstPreferencesDialog.cpp.
 */

using namespace Audio;

void PreferencesDialog::selectAudioTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabAudio);
}

void PreferencesDialog::selectMidiTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabMidi);
}

void PreferencesDialog::selectVstPluginsTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabVST);
}

void PreferencesDialog::populateMidiTab()
{
// if (mainController->isunningAsVstPlugin())
// return;

    // clear
    QLayoutItem *item;
    while ((item = ui->midiContentPanel->layout()->takeAt(0)) != 0) {
        if (item->widget())
            delete item->widget();
        delete item;
    }

    Midi::MidiDriver *midiDriver = mainController->getMidiDriver();
    int maxInputDevices = midiDriver->getMaxInputDevices();
    if (maxInputDevices > 0) {
        QList<bool> midiInputsStatus = mainController->getSettings().getMidiInputDevicesStatus();
        for (int i = 0; i < maxInputDevices; ++i) {
            QCheckBox *checkBox = new QCheckBox(midiDriver->getInputDeviceName(i));
            ui->midiContentPanel->layout()->addWidget(checkBox);
            checkBox->setChecked(
                midiInputsStatus.isEmpty() || i > midiInputsStatus.size()-1 || midiInputsStatus.at(
                    i));
        }
        ui->midiContentPanel->layout()->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum,
                                                                QSizePolicy::Expanding));
    } else {// no devices detected
        QLabel *label = new QLabel("No midi input device detected!");
        ui->midiContentPanel->layout()->addWidget(label);
        ui->midiContentPanel->layout()->setAlignment(label, Qt::AlignCenter);
    }
}

void PreferencesDialog::populateAudioTab()
{
    populateAsioDriverCombo();
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();
    ui->buttonControlPanel->setVisible(mainController->getAudioDriver()->hasControlPanel());
}

void PreferencesDialog::populateAsioDriverCombo()
{
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    int devices = audioDriver->getDevicesCount();
    ui->comboAudioDevice->clear();
    for (int d = 0; d < devices; d++) {
        // using device index as userData in comboBox
        ui->comboAudioDevice->addItem(audioDriver->getAudioDeviceName(d), d);
    }
    ui->comboAudioDevice->setCurrentIndex(audioDriver->getAudioDeviceIndex());
}

void PreferencesDialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    int maxInputs = audioDriver->getMaxInputs();
    for (int i = 0; i < maxInputs; i++)
        ui->comboFirstInput->addItem(audioDriver->getInputChannelName(i), i);
    int firstInputIndex = audioDriver->getSelectedInputs().getFirstChannel();
    if (firstInputIndex < maxInputs)
        ui->comboFirstInput->setCurrentIndex(firstInputIndex);
    else
        ui->comboFirstInput->setCurrentIndex(0);
}

void PreferencesDialog::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    int maxInputs = audioDriver->getMaxInputs();
    int currentFirstInput = ui->comboFirstInput->currentData().toInt();
    int items = 0;
    const int MAX_ITEMS = maxInputs - currentFirstInput;
    for (int i = currentFirstInput; items < MAX_ITEMS; i++, items++)
        ui->comboLastInput->addItem(audioDriver->getInputChannelName(i), i);
    ChannelRange inputsRange = audioDriver->getSelectedInputs();
    int lastInputIndex = inputsRange.getLastChannel();
    if (lastInputIndex < ui->comboLastInput->count())
        ui->comboLastInput->setCurrentIndex(lastInputIndex);
    else
        ui->comboLastInput->setCurrentIndex(ui->comboLastInput->count()-1);
}

void PreferencesDialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    int maxOuts = audioDriver->getMaxOutputs();
    for (int i = 0; i < maxOuts; i++)
        ui->comboFirstOutput->addItem(audioDriver->getOutputChannelName(i), i);
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getSelectedOutputs().getFirstChannel());
}

void PreferencesDialog::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = mainController->getAudioDriver()->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if (currentFirstOut + 1 < maxOuts)
        currentFirstOut++;// to avoid 1 channel output
    int items = 0;
    const int MAX_ITEMS = 1;// std::min( maxOuts - currentFirstOut, 2);
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    for (int i = currentFirstOut; items < MAX_ITEMS; i++, items++)
        ui->comboLastOutput->addItem(audioDriver->getOutputChannelName(i), i);
    int lastOutputIndex = audioDriver->getSelectedOutputs().getLastChannel();
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex(index >= 0 ? index : 0);
}

void PreferencesDialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();

    AudioDriver *audioDriver = mainController->getAudioDriver();
    QList<int> sampleRates = audioDriver->getValidSampleRates(audioDriver->getAudioDeviceIndex());
    foreach (int sampleRate, sampleRates)
        ui->comboSampleRate->addItem(QString::number(sampleRate), sampleRate);
    ui->comboSampleRate->setCurrentText(QString::number(audioDriver->getSampleRate()));

    ui->comboSampleRate->setEnabled(!sampleRates.isEmpty());
}

void PreferencesDialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    AudioDriver *audioDriver = mainController->getAudioDriver();
    QList<int> bufferSizes = audioDriver->getValidBufferSizes(audioDriver->getAudioDeviceIndex());
    foreach (int size, bufferSizes)
        ui->comboBufferSize->addItem(QString::number(size), size);
    ui->comboBufferSize->setCurrentText(QString::number(audioDriver->getBufferSize()));
    ui->comboBufferSize->setEnabled(!bufferSizes.isEmpty());
}

// ++++++++++++
void PreferencesDialog::on_comboAudioDevice_activated(int index)
{
    int deviceIndex = ui->comboAudioDevice->itemData(index).toInt();
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    audioDriver->setAudioDeviceIndex(deviceIndex);
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
}

void PreferencesDialog::on_comboFirstInput_currentIndexChanged(int /*index*/)
{
    populateLastInputCombo();
}

void PreferencesDialog::on_comboFirstOutput_currentIndexChanged(int /*index*/)
{
    populateLastOutputCombo();
    Audio::AudioDriver *audioDriver = mainController->getAudioDriver();
    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void PreferencesDialog::on_okButton_released()
{
    int selectedAudioDevice = ui->comboAudioDevice->currentIndex();
    int firstIn = ui->comboFirstInput->currentData().toInt();
    int lastIn = ui->comboLastInput->currentData().toInt();
    int firstOut = ui->comboFirstOutput->currentData().toInt();
    int lastOut = ui->comboLastOutput->currentData().toInt();
    int sampleRate = ui->comboSampleRate->currentText().toInt();
    int bufferSize = ui->comboBufferSize->currentText().toInt();

    // build midi inputs devices status
    QList<bool> midiInputsStatus;
    QList<QCheckBox *> boxes = ui->midiContentPanel->findChildren<QCheckBox *>();
    foreach (QCheckBox *check, boxes)
        midiInputsStatus.append(check->isChecked());

    this->accept();

    emit ioPreferencesChanged(midiInputsStatus, selectedAudioDevice, firstIn, lastIn, firstOut,
                              lastOut, sampleRate, bufferSize);
}

void PreferencesDialog::populateVstTab()
{
    clearScanFolderWidgets();// remove all widgets before add the paths
    QStringList ScanFoldersList = mainController->getSettings().getVstScanFolders();
    QStringList VstList = mainController->getSettings().getVstPluginsPaths();
    QStringList BlackVstList = mainController->getSettings().getBlackListedPlugins();

    // populate the paths
    foreach (QString scanFolder, ScanFoldersList)
        createWidgetsToNewScanFolder(scanFolder);
    // populate the VST list
    ui->plainTextEdit->clear();
    foreach (QString path, VstList)
        UpdateVstList(path);
    // populate the BlackBox
    ui->BlackBoxText->clear();
    foreach (QString path, BlackVstList)
        UpdateBlackBox(path, true);// add vst
}

void PreferencesDialog::on_prefsTab_currentChanged(int index)
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
