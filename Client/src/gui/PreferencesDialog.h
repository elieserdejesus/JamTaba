#pragma once
#include <QDialog>
#include <QMap>

namespace Audio {    class AudioDriver; }
namespace Ui    {    class IODialog; }
namespace Midi  {    class MidiDriver;  }
namespace Controller{class MainController; }

class MainWindow;

//++++++++++++++++++++++++++++++++++
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(Controller::MainController* mainController, MainWindow* mainWindow);
    ~PreferencesDialog();
    void selectAudioTab();
    void selectMidiTab();
    void selectVstPluginsTab();
    void selectRecordingTab();
private slots:
    void on_comboFirstInput_currentIndexChanged(int);
    void on_comboFirstOutput_currentIndexChanged(int);
    void on_comboAudioDevice_activated(int index);
    void on_okButton_released();

    void on_prefsTab_currentChanged(int index);

    void on_buttonAddVstScanFolder_clicked();

    void on_buttonRemoveVstPathClicked();

    void on_buttonClearVstCache_clicked();

    void on_buttonScanVSTs_clicked();

    void on_browseRecPathButton_clicked();

    void on_recordingCheckBox_clicked();

    void on_pluginsScanFinished();

    void on_ButtonVst_Refresh_clicked();

    void on_ButtonVST_AddToBlackList_clicked();

signals:
    void ioPreferencesChanged(QList<bool> midiInputsStatus, int selectedAudioDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);

private:
    Ui::IODialog *ui;
    Controller::MainController* mainController;
    MainWindow* mainWindow;

    //AUDIO
    void populateAsioDriverCombo();
    void populateFirstInputCombo();
    void populateLastInputCombo();
    void populateFirstOutputCombo();
    void populateLastOutputCombo();
    void populateSampleRateCombo();
    void populateBufferSizeCombo();
    void populateAudioTab();


    //MIDI
    void populateMidiTab();

    //VST
    void populateVstTab();
    void addVstScanPath(QString path);
    void createWidgetsToNewScanFolder(QString path);
    void UpdateVstList(QString path);
    void UpdateBlackBox(QString path);
    void clearScanFolderWidgets();
    QList<QPushButton*> scanFoldersButtons;

    //recording
    void populateRecordingTab();
};


