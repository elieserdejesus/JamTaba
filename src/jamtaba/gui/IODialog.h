#pragma once
#include <QDialog>
#include <QCloseEvent>

namespace Audio{ class AudioDriver;}
namespace Ui{ class IODialog; }


//++++++++++++++++++++++++++++++++++
class AudioIODialog : public QDialog
{
    Q_OBJECT

public:
    AudioIODialog(Audio::AudioDriver* audioDriver, QWidget *parent = 0);
    ~AudioIODialog();
   // virtual void closeEvent(QCloseEvent *);

private slots:
    //void on_comboAsioDriver_currentIndexChanged(int);
    void on_comboFirstInput_currentIndexChanged(int);
    void on_comboFirstOutput_currentIndexChanged(int);
    //void on_comboAsioDriver_activated(const QString &arg1);
    void on_comboAsioDriver_activated(int index);
    void on_okButton_released();

signals:
    void audioIOPropertiesChanged(int selectedDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);

private:
    Ui::IODialog *ui;
    Audio::AudioDriver* audioDriver;

    void populateAsioDriverCombo();
    void populateFirstInputCombo();
    void populateLastInputCombo();
    void populateFirstOutputCombo();
    void populateLastOutputCombo();
    void populateSampleRateCombo();
    void populateBufferSizeCombo();
};


