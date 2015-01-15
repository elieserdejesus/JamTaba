#include "audioiodialog.h"
#include "ui_audioiodialog.h"
#include <QDebug>

AudioIODialog::AudioIODialog(PortAudioDriver &driver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioIODialog), portAudioDriver(&driver)
{
    ui->setupUi(this);
    setModal(true);
    populateAsioDriverCombo();
    populateFirstInputCombo();
    populateFirstOutputCombo();
    populateSampleRateCombo();
    populateBufferSizeCombo();
}

AudioIODialog::~AudioIODialog()
{
    delete ui;
}

void AudioIODialog::populateAsioDriverCombo()
{
    int devices = this->portAudioDriver->getDevicesCount();
    ui->comboAsioDriver->clear();
    for(int d=0; d < devices; d++){
        ui->comboAsioDriver->addItem(portAudioDriver->getInputDeviceName(d), d);//using device index as userData in comboBox
    }
    qDebug() << "setando combo para inputDeviceIndex: " << portAudioDriver->getInputDeviceIndex() << " outputDeviceIndex: " << portAudioDriver->getOutputDeviceIndex();
    ui->comboAsioDriver->setCurrentIndex(portAudioDriver->getInputDeviceIndex());

}

void AudioIODialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    int maxInputs = portAudioDriver->getMaxInputs();
    for(int i=0; i < maxInputs; i++){
        ui->comboFirstInput->addItem( portAudioDriver->getInputChannelName(i), i );
    }
    if(portAudioDriver->getFirstInput() < maxInputs){
        ui->comboFirstInput->setCurrentIndex(portAudioDriver->getFirstInput());
    }
    else{
        ui->comboFirstInput->setCurrentIndex(0);
    }
}

void AudioIODialog::populateLastInputCombo()
{
    ui->comboLastInput->clear();
    int maxInputs = portAudioDriver->getMaxInputs();
    int currentFirstInput = ui->comboFirstInput->currentData().toInt();
    for(int i=currentFirstInput; i < maxInputs; i++){
        ui->comboLastInput->addItem( portAudioDriver->getInputChannelName(i), i );
    }
    int lastInputIndex = portAudioDriver->getFirstInput() + portAudioDriver->getInputs() -1;
    if( lastInputIndex < ui->comboLastInput->count()){
        ui->comboLastInput->setCurrentIndex(lastInputIndex);
    }
    else{
        ui->comboLastInput->setCurrentIndex(ui->comboLastInput->count()-1);
    }
}

void AudioIODialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    int maxOuts = portAudioDriver->getMaxOutputs();
    for(int i=0; i < maxOuts; i++){
        ui->comboFirstOutput->addItem( portAudioDriver->getOutputChannelName(i), i );
    }
    ui->comboFirstOutput->setCurrentIndex(portAudioDriver->getFirstOutput());
}

void AudioIODialog::populateLastOutputCombo()
{
    ui->comboLastOutput->clear();
    int maxOuts = portAudioDriver->getMaxOutputs();
    int currentFirstOut = ui->comboFirstOutput->currentData().toInt();
    if(currentFirstOut + 1 < maxOuts){
        currentFirstOut++;//to avoid 1 channel output
    }
    for(int i=currentFirstOut; i < maxOuts; i++){
        ui->comboLastOutput->addItem( portAudioDriver->getOutputChannelName(i), i);
    }
    int lastOutputIndex = portAudioDriver->getFirstOutput() + portAudioDriver->getOutputs() -1;
    int index = ui->comboLastOutput->findData(lastOutputIndex);
    ui->comboLastOutput->setCurrentIndex( index >=0 ? index : 0);

//    if( lastOutputIndex < ui->comboLastOutput->count()){
//        ui->comboLastOutput->setCurrentIndex(lastOutputIndex);
//    }
//    else{
//        ui->comboLastOutput->setCurrentIndex(ui->comboLastOutput->count()-1);
//    }
}

void AudioIODialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();
    ui->comboSampleRate->addItem("44100", 44100);
    ui->comboSampleRate->addItem("48000", 48000);
    ui->comboSampleRate->addItem("96000", 96000);
    ui->comboSampleRate->addItem("192000", 192000);

    ui->comboSampleRate->setCurrentText(QString::number( portAudioDriver->getSampleRate()));
}

void AudioIODialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    ui->comboBufferSize->addItem("64", 64);
    ui->comboBufferSize->addItem("128", 128);
    ui->comboBufferSize->addItem("256", 256);
    ui->comboBufferSize->addItem("512", 512);
    ui->comboBufferSize->addItem("1024", 1024);
    ui->comboBufferSize->addItem("2048", 2048);
    ui->comboBufferSize->setCurrentText(QString::number(portAudioDriver->getBufferSize()));
    //ui->comboBufferSize->addItem("4096", 4096);
}

//++++++++++++

void AudioIODialog::on_comboAsioDriver_activated(int index)
{
    int deviceIndex = ui->comboAsioDriver->itemData(index).toInt();
    portAudioDriver->setInputDeviceIndex(deviceIndex);
#ifdef _WIN32
    portAudioDriver->setOutputDeviceIndex(deviceIndex);
#endif
    populateFirstInputCombo();
    populateFirstOutputCombo();
    ui->groupBoxOutputs->setEnabled(portAudioDriver->getMaxOutputs() > 2);
}

void AudioIODialog::on_comboFirstInput_currentIndexChanged(int /*index*/)
{
    populateLastInputCombo();
}

void AudioIODialog::on_comboFirstOutput_currentIndexChanged(int /*index*/)
{
    populateLastOutputCombo();
}

void AudioIODialog::closeEvent(QCloseEvent *)
{
    int selectedDevice = ui->comboAsioDriver->currentIndex();
    int firstIn = ui->comboFirstInput->currentData().toInt();
    int lastIn = ui->comboLastInput->currentData().toInt();
    int firstOut = ui->comboFirstOutput->currentData().toInt();
    int lastOut = ui->comboLastOutput->currentData().toInt();
    int sampleRate = ui->comboSampleRate->currentText().toInt();
    int bufferSize = ui->comboBufferSize->currentText().toInt();
    emit audioIOPropertiesChanged(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);

}



