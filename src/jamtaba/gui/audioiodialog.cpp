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
}

void AudioIODialog::populateFirstInputCombo()
{
    ui->comboFirstInput->clear();
    int maxInputs = portAudioDriver->getMaxInputs();
    for(int i=0; i < maxInputs; i++){
        ui->comboFirstInput->addItem( portAudioDriver->getInputChannelName(i), i );
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
}

void AudioIODialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    int maxOuts = portAudioDriver->getMaxOutputs();
    for(int i=0; i < maxOuts; i++){
        ui->comboFirstOutput->addItem( portAudioDriver->getOutputChannelName(i), i );
    }
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
}

void AudioIODialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();
    ui->comboSampleRate->addItem("44100");
    ui->comboSampleRate->addItem("48000");
    ui->comboSampleRate->addItem("96000");
    ui->comboSampleRate->addItem("192000");
}

void AudioIODialog::populateBufferSizeCombo()
{
    ui->comboBufferSize->clear();
    ui->comboBufferSize->addItem("64");
    ui->comboBufferSize->addItem("128");
    ui->comboBufferSize->addItem("256");
    ui->comboBufferSize->addItem("512");
    ui->comboBufferSize->addItem("1024");
    ui->comboBufferSize->addItem("2048");
    ui->comboBufferSize->addItem("4096");
}

//++++++++++++

void AudioIODialog::on_comboAsioDriver_currentIndexChanged(int index)
{
    int deviceIndex = ui->comboAsioDriver->itemData(index).toInt();
    portAudioDriver->setInputDeviceIndex(deviceIndex);
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
   qDebug() << "closeEvent";
    PaDeviceIndex device = portAudioDriver->getInputDeviceIndex();
    int firstIn = ui->comboFirstInput->currentData().toInt();
    int lastIn = ui->comboLastInput->currentData().toInt();
    int firstOut = ui->comboFirstOutput->currentData().toInt();
    int lastOut = ui->comboLastOutput->currentData().toInt();
    int sampleRate = ui->comboSampleRate->currentText().toInt();
    int bufferSize = ui->comboBufferSize->currentText().toInt();
    emit audioIOPropertiesChanged(device, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);

}


