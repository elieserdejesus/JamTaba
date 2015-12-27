#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>

using namespace Audio;
using namespace Controller;

PreferencesDialog::PreferencesDialog(Controller::MainController *mainController,
                                     MainWindow *mainWindow) :
    QDialog(mainWindow),
    ui(new Ui::IODialog),
    mainController(mainController),
    mainWindow(mainWindow)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->comboLastOutput->setEnabled(false);

    setupSignals();
    populateAllTabs();
}

void PreferencesDialog::setupSignals()
{
    // the 'accept' slot is overrided in inherited classes (StandalonePreferencesDialog and VstPreferencesDialog)
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui->prefsTab, SIGNAL(currentChanged(int)), this, SLOT(selectPreferencesTab(int)));
    connect(ui->recordingCheckBox, SIGNAL(clicked(bool)), this,
            SLOT(setMultiTrackRecordingStatus(bool)));
    connect(ui->browseRecPathButton, SIGNAL(clicked(bool)), this, SLOT(selectRecordingPath()));
}

void PreferencesDialog::populateAllTabs(){
    populateRecordingTab();
}

void PreferencesDialog::selectRecordingTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabRecording);
}

void PreferencesDialog::populateRecordingTab()
{
    QString recordingPath = mainController->getSettings().getRecordingPath();
    QDir recordDir(recordingPath);
    bool isSaveMultiTrackActivated = mainController->getSettings().isSaveMultiTrackActivated();
    ui->recordingCheckBox->setChecked(isSaveMultiTrackActivated);
    ui->recordPathLineEdit->setText(recordDir.absolutePath());
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

// Recording TAB controls --------------------
void PreferencesDialog::selectRecordingPath()
{
    QFileDialog fileDialog(this, "Choosing recording path ...");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (fileDialog.exec()) {
        QDir dir = fileDialog.directory();
        mainController->storeRecordingPath(dir.absolutePath());
        ui->recordPathLineEdit->setText(dir.absolutePath());
    }
}

void PreferencesDialog::setMultiTrackRecordingStatus(bool recordingActivated)
{
    mainController->storeRecordingMultiTracksStatus(recordingActivated);
}
