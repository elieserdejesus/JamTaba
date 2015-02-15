#include "audioiodialog.h"
#include "ui_audioiodialog.h"

#include <QDebug>
#include "../audio/core/AudioDriver.h"

using namespace Audio;

AudioIODialog::AudioIODialog(AudioDriver* driver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AudioIODialog), audioDriver(driver)
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
    int devices = this->audioDriver->getDevicesCount();
    ui->comboAsioDriver->clear();
    for(int d=0; d < devices; d++){
        ui->comboAsioDriver->addItem(audioDriver->getInputDeviceName(d), d);//using device index as userData in comboBox
    }
    qDebug() << "setando combo para inputDeviceIndex: " << audioDriver->getInputDeviceIndex() << " outputDeviceIndex: " << audioDriver->getOutputDeviceIndex();
    ui->comboAsioDriver->setCurrentIndex(audioDriver->getInputDeviceIndex());

}

void AudioIODialog::populateFirstInputCombo()
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

void AudioIODialog::populateLastInputCombo()
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

void AudioIODialog::populateFirstOutputCombo()
{
    ui->comboFirstOutput->clear();
    int maxOuts = 2;//portAudioDriver->getMaxOutputs();
    for(int i=0; i < maxOuts; i++){
        ui->comboFirstOutput->addItem( audioDriver->getOutputChannelName(i), i );
    }
    ui->comboFirstOutput->setCurrentIndex(audioDriver->getFirstOutput());
}

void AudioIODialog::populateLastOutputCombo()
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

void AudioIODialog::populateSampleRateCombo()
{
    ui->comboSampleRate->clear();
    ui->comboSampleRate->addItem("44100", 44100);
    ui->comboSampleRate->addItem("48000", 48000);
    ui->comboSampleRate->addItem("96000", 96000);
    ui->comboSampleRate->addItem("192000", 192000);

    ui->comboSampleRate->setCurrentText(QString::number( audioDriver->getSampleRate()));
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
    ui->comboBufferSize->setCurrentText(QString::number(audioDriver->getBufferSize()));
    //ui->comboBufferSize->addItem("4096", 4096);
}

//++++++++++++

void AudioIODialog::on_comboAsioDriver_activated(int index)
{
    int deviceIndex = ui->comboAsioDriver->itemData(index).toInt();
    audioDriver->setInputDeviceIndex(deviceIndex);
#ifdef _WIN32
    audioDriver->setOutputDeviceIndex(deviceIndex);
#endif
    populateFirstInputCombo();
    populateFirstOutputCombo();

}

void AudioIODialog::on_comboFirstInput_currentIndexChanged(int /*index*/)
{
    populateLastInputCombo();
}

void AudioIODialog::on_comboFirstOutput_currentIndexChanged(int /*index*/)
{
    populateLastOutputCombo();
    ui->groupBoxOutputs->setEnabled(audioDriver->getMaxOutputs() > 2);
}

void AudioIODialog::on_okButton_released()
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
