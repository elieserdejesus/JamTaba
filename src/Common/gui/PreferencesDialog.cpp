#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include "persistence/Settings.h"
#include "MetronomeUtils.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);

    ui->comboLastOutput->setEnabled(false);
}

void PreferencesDialog::toggleBuiltInMetronomeSounds(bool usingBuiltInMetronome)
{
    ui->groupBoxCustomMetronome->setChecked(!usingBuiltInMetronome);
    refreshMetronomeControlsStyleSheet();
}

void PreferencesDialog::toggleCustomMetronomeSounds(bool usingCustomMetronome)
{
    ui->groupBoxBuiltInMetronomes->setChecked(!usingCustomMetronome);
    refreshMetronomeControlsStyleSheet();
}

void PreferencesDialog::refreshMetronomeControlsStyleSheet()
{
    style()->unpolish(ui->groupBoxCustomMetronome);
    style()->unpolish(ui->groupBoxBuiltInMetronomes);
    style()->unpolish(ui->textFieldPrimaryBeat);
    style()->unpolish(ui->textFieldSecondaryBeat);
    style()->unpolish(ui->browsePrimaryBeatButton);
    style()->unpolish(ui->browseSecondaryBeatButton);

    style()->polish(ui->groupBoxCustomMetronome);
    style()->polish(ui->groupBoxBuiltInMetronomes);
    style()->polish(ui->textFieldPrimaryBeat);
    style()->polish(ui->textFieldSecondaryBeat);
    style()->polish(ui->browsePrimaryBeatButton);
    style()->polish(ui->browseSecondaryBeatButton);
}

void PreferencesDialog::initialize(PreferencesTab initialTab, const Persistence::Settings *settings)
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
    connect(ui->browseRecPathButton, SIGNAL(clicked(bool)), this, SLOT(openRecordingPathBrowser()));

    connect(ui->groupBoxCustomMetronome, SIGNAL(toggled(bool)), this, SLOT(toggleCustomMetronomeSounds(bool)));
    connect(ui->groupBoxBuiltInMetronomes, SIGNAL(toggled(bool)), this, SLOT(toggleBuiltInMetronomeSounds(bool)));
    connect(ui->browsePrimaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(openPrimaryBeatAudioFileBrowser()));
    connect(ui->browseSecondaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(openSecondaryBeatAudioFileBrowser()));
}

void PreferencesDialog::accept()
{
    if (ui->groupBoxBuiltInMetronomes->isChecked()) {
        emit builtInMetronomeSelected(ui->comboBuiltInMetronomes->currentText());
    }
    else{
        emit customMetronomeSelected(ui->textFieldPrimaryBeat->text(), ui->textFieldSecondaryBeat->text());
    }
    QDialog::accept();
}

void PreferencesDialog::populateAllTabs()
{
    populateRecordingTab();
    populateMetronomeTab();
}

void PreferencesDialog::selectRecordingTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabRecording);
}

void PreferencesDialog::populateMetronomeTab()
{
    Q_ASSERT(settings);

    bool usingCustomMetronomeSounds = settings->isUsingCustomMetronomeSounds();
    ui->groupBoxCustomMetronome->setChecked(usingCustomMetronomeSounds);
    ui->groupBoxBuiltInMetronomes->setChecked(!usingCustomMetronomeSounds);
    ui->textFieldPrimaryBeat->setText(settings->getMetronomeFirstBeatFile());
    ui->textFieldSecondaryBeat->setText(settings->getMetronomeSecondaryBeatFile());

    //combo embedded metronome sounds
    QList<QString> metronomeAliases = Audio::MetronomeUtils::getBuiltInMetronomeAliases();
    ui->comboBuiltInMetronomes->clear();
    foreach (QString alias, metronomeAliases) {
        ui->comboBuiltInMetronomes->addItem(alias, alias);
    }

    //using built-in metronome?
    if (!settings->isUsingCustomMetronomeSounds()){
        ui->comboBuiltInMetronomes->setCurrentText(settings->getBuiltInMetronome());
    }
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

void PreferencesDialog::openPrimaryBeatAudioFileBrowser()
{
    QString caption = tr("Choosing Primary beat audio file...");
    QString filter = tr("Audio Files (*.wav, *.ogg)");
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldPrimaryBeat->setText(filePath);
    }
}

void PreferencesDialog::openSecondaryBeatAudioFileBrowser()
{
    QString caption = tr("Choosing Secondary beat audio file...");
    QString filter = tr("Audio Files (*.wav, *.ogg)");
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldSecondaryBeat->setText(filePath);
    }
}

QString PreferencesDialog::openAudioFileBrowser(const QString caption)
{
    QString filter = "Audio Files (*.wav, *.ogg)";
    QString dir = ".";
    return QFileDialog::getOpenFileName(this, caption, dir, filter);
}

void PreferencesDialog::openRecordingPathBrowser()
{
    QFileDialog fileDialog(this, tr("Choosing recording path ..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (fileDialog.exec()) {
        QDir dir = fileDialog.directory();
        QString newRecordingPath = dir.absolutePath();
        ui->recordPathLineEdit->setText(newRecordingPath);
        emit recordingPathSelected(newRecordingPath);
    }
}
