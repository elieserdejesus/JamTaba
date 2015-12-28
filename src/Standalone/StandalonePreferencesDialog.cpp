#include "StandalonePreferencesDialog.h"

#include "audio/core/AudioDriver.h"
#include "StandAloneMainController.h"
#include "MainWindow.h"
#include "QFileDialog"

/**
 This file contains the common/shared implementation for the Jamtaba plataforms (Win, Mac and Linux) in Standalone. In the Vst Plugin some details are different and implemented in the file VstPreferencesDialog.cpp.
 */

using namespace Audio;
using namespace Controller;

// Internal class used to handle the VST scan folders widgets
class StandalonePreferencesDialog::ScanFolderPanel : public QWidget
{
public:
    ScanFolderPanel(QString folder) :
        QWidget(0),
        removeButton(nullptr),
        scanFolder(folder)
    {
        removeButton = new QPushButton(QIcon(":/images/less.png"), "");
        removeButton->setToolTip("Remove this folder from scanning");

        QHBoxLayout *layout = new QHBoxLayout();
        this->setLayout(layout);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(removeButton);
        layout->addWidget(new QLabel(scanFolder, this), 1.0);
    }

    QPushButton *getRemoveButton() const
    {
        return removeButton;
    }

    QString getScanFolder() const
    {
        return scanFolder;
    }

private:
    QPushButton *removeButton;
    QString scanFolder;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=

StandalonePreferencesDialog::StandalonePreferencesDialog(Controller::MainController *mainController,
                                                         MainWindow *mainWindow, int initialTab) :
    PreferencesDialog(mainController, mainWindow)
{
    setupSignals();
    populateAllTabs();
    selectPreferencesTab(initialTab);

#ifdef Q_OS_MAC
    ui->comboAudioDevice->setVisible(false);
    ui->asioDriverLabel->setVisible(false);
    ui->groupBoxInputs->setVisible(false);
    ui->groupBoxOutputs->setVisible(false);
#endif
}

void StandalonePreferencesDialog::populateAllTabs()
{
    populateAudioTab();
    populateMidiTab();
    populateVstTab();
    populateRecordingTab();
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
            SLOT(openExternalAudioControlPanel()));

    connect(ui->buttonAddVstScanFolder, SIGNAL(clicked(bool)), this, SLOT(addVstScanFolder()));

    connect(ui->buttonClearVstAndScan, SIGNAL(clicked(bool)), this, SLOT(scansFully()));

    connect(ui->ButtonVst_Refresh, SIGNAL(clicked(bool)), this, SLOT(scanNewPlugins()));

    connect(ui->ButtonVST_AddToBlackList, SIGNAL(clicked(bool)), this,
            SLOT(addBlackListedPlugins()));

    connect(ui->ButtonVST_RemFromBlkList, SIGNAL(clicked(bool)), this,
            SLOT(removeBlackListedPlugins()));

    connect(mainController->getPluginFinder(), SIGNAL(scanFinished(bool)), this,
            SLOT(populateVstTab()));
}

void StandalonePreferencesDialog::addVstScanFolder()
{
    QFileDialog fileDialog(this, "Adding VST path ...");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (fileDialog.exec()) {
        QDir dir = fileDialog.directory();
        addVstFolderToScan(dir.absolutePath());
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
        mainController->removePluginsScanPath(panelToDelete->getScanFolder());
        ui->panelScanFolders->layout()->removeWidget(panelToDelete);
        panelToDelete->deleteLater();
    }
}

void StandalonePreferencesDialog::updateVstList(QString path)
{
    ui->vstListWidget->appendPlainText(path);
}

void StandalonePreferencesDialog::updateBlackBox(QString path, bool add)
{
    if (add) {
        ui->blackListWidget->appendPlainText(path);
    } else {
        QString str = ui->blackListWidget->toPlainText();
        if (str.contains(path)) {
            ui->blackListWidget->clear();
            mainController->removeBlackVst(str.indexOf(path));
            QStringList list = mainController->getSettings().getBlackListedPlugins();
            foreach (QString s, list)
                ui->blackListWidget->appendPlainText(s);
        }
    }
}

void StandalonePreferencesDialog::createWidgetsToNewFolder(QString path)
{
    ScanFolderPanel *panel = new ScanFolderPanel(path);
    connect(panel->getRemoveButton(), SIGNAL(clicked(bool)), this, SLOT(removeVstscanFolder()));
    ui->panelScanFolders->layout()->addWidget(panel);
}

