#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include "../audio/core/AudioDriver.h"
#include "../midi/MidiDriver.h"
#include "../persistence/ConfigStore.h"

using namespace Audio;

PreferencesDialog::PreferencesDialog(AudioDriver* driver, Midi::MidiDriver *midiDriver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IODialog), audioDriver(driver), midiDriver(midiDriver)
{
    ui->setupUi(this);
    setModal(true);

    //audio
    //populateAudioTab();

    //midi
    //populateMidiInputCombo();
    ui->prefsTab->setCurrentIndex(0);
    populateAudioTab();
}

void PreferencesDialog::populateMidiInputCombo(){
    int maxDevices = midiDriver->getMaxInputDevices();
    for (int i = 0; i < maxDevices; ++i) {
        ui->comboMidiInput->addItem( QString( midiDriver->getInputDeviceName(i)));
    }
}

void PreferencesDialog::populateAudioTab(){
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
    qDebug() << "Settings: " << Persistence::ConfigStore::getSettingsFilePath();
    clearScanPathWidgets();//remove all widgets before add the paths
    QStringList paths = Persistence::ConfigStore::getVstScanPaths();
    foreach (QString path, paths) {
        createWidgetsToNewScanPath(path);
    }
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::populateAsioDriverCombo()
{
    int devices = this->audioDriver->getDevicesCount();
    ui->comboAsioDriver->clear();
    for(int d=0; d < devices; d++){
        ui->comboAsioDriver->addItem(audioDriver->getInputDeviceName(d), d);//using device index as userData in comboBox
    }
    qDebug() << "setando combo para inputDeviceIndex: " << audioDriver->getInputDeviceIndex() << " outputDeviceIndex: " << audioDriver->getOutputDeviceIndex();
    ui->comboAsioDriver->setCurrentIndex(audioDriver->getInputDeviceIndex());

}

void PreferencesDialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    int maxInputs = 2;//portAudioDriver->getMaxInputs();
    for(int i=0; i < maxInputs; i++){
        ui->comboFirstInput->addItem( audioDriver->getInputChannelName(i), i );
    }
    if(audioDriver->getFirstInput() < maxInputs){
        ui->comboFirstInput->setCurrentIndex(audioDriver->getFirstInput());
    }
    else{
        ui->comboFirstInput->setCurrentIndex(0);
    }
}

void PreferencesDialog::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    int maxInputs = 2;//portAudioDriver->getMaxInputs();
    int currentFirstInput = ui->comboFirstInput->currentData().toInt();
    int items = 0;
    const int MAX_ITEMS = maxInputs - currentFirstInput;
    for(int i=currentFirstInput; items < MAX_ITEMS; i++, items++){
        ui->comboLastInput->addItem( audioDriver->getInputChannelName(i), i );
    }
    int lastInputIndex = audioDriver->getFirstInput() + audioDriver->getInputs() -1;
    if( lastInputIndex < ui->comboLastInput->count()){
        ui->comboLastInput->setCurrentIndex(lastInputIndex);
    }
    else{
        ui->comboLastInput->setCurrentIndex(ui->comboLastInput->count()-1);
    }
}

void PreferencesDialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    int maxOuts = 2;//portAudioDriver->getMaxOutputs();
    for(int i=0; i < maxOuts; i++){
        ui->comboFirstOutput->addItem( audioDriver->getOutputChannelName(i), i );
    }
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getFirstOutput());
}

void PreferencesDialog::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = 2;//portAudioDriver->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if(currentFirstOut + 1 < maxOuts){
        currentFirstOut++;//to avoid 1 channel output
    }
    int items = 0;
    const int MAX_ITEMS = maxOuts - currentFirstOut;
    for(int i=currentFirstOut; items < MAX_ITEMS; i++, items++){
        ui->comboLastOutput->addItem( audioDriver->getOutputChannelName(i), i);
    }
    int lastOutputIndex = audioDriver->getFirstOutput() + audioDriver->getOutputs() -1;
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex( index >=0 ? index : 0);
}

void PreferencesDialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();
    ui->comboSampleRate->addItem("44100", 44100);
    ui->comboSampleRate->addItem("48000", 48000);
    ui->comboSampleRate->addItem("96000", 96000);
    ui->comboSampleRate->addItem("192000", 192000);

    ui->comboSampleRate->setCurrentText(QString::number( audioDriver->getSampleRate()));
}

void PreferencesDialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    ui->comboBufferSize->addItem("64", 64);
    ui->comboBufferSize->addItem("128", 128);
    ui->comboBufferSize->addItem("256", 256);
    ui->comboBufferSize->addItem("512", 512);
    ui->comboBufferSize->addItem("1024", 1024);
    ui->comboBufferSize->addItem("2048", 2048);
    ui->comboBufferSize->setCurrentText(QString::number(audioDriver->getBufferSize()));
    //ui->comboBufferSize->addItem("4096", 4096);
}

//++++++++++++

void PreferencesDialog::on_comboAsioDriver_activated(int index)
{
    int deviceIndex = ui->comboAsioDriver->itemData(index).toInt();
    audioDriver->setInputDeviceIndex(deviceIndex);
#ifdef _WIN32
    audioDriver->setOutputDeviceIndex(deviceIndex);
#endif
    populateFirstInputCombo();
    populateFirstOutputCombo();

}

void PreferencesDialog::on_comboFirstInput_currentIndexChanged(int /*index*/)
{
    populateLastInputCombo();
}

void PreferencesDialog::on_comboFirstOutput_currentIndexChanged(int /*index*/)
{
    populateLastOutputCombo();
    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void PreferencesDialog::on_okButton_released()
{
    int selectedAudioDevice = ui->comboAsioDriver->currentIndex();
    int firstIn = ui->comboFirstInput->currentData().toInt();
    int lastIn = ui->comboLastInput->currentData().toInt();
    int firstOut = ui->comboFirstOutput->currentData().toInt();
    int lastOut = ui->comboLastOutput->currentData().toInt();
    int sampleRate = ui->comboSampleRate->currentText().toInt();
    int bufferSize = ui->comboBufferSize->currentText().toInt();
    int selectedMidiDevice = (ui->comboMidiInput->count() > 0) ? ui->comboMidiInput->currentIndex() : -1;
    emit ioChanged(selectedMidiDevice, selectedAudioDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
}

void PreferencesDialog::on_prefsTab_currentChanged(int index)
{
    switch(index){
        case 0: populateAudioTab(); break;
        case 1: populateMidiInputCombo(); break;
        case 2: populateVstTab(); break;
    }
}


//VST
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
        Persistence::ConfigStore::removeVstScanPath(buttonIndex);
    }
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
    Persistence::ConfigStore::addVstScanPath(path);
}

//void PreferencesDialog::clearLayout(QLayout* layout, bool deleteWidgets)
//{
//    while (QLayoutItem* item = layout->takeAt(0))
//    {
//        if (deleteWidgets)
//        {
//            if (QWidget* widget = item->widget())
//                delete widget;
//        }
//        if (QLayout* childLayout = item->layout())
//            clearLayout(childLayout, deleteWidgets);
//        delete item;
//    }
//}


