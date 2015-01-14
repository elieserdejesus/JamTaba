#pragma once

#include <QDialog>
#include "../audio/PortAudioDriver.h"

namespace Ui {
class AudioIODialog;
}

class AudioIODialog : public QDialog
{
    Q_OBJECT

public:
    explicit AudioIODialog(PortAudioDriver &portAudioDriver, QWidget *parent = 0);
    ~AudioIODialog();

private slots:
    void on_comboAsioDriver_currentIndexChanged(int);

    void on_comboFirstInput_currentIndexChanged(int);

    void on_comboFirstOutput_currentIndexChanged(int);

    //void on_okButton_clicked();

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

