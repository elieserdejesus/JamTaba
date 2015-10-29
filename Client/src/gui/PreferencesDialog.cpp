#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QCheckBox>
#include "../audio/core/AudioDriver.h"
#include "../midi/MidiDriver.h"
#include "../persistence/Settings.h"
#include "../MainController.h"
//#include "../StandAloneMainController.h"

using namespace Audio;
using namespace Controller;

PreferencesDialog::PreferencesDialog(Controller::MainController* mainController, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IODialog), mainController(mainController)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    //populateAudioTab();
    //populateMidiInputCombo();

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
                                  SLOT(onPluginsScanFinished));
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
}

void PreferencesDialog::clearScanPathWidgets(){
    foreach (QWidget* removeButton, scanPathButtons) {
        ui->panelPaths->layout()->removeWidget(removeButton->parentWidget());
        delete removeButton->parentWidget();
    }
    scanPathButtons.clear();
}

void PreferencesDialog::populateVstTab(){
    if(mainController->isRunningAsVstPlugin()){
        return;
    }
    clearScanPathWidgets();//remove all widgets before add the paths
    QStringList paths = mainController->getSettings().getVstScanPaths();
    QStringList VstList = mainController->getSettings().getVstPluginsPaths();
    //populate the paths
    foreach (QString path, paths) {
        createWidgetsToNewScanPath(path);
    }
     //populate the VST
    foreach (QString path, VstList) {
        createWidgetsToVstList(path);
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
        ui->comboAudioDevice->addItem(audioDriver->getInputDeviceName(d), d);//using device index as userData in comboBox
    }
    ui->comboAudioDevice->setCurrentIndex(audioDriver->getInputDeviceIndex());

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
    QList<int> sampleRates = audioDriver->getValidSampleRates( audioDriver->getOutputDeviceIndex());
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
    QList<int> bufferSizes = audioDriver->getValidBufferSizes(audioDriver->getInputDeviceIndex());
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
    audioDriver->setInputDeviceIndex(deviceIndex);
#ifdef _WIN32
    audioDriver->setOutputDeviceIndex(deviceIndex);
#endif
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
void PreferencesDialog::onPluginsScanFinished(){
    populateVstTab();
}
void PreferencesDialog::on_buttonAddVstPath_clicked()
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
    while(scanPathButtons.at(buttonIndex) != buttonClicked){
        buttonIndex++;
    }
    if(buttonIndex < scanPathButtons.size()){
        //ui->panelPaths->layout()->removeWidget(scanPathButtons.at(buttonIndex));
        ui->panelPaths->layout()->removeWidget(buttonClicked->parentWidget());
        scanPathButtons.removeOne(buttonClicked);
        delete buttonClicked->parentWidget();
        mainController->removePluginsScanPath(buttonIndex);
    }
}

void PreferencesDialog::createWidgetsToVstList(QString path){
    ui->plainTextEdit->appendPlainText(path);
    }
void PreferencesDialog::createWidgetsToNewScanPath(QString path){
    QVBoxLayout* panelLayout = (QVBoxLayout*)ui->panelPaths->layout();
    QWidget* parent = new QWidget(this);
    QHBoxLayout* lineLayout = new QHBoxLayout(parent);
    QPushButton* removeButton = new QPushButton("Remove", this);
    QObject::connect( removeButton, SIGNAL(clicked()), this, SLOT(on_buttonRemoveVstPathClicked()));
    lineLayout->addWidget(removeButton);
    lineLayout->addWidget( new QLabel(path, this), 1.0);
    panelLayout->removeItem(ui->verticalSpacer_2);
    int index = panelLayout->count();//the last widget is a spacer, add before spacer
    panelLayout->insertWidget(index, parent);
    panelLayout->addSpacerItem(ui->verticalSpacer_2);

    scanPathButtons.push_back(removeButton);//save the widget to easy remove all widgets when the VSt Scan path tab is populated
}

void PreferencesDialog::addVstScanPath(QString path){
    createWidgetsToNewScanPath( path );
    mainController->addPluginsScanPath(path);
}

void PreferencesDialog::on_buttonClearVstCache_clicked()
{
    mainController->clearPluginsCache();
}

void PreferencesDialog::on_buttonScanVSTs_clicked()
{
    if(mainController){
        mainController->scanPlugins();
        //from here we should fill the vst list ?
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



