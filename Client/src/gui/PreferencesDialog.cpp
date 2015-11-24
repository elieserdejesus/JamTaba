#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QCheckBox>
#include "../audio/core/AudioDriver.h"
#include "../midi/MidiDriver.h"
#include "../persistence/Settings.h"
#include "../MainController.h"
#include "MainWindow.h"

using namespace Audio;
using namespace Controller;

PreferencesDialog::PreferencesDialog(Controller::MainController* mainController, MainWindow* mainWindow) :
    QDialog(mainWindow),
    ui(new Ui::IODialog), mainController(mainController), mainWindow(mainWindow)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->comboLastOutput->setEnabled(false);

    if(!mainController->isRunningAsVstPlugin()){
        #ifdef Q_OS_MACX
            ui->comboAudioDevice->setVisible(false);
            ui->asioDriverLabel->setVisible(false);
            ui->groupBoxInputs->setVisible(false);
            ui->groupBoxOutputs->setVisible(false);
        #endif
        QObject::connect( mainController->getPluginFinder(),
                                  SIGNAL(scanFinished()),
                                  this,
                                  SLOT(on_pluginsScanFinished()));
        populateAudioTab();
        populateMidiTab();
        populateVstTab();
    }
    else{
        //remove the first 3 tabs (audio, midi and VSTs)
        ui->prefsTab->removeTab(0);
        ui->prefsTab->removeTab(0);
        ui->prefsTab->removeTab(0);
    }

    populateRecordingTab();
}

void PreferencesDialog::selectAudioTab(){
    if(!mainController->isRunningAsVstPlugin()){
        ui->prefsTab->setCurrentWidget(ui->tabAudio);
    }
}

void PreferencesDialog::selectMidiTab(){
    if(!mainController->isRunningAsVstPlugin()){
        ui->prefsTab->setCurrentWidget(ui->tabMidi);
    }
}
void PreferencesDialog::selectVstPluginsTab(){
    if(!mainController->isRunningAsVstPlugin()){
        ui->prefsTab->setCurrentWidget(ui->tabVST);
    }
}

void PreferencesDialog::selectRecordingTab(){
    ui->prefsTab->setCurrentWidget(ui->tabRecording);
}


void PreferencesDialog::populateMidiTab(){
    if(mainController->isRunningAsVstPlugin()){
        return;
    }
    //clear
    QLayoutItem *item;
    while ((item = ui->midiContentPanel->layout()->takeAt(0)) != 0) {
        if(item->widget()){
            delete item->widget();
        }
        //ui->midiContentPanel->layout()->removeItem(item);
        delete item;
    }

    Midi::MidiDriver* midiDriver = mainController->getMidiDriver();
    int maxInputDevices = midiDriver->getMaxInputDevices();
    if(maxInputDevices > 0){
        QList<bool> midiInputsStatus = mainController->getSettings().getMidiInputDevicesStatus();
        for (int i = 0; i < maxInputDevices; ++i) {
            QCheckBox* checkBox = new QCheckBox(midiDriver->getInputDeviceName(i));
            ui->midiContentPanel->layout()->addWidget( checkBox );
            checkBox->setChecked( midiInputsStatus.isEmpty() || i > midiInputsStatus.size()-1 ||  midiInputsStatus.at(i) );
        }
        ui->midiContentPanel->layout()->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    }
    else{//no devices detected
        QLabel* label = new QLabel("No midi input device detected!");
        ui->midiContentPanel->layout()->addWidget(label);
        ui->midiContentPanel->layout()->setAlignment(label, Qt::AlignCenter);
    }
}

void PreferencesDialog::populateAudioTab(){
    if(mainController->isRunningAsVstPlugin()){
        return;
    }
    populateAsioDriverCombo();
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();
    ui->buttonControlPanel->setVisible(mainController->getAudioDriver()->hasControlPanel());
}

void PreferencesDialog::clearScanFolderWidgets(){
    foreach (QWidget* removeButton, scanFoldersButtons) {
        ui->panelScanFolders->layout()->removeWidget(removeButton->parentWidget());
        delete removeButton->parentWidget();
    }
    scanFoldersButtons.clear();
}

