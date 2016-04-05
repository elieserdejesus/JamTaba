#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include "persistence/Settings.h"
#include "MetronomeUtils.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IODialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);

    ui->comboLastOutput->setEnabled(false);

    ui->groupBoxCustomSounds->setChecked(false);
}

void PreferencesDialog::toggleMetronomeCustomSounds(bool status)
{
    refreshMetronomeControlsStyleSheet();
    emit usingMetronomeCustomSoundsStatusChanged(status);
}

void PreferencesDialog::refreshMetronomeControlsStyleSheet()
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
    connect(ui->browseRecPathButton, SIGNAL(clicked(bool)), this, SLOT(openRecordingPathBrowser()));

    connect(ui->groupBoxCustomSounds, SIGNAL(toggled(bool)), this, SLOT(toggleMetronomeCustomSounds(bool)));
    connect(ui->browsePrimaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(openPrimaryBeatAudioFileBrowser()));
    connect(ui->browseSecondaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(openSecondaryBeatAudioFileBrowser()));

    connect(ui->textFieldPrimaryBeat, SIGNAL(editingFinished()), this, SLOT(emitFirstBeatAudioFileChanged()));
    connect(ui->textFieldSecondaryBeat, SIGNAL(editingFinished()), this, SLOT(emitSecondaryBeatAudioFileChanged()));
    connect(ui->comboBuiltInMetronomes, SIGNAL(activated(QString)), this, SIGNAL(builtInMetronomeSelected(QString)));
}

void PreferencesDialog::emitFirstBeatAudioFileChanged()
{
    emit metronomePrimaryBeatAudioFileSelected(ui->textFieldPrimaryBeat->text());
}

void PreferencesDialog::emitSecondaryBeatAudioFileChanged()
{
    emit metronomeSecondaryBeatAudioFileSelected(ui->textFieldSecondaryBeat->text());
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

    ui->groupBoxCustomSounds->setChecked(settings->isUsingCustomMetronomeSounds());
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
    QString filter = tr("Audio Files (*.wav)");
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldPrimaryBeat->setText(filePath);
        emit metronomePrimaryBeatAudioFileSelected(filePath);
    }
}

void PreferencesDialog::openSecondaryBeatAudioFileBrowser()
{
    QString caption = tr("Choosing Secondary beat audio file...");
    QString filter = tr("Audio Files (*.wav)");
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldSecondaryBeat->setText(filePath);
        emit metronomeSecondaryBeatAudioFileSelected(filePath);
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
