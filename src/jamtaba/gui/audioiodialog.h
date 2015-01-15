#pragma once

#include <QDialog>
#include "../audio/PortAudioDriver.h"
#include <QCloseEvent>

namespace Ui {
class AudioIODialog;
}

//++++++++++++++++++++++++++++++++++
class AudioIODialog : public QDialog
{
    Q_OBJECT

public:
    explicit AudioIODialog(PortAudioDriver &portAudioDriver, QWidget *parent = 0);
    ~AudioIODialog();
    virtual void closeEvent(QCloseEvent *);

private slots:
    void on_comboAsioDriver_currentIndexChanged(int);
    void on_comboFirstInput_currentIndexChanged(int);
    void on_comboFirstOutput_currentIndexChanged(int);

    //void on_okButton_released();

signals:
    void audioIOPropertiesChanged(int selectedDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);

private:
    Ui::AudioIODialog *ui;
    PortAudioDriver* portAudioDriver;

    void populateAsioDriverCombo();
    void populateFirstInputCombo();
    void populateLastInputCombo();
    void populateFirstOutputCombo();
    void populateLastOutputCombo();
    void populateSampleRateCombo();
    void populateBufferSizeCombo();
};