void PreferencesDialog::populateVstTab(){
    if(mainController->isRunningAsVstPlugin()){
        return;
    }
    clearScanFolderWidgets();//remove all widgets before add the paths
    QStringList ScanFoldersList = mainController->getSettings().getVstScanFolders();
    QStringList VstList = mainController->getSettings().getVstPluginsPaths();
    QStringList BlackVstList = mainController->getSettings().getBlackBox();

    //populate the paths
    foreach (QString scanFolder, ScanFoldersList) {
        createWidgetsToNewScanFolder(scanFolder);
    }
     //populate the VST list
    ui->plainTextEdit->clear();
    foreach (QString path, VstList) {
        UpdateVstList(path);
    }
    //populate the BlackBox
   ui->BlackBoxText->clear();
   foreach (QString path, BlackVstList) {
       UpdateBlackBox(path,true);//add vst
   }
}

void PreferencesDialog::populateRecordingTab(){
    QString recordingPath = mainController->getSettings().getRecordingPath();
    QDir recordDir(recordingPath);
    bool isSaveMultiTrackActivated = mainController->getSettings().isSaveMultiTrackActivated();
    ui->recordingCheckBox->setChecked(isSaveMultiTrackActivated);
    ui->recordPathLineEdit->setText(recordDir.absolutePath());
}

PreferencesDialog::~PreferencesDialog(){
    delete ui;
}

void PreferencesDialog::populateAsioDriverCombo(){
    if(mainController->isRunningAsVstPlugin()){
        return;
    }
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    int devices = audioDriver->getDevicesCount();
    ui->comboAudioDevice->clear();
    for(int d=0; d < devices; d++){
        ui->comboAudioDevice->addItem(audioDriver->getAudioDeviceName(d), d);//using device index as userData in comboBox
    }
    ui->comboAudioDevice->setCurrentIndex(audioDriver->getAudioDeviceIndex());

}

void PreferencesDialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    int maxInputs = audioDriver->getMaxInputs();
    for(int i=0; i < maxInputs; i++){
        ui->comboFirstInput->addItem( audioDriver->getInputChannelName(i), i );
    }
    int firstInputIndex = audioDriver->getSelectedInputs().getFirstChannel();
    if(firstInputIndex < maxInputs){
        ui->comboFirstInput->setCurrentIndex(firstInputIndex);
    }
    else{
        ui->comboFirstInput->setCurrentIndex(0);
    }
}

void PreferencesDialog::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    int maxInputs = audioDriver->getMaxInputs();
    int currentFirstInput = ui->comboFirstInput->currentData().toInt();
    int items = 0;
    const int MAX_ITEMS = maxInputs - currentFirstInput;
    for(int i=currentFirstInput; items < MAX_ITEMS; i++, items++){
        ui->comboLastInput->addItem( audioDriver->getInputChannelName(i), i );
    }
    ChannelRange inputsRange = audioDriver->getSelectedInputs();
    int lastInputIndex = inputsRange.getLastChannel();
    if( lastInputIndex < ui->comboLastInput->count()){
        ui->comboLastInput->setCurrentIndex(lastInputIndex);
    }
    else{
        ui->comboLastInput->setCurrentIndex(ui->comboLastInput->count()-1);
    }
}

void PreferencesDialog::populateFirstOutputCombo(){
    ui->comboFirstOutput->clear();
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    int maxOuts = audioDriver->getMaxOutputs();
    for(int i=0; i < maxOuts; i++){
        ui->comboFirstOutput->addItem( audioDriver->getOutputChannelName(i), i );
    }
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getSelectedOutputs().getFirstChannel());
}

void PreferencesDialog::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = mainController->getAudioDriver()->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if(currentFirstOut + 1 < maxOuts){
        currentFirstOut++;//to avoid 1 channel output
    }
    int items = 0;
    const int MAX_ITEMS = 1;//std::min( maxOuts - currentFirstOut, 2);
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    for(int i=currentFirstOut; items < MAX_ITEMS; i++, items++){
        ui->comboLastOutput->addItem( audioDriver->getOutputChannelName(i), i);
    }
    int lastOutputIndex = audioDriver->getSelectedOutputs().getLastChannel();
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex( index >=0 ? index : 0);
}

void PreferencesDialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();

    AudioDriver* audioDriver = mainController->getAudioDriver();
    QList<int> sampleRates = audioDriver->getValidSampleRates( audioDriver->getAudioDeviceIndex());
    foreach (int sampleRate, sampleRates) {
        ui->comboSampleRate->addItem(QString::number(sampleRate), sampleRate);
    }
    ui->comboSampleRate->setCurrentText(QString::number( audioDriver->getSampleRate()));

    ui->comboSampleRate->setEnabled(!sampleRates.isEmpty());
}

void PreferencesDialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    AudioDriver* audioDriver = mainController->getAudioDriver();
    QList<int> bufferSizes = audioDriver->getValidBufferSizes(audioDriver->getAudioDeviceIndex());
    foreach (int size, bufferSizes) {
        ui->comboBufferSize->addItem(QString::number(size), size);
    }
    ui->comboBufferSize->setCurrentText(QString::number(audioDriver->getBufferSize()));
    ui->comboBufferSize->setEnabled(!bufferSizes.isEmpty());
}
//++++++++++++
void PreferencesDialog::on_comboAudioDevice_activated(int index)
{
    int deviceIndex = ui->comboAudioDevice->itemData(index).toInt();
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    audioDriver->setAudioDeviceIndex(deviceIndex);
//#ifdef _WIN32
//    audioDriver->setOutputDeviceIndex(deviceIndex);
//#endif
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
    Audio::AudioDriver* audioDriver = mainController->getAudioDriver();
    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void PreferencesDialog::on_okButton_released()
{
    if(mainController->isRunningAsVstPlugin()){
        this->accept();
        return;
    }
    int selectedAudioDevice = ui->comboAudioDevice->currentIndex();
    int firstIn = ui->comboFirstInput->currentData().toInt();
    int lastIn = ui->comboLastInput->currentData().toInt();
    int firstOut = ui->comboFirstOutput->currentData().toInt();
    int lastOut = ui->comboLastOutput->currentData().toInt();
    int sampleRate = ui->comboSampleRate->currentText().toInt();
    int bufferSize = ui->comboBufferSize->currentText().toInt();

    //build midi inputs devices status
    QList<bool> midiInputsStatus;
    QList<QCheckBox*> boxes = ui->midiContentPanel->findChildren<QCheckBox*>();
    foreach (QCheckBox* check, boxes) {
        midiInputsStatus.append(check->isChecked());
    }

    this->accept();

    emit ioPreferencesChanged(midiInputsStatus, selectedAudioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
}

void PreferencesDialog::on_prefsTab_currentChanged(int index)
{
    if(mainController->isRunningAsVstPlugin()){
        populateRecordingTab();
        return;
    }

    switch(index){
        case 0: populateAudioTab(); break;
        case 1: populateMidiTab(); break;
        case 2: populateVstTab(); break;
        case 3: populateRecordingTab(); break;
    }
}


//VST

void PreferencesDialog::on_pluginsScanFinished(){
    populateVstTab();
}
void PreferencesDialog::on_buttonAddVstScanFolder_clicked()
{
    QFileDialog fileDialog(this, "Adding VST path ...");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if(fileDialog.exec() ){
        QDir dir = fileDialog.directory();
        addVstScanPath(dir.absolutePath());
    }
}

void PreferencesDialog::on_buttonRemoveVstPathClicked(){
    QPushButton* buttonClicked = (QPushButton*)sender();
    //find the index of button clicked
    int buttonIndex = 0;
    while(scanFoldersButtons.at(buttonIndex) != buttonClicked){
        buttonIndex++;
    }
    if(buttonIndex < scanFoldersButtons.size()){
        //ui->panelPaths->layout()->removeWidget(scanPathButtons.at(buttonIndex));
        ui->panelScanFolders->layout()->removeWidget(buttonClicked->parentWidget());
        scanFoldersButtons.removeOne(buttonClicked);
        delete buttonClicked->parentWidget();
        mainController->removePluginsScanPath(buttonIndex);
    }
}

void PreferencesDialog::UpdateVstList(QString path)
{
    ui->plainTextEdit->appendPlainText(path);
}
void PreferencesDialog::UpdateBlackBox(QString path, bool add)
{
    if(add)
    ui->BlackBoxText->appendPlainText(path);
    else
    {
       QString str=ui->BlackBoxText->toPlainText();
            if(str.contains(path))
            {
              ui->BlackBoxText->clear();
              mainController->removeBlackVst(str.indexOf(path));
              QStringList list=mainController->getSettings().getBlackBox();
               foreach(QString s,list)
               {
                 ui->BlackBoxText->appendPlainText(s);
               }
             }
     }

}
void PreferencesDialog::createWidgetsToNewScanFolder(QString path){
    QVBoxLayout* panelLayout = (QVBoxLayout*)ui->panelScanFolders->layout();
    QWidget* parent = new QWidget(this);
    QHBoxLayout* lineLayout = new QHBoxLayout(parent);
    lineLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton* removeButton = new QPushButton(QIcon(":/images/less.png"),"", parent);
    removeButton->setToolTip("Remove this folder from scanning");
    QObject::connect( removeButton, SIGNAL(clicked()), this, SLOT(on_buttonRemoveVstPathClicked()));
    lineLayout->addWidget(removeButton);
    lineLayout->addWidget( new QLabel(path, this), 1.0);
    panelLayout->addWidget(parent);
    scanFoldersButtons.push_back(removeButton);//save the widget to easy remove all widgets when the VSt Scan path tab is populated

}

void PreferencesDialog::addVstScanPath(QString path){
    createWidgetsToNewScanFolder( path );
    mainController->addPluginsScanPath(path);
}

void PreferencesDialog::on_buttonClearVstCache_clicked()
{
    mainController->clearPluginsCache();
    ui->plainTextEdit->clear();
}

//void PreferencesDialog::on_buttonScanVSTs_clicked()
//{
//    if(mainController){
//        if(mainWindow){//save the config file before start scanning
//            mainController->saveLastUserSettings(mainWindow->getInputsSettings());
//        }
//        mainController->scanPlugins();
//    }
//}


//REFRESH VST LIST
void PreferencesDialog::on_ButtonVst_Refresh_clicked()
{
    if(mainController)
    {
        if(mainWindow)
        {//save the config file before start scanning
            mainController->saveLastUserSettings(mainWindow->getInputsSettings());
        }
        //clear the cache
        mainController->clearPluginsCache();
        //scan again
        mainController->scanPlugins();
    }

    populateVstTab();
}

// ADD A VST IN BLACKLIST
void PreferencesDialog::on_ButtonVST_AddToBlackList_clicked()
{
    QFileDialog VstDialog(this, "Add Vst(s) to BlackBox ...");
    VstDialog.setNameFilter(tr("Dll(*.dll)"));
    QStringList foldersToScan = mainController->getSettings().getVstScanFolders();
    if(!foldersToScan.isEmpty()){
        VstDialog.setDirectory(foldersToScan.first());
    }
    VstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    VstDialog.setFileMode(QFileDialog::ExistingFiles);
    if(VstDialog.exec() )
    {
        QStringList VstNames = VstDialog.selectedFiles();
        foreach (QString string, VstNames)
        {
         UpdateBlackBox(string,true);//add to
         mainController->addBlackVstToSettings(string);
        }

     }
}
void PreferencesDialog::on_ButtonVST_RemFromBlkList_clicked()
{
    QFileDialog VstDialog(this, "Remove Vst(s) to BlackBox ...");
    VstDialog.setNameFilter(tr("Dll(*.dll)"));
    QStringList foldersToScan = mainController->getSettings().getVstScanFolders();
    if(!foldersToScan.isEmpty()){
        VstDialog.setDirectory(foldersToScan.first());
    }
    VstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    VstDialog.setFileMode(QFileDialog::ExistingFiles);
    if(VstDialog.exec() )
    {
        QStringList VstNames = VstDialog.selectedFiles();
        foreach (QString string, VstNames)
        {
         UpdateBlackBox(string,false);//Remove from
         mainController->removeBlackVst(0);//index
        }

     }

}

//Recording TAB controls --------------------
void PreferencesDialog::on_browseRecPathButton_clicked(){
    QFileDialog fileDialog(this, "Choosing recording path ...");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if(fileDialog.exec() ){
        QDir dir = fileDialog.directory();
        mainController->storeRecordingPath(dir.absolutePath());
        ui->recordPathLineEdit->setText(dir.absolutePath());
    }
}

void PreferencesDialog::on_recordingCheckBox_clicked(){
    mainController->storeRecordingMultiTracksStatus( ui->recordingCheckBox->isChecked());
}
//+++++++++++++++++++++++++++

void PreferencesDialog::on_buttonControlPanel_clicked()
{
    AudioDriver* audioDriver = mainController->getAudioDriver();
    if(audioDriver->hasControlPanel()){//just in case
        audioDriver->openControlPanel((void*)mainWindow->winId());
    }
}