void StandalonePreferencesDialog::addVstFolderToScan(QString folder)
{
    createWidgetsToNewFolder(folder);
    mainController->addPluginsScanPath(folder);
}

// clear the vst cache and run a complete scan
void StandalonePreferencesDialog::scansFully()
{
    if (mainController && mainWindow) {
        // save the config file before start scanning
        mainController->saveLastUserSettings(mainWindow->getInputsSettings());

        // clear
        mainController->clearPluginsCache();
        ui->vstListWidget->clear();

        // scan
        mainController->scanPlugins();
    }
}

// Refresh vsts scanning only the new plugins
void StandalonePreferencesDialog::scanNewPlugins()
{
    if (mainController && mainWindow) {
        // save the config file before start scanning
        mainController->saveLastUserSettings(mainWindow->getInputsSettings());

        // scan only new plugins
        mainController->scanPlugins(true);
    }
}

// ADD A VST IN BLACKLIST
void StandalonePreferencesDialog::addBlackListedPlugins()
{
    QFileDialog vstDialog(this, "Add Vst(s) to BlackBox ...");
    vstDialog.setNameFilter("Dll(*.dll)");// TODO in mac the extension is .vst
    QStringList foldersToScan = mainController->getSettings().getVstScanFolders();
    if (!foldersToScan.isEmpty())
        vstDialog.setDirectory(foldersToScan.first());
    vstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    vstDialog.setFileMode(QFileDialog::ExistingFiles);

    if (vstDialog.exec()) {
        QStringList vstNames = vstDialog.selectedFiles();
        foreach (QString string, vstNames) {
            updateBlackBox(string, true);// add to
            mainController->addBlackVstToSettings(string);
        }
    }
}

void StandalonePreferencesDialog::removeBlackListedPlugins()
{
    QFileDialog vstDialog(this, "Remove Vst(s) from Black List ...");
    vstDialog.setNameFilter("Dll(*.dll)");// TODO mac extension is .vst
    QStringList foldersToScan = mainController->getSettings().getVstScanFolders();
    if (!foldersToScan.isEmpty())
        vstDialog.setDirectory(foldersToScan.first());
    vstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    vstDialog.setFileMode(QFileDialog::ExistingFiles);
    if (vstDialog.exec()) {
        QStringList vstNames = vstDialog.selectedFiles();
        foreach (QString string, vstNames) {
            updateBlackBox(string, false);// Remove from
            mainController->removeBlackVst(0);// index
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

    Midi::MidiDriver *midiDriver = dynamic_cast<StandaloneMainController*>(mainController)->getMidiDriver();
    int maxInputDevices = midiDriver->getMaxInputDevices();
    if (maxInputDevices > 0) {
        QList<bool> midiInputsStatus = mainController->getSettings().getMidiInputDevicesStatus();
        for (int i = 0; i < maxInputDevices; ++i) {
            QCheckBox *checkBox = new QCheckBox(midiDriver->getInputDeviceName(i));
            ui->midiContentPanel->layout()->addWidget(checkBox);
            bool checkedStatus = midiInputsStatus.at(i) || i > midiInputsStatus.size()-1;
            checkBox->setChecked(midiInputsStatus.isEmpty() || checkedStatus);
        }
        QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui->midiContentPanel->layout()->addItem(spacer);
    } else {// no devices detected
        QLabel *label = new QLabel("No midi input device detected!");
        ui->midiContentPanel->layout()->addWidget(label);
        ui->midiContentPanel->layout()->setAlignment(label, Qt::AlignCenter);
    }
}

Audio::AudioDriver* StandalonePreferencesDialog::getAudioDriver(){
    return dynamic_cast<StandaloneMainController*>(mainController)->getAudioDriver();
}

void StandalonePreferencesDialog::populateAudioTab()
{
    populateAsioDriverCombo();
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();

    ui->buttonControlPanel->setVisible(getAudioDriver()->hasControlPanel());
}

void StandalonePreferencesDialog::populateAsioDriverCombo()
{
    Audio::AudioDriver *audioDriver = getAudioDriver();
    int devices = audioDriver->getDevicesCount();
    ui->comboAudioDevice->clear();
    for (int d = 0; d < devices; d++) {
        // using device index as userData in comboBox
        ui->comboAudioDevice->addItem(audioDriver->getAudioDeviceName(d), d);
    }
    ui->comboAudioDevice->setCurrentIndex(audioDriver->getAudioDeviceIndex());
}

void StandalonePreferencesDialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    Audio::AudioDriver *audioDriver = getAudioDriver();
    int maxInputs = audioDriver->getMaxInputs();
    for (int i = 0; i < maxInputs; i++)
        ui->comboFirstInput->addItem(audioDriver->getInputChannelName(i), i);
    int firstInputIndex = audioDriver->getSelectedInputs().getFirstChannel();
    if (firstInputIndex < maxInputs)
        ui->comboFirstInput->setCurrentIndex(firstInputIndex);
    else
        ui->comboFirstInput->setCurrentIndex(0);
}

void StandalonePreferencesDialog::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    Audio::AudioDriver *audioDriver = getAudioDriver();
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

void StandalonePreferencesDialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    Audio::AudioDriver *audioDriver = getAudioDriver();
    int maxOuts = audioDriver->getMaxOutputs();
    for (int i = 0; i < maxOuts; i++)
        ui->comboFirstOutput->addItem(audioDriver->getOutputChannelName(i), i);
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getSelectedOutputs().getFirstChannel());
}

