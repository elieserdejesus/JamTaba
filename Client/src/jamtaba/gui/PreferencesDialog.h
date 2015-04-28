#pragma once
#include <QDialog>
#include <QCloseEvent>

namespace Audio {    class AudioDriver; }
namespace Ui    {       class IODialog; }
namespace Midi  {    class MidiDriver;  }

//++++++++++++++++++++++++++++++++++
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(Audio::AudioDriver* audioDriver, Midi::MidiDriver* midiDriver, QWidget *parent = 0);
    ~PreferencesDialog();
   // virtual void closeEvent(QCloseEvent *);

private slots:
    //void on_comboAsioDriver_currentIndexChanged(int);
    void on_comboFirstInput_currentIndexChanged(int);
    void on_comboFirstOutput_currentIndexChanged(int);
    //void on_comboAsioDriver_activated(const QString &arg1);
    void on_comboAsioDriver_activated(int index);
    void on_okButton_released();

    void on_prefsTab_currentChanged(int index);
    void on_buttonAddVstPath_clicked();
    void on_buttonRemoveVstPathClicked();

signals:
    void ioChanged(int selectedMidiDevice, int selectedAudioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
private:
    Ui::IODialog *ui;
    Audio::AudioDriver* audioDriver;
    Midi::MidiDriver* midiDriver;

    //AUDIO
    void populateAsioDriverCombo();
    void populateFirstInputCombo();
    void populateLastInputCombo();
    void populateFirstOutputCombo();
    void populateLastOutputCombo();
    void populateSampleRateCombo();
    void populateBufferSizeCombo();
    void populateAudioTab();
    void populateVstTab();

    //void clearLayout(QLayout* layout, bool deleteWidgets = true);

    //MIDI
    void populateMidiInputCombo();

    //VST
    void addVstScanPath(QString path);
    void createWidgetsToNewScanPath(QString path);
    void clearScanPathWidgets();
    QList<QPushButton*> scanPathButtons;
    //void removeVstScanPath(QString path);
};


