#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include "persistence/Settings.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IODialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);

    ui->comboLastOutput->setEnabled(false);

    connect(ui->groupBoxCustomSounds, SIGNAL(toggled(bool)), this, SLOT(refreshCustomMetronomeControlsStyleSheet()));

    ui->groupBoxCustomSounds->setChecked(false);
}

void PreferencesDialog::refreshCustomMetronomeControlsStyleSheet()
{
    style()->unpolish(ui->groupBoxCustomSounds);
    style()->unpolish(ui->textFieldPrimaryBeat);
    style()->unpolish(ui->textFieldSecondaryBeat);
    style()->unpolish(ui->browsePrimaryBeatButton);
    style()->unpolish(ui->browseSecondaryBeatButton);

    style()->polish(ui->groupBoxCustomSounds);
    style()->polish(ui->textFieldPrimaryBeat);
    style()->polish(ui->textFieldSecondaryBeat);
    style()->polish(ui->browsePrimaryBeatButton);
    style()->polish(ui->browseSecondaryBeatButton);
}

void PreferencesDialog::initialize(int initialTab, const Persistence::Settings *settings)
{
    Q_UNUSED(initialTab);
    this->settings = settings;
    setupSignals();
    populateAllTabs();
}

void PreferencesDialog::setupSignals()
{
    // the 'accept' slot is overrided in inherited classes (StandalonePreferencesDialog and VstPreferencesDialog)
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui->prefsTab, SIGNAL(currentChanged(int)), this, SLOT(selectTab(int)));
    connect(ui->recordingCheckBox, SIGNAL(clicked(bool)), this,
            SIGNAL(multiTrackRecordingStatusChanged(bool)));
    connect(ui->browseRecPathButton, SIGNAL(clicked(bool)), this, SLOT(selectRecordingPath()));

    connect(ui->browsePrimaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(selectPrimaryBeatAudioFile()));
    connect(ui->browseSecondaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(selectSecondaryBeatAudioFile()));
}

void PreferencesDialog::populateAllTabs()
{
    populateRecordingTab();
}

void PreferencesDialog::selectRecordingTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabRecording);
}

void PreferencesDialog::populateRecordingTab()
{
    Q_ASSERT(settings);
    Persistence::RecordingSettings recordingSettings = settings->getRecordingSettings();
    QDir recordDir(recordingSettings.recordingPath);
    ui->recordingCheckBox->setChecked(recordingSettings.saveMultiTracksActivated);
    ui->recordPathLineEdit->setText(recordDir.absolutePath());
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::selectPrimaryBeatAudioFile()
{
    QString caption = "Choosing Primary beat audio file...";
    QString filter = "Audio Files (*.wav)";
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldPrimaryBeat->setText(filePath);
        emit metronomePrimaryBeatAudioFileSelected(filePath);
    }
}

void PreferencesDialog::selectSecondaryBeatAudioFile()
{
    QString caption = "Choosing Secondary beat audio file...";
    QString filter = "Audio Files (*.wav)";
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldSecondaryBeat->setText(filePath);
        emit metronomeSecondaryBeatAudioFileSelected(filePath);
    }
}

void PreferencesDialog::selectRecordingPath()
{
    QFileDialog fileDialog(this, "Choosing recording path ...");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (fileDialog.exec()) {
        QDir dir = fileDialog.directory();
        QString newRecordingPath = dir.absolutePath();
        ui->recordPathLineEdit->setText(newRecordingPath);
        emit recordingPathSelected(newRecordingPath);
    }
}