void StandalonePreferencesDialog::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = getAudioDriver()->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if (currentFirstOut + 1 < maxOuts)
        currentFirstOut++;// to avoid 1 channel output

    int items = 0;
    const int MAX_ITEMS = 1;// std::min( maxOuts - currentFirstOut, 2);
    Audio::AudioDriver *audioDriver = getAudioDriver();
    for (int i = currentFirstOut; items < MAX_ITEMS; i++, items++)
        ui->comboLastOutput->addItem(audioDriver->getOutputChannelName(i), i);
    int lastOutputIndex = audioDriver->getSelectedOutputs().getLastChannel();
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex(index >= 0 ? index : 0);

    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void StandalonePreferencesDialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();

    AudioDriver *audioDriver = getAudioDriver();
    QList<int> sampleRates = audioDriver->getValidSampleRates(audioDriver->getAudioDeviceIndex());
    foreach (int sampleRate, sampleRates)
        ui->comboSampleRate->addItem(QString::number(sampleRate), sampleRate);

    ui->comboSampleRate->setCurrentText(QString::number(audioDriver->getSampleRate()));
    ui->comboSampleRate->setEnabled(!sampleRates.isEmpty());
}

void StandalonePreferencesDialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    AudioDriver *audioDriver = getAudioDriver();
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
    Audio::AudioDriver *audioDriver = getAudioDriver();
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
    int sampleRate = ui->comboSampleRate->currentText().toInt();
    int bufferSize = ui->comboBufferSize->currentText().toInt();

    // build midi inputs devices status
    QList<bool> midiInputsStatus;
    QList<QCheckBox *> boxes = ui->midiContentPanel->findChildren<QCheckBox *>();
    foreach (QCheckBox *check, boxes)
        midiInputsStatus.append(check->isChecked());

    QDialog::accept();

    emit ioPreferencesChanged(midiInputsStatus, selectedAudioDevice, firstIn, lastIn, firstOut,
                              lastOut, sampleRate, bufferSize);
}

void StandalonePreferencesDialog::populateVstTab()
{
    clearScanFolderWidgets();// remove all widgets before add the paths
    QStringList scanFoldersList = mainController->getSettings().getVstScanFolders();
    QStringList vstList = mainController->getSettings().getVstPluginsPaths();
    QStringList blackVstList = mainController->getSettings().getBlackListedPlugins();

    // populate the paths
    foreach (QString scanFolder, scanFoldersList)
        createWidgetsToNewFolder(scanFolder);

    // populate the VST list
    ui->vstListWidget->clear();
    foreach (QString path, vstList)
        updateVstList(path);

    // populate the BlackBox
    ui->blackListWidget->clear();
    foreach (QString path, blackVstList)
        updateBlackBox(path, true);// add vst
}

void StandalonePreferencesDialog::selectPreferencesTab(int index)
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

void StandalonePreferencesDialog::openExternalAudioControlPanel()
{
    AudioDriver *audioDriver = getAudioDriver();
    if (audioDriver->hasControlPanel())// just in case
        audioDriver->openControlPanel((void *)mainWindow->winId());
}
