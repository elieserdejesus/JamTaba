#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QtWidgets>
#include <QDebug>
#include <QFileDialog>
#include "persistence/Settings.h"
#include "MetronomeUtils.h"
#include "audio/vorbis/VorbisEncoder.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);

    ui->comboLastOutput->setEnabled(false);

    connect(ui->recordPathLineEdit, &QLineEdit::textEdited, this, &PreferencesDialog::recordingPathSelected);

    // populate bit rate combo box
    quint8 bitRates[] = {16, 32};
    ui->comboBoxBitRate->clear();
    for (quint8 bitRate : bitRates) {
        ui->comboBoxBitRate->addItem(QString::number(bitRate) + " bits", QVariant::fromValue(bitRate));
    }
    connect(ui->comboBoxBitRate, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](){
        quint8 bitRate = ui->comboBoxBitRate->currentData().toInt();
        emit looperWaveFilesBitDepthChanged(bitRate);
    });

    connect(ui->radioButtonWaveFile, &QRadioButton::toggled, ui->comboBoxBitRate, &QComboBox::setEnabled);
    connect(ui->radioButtonWaveFile, &QRadioButton::toggled, ui->labelBitRate, &QComboBox::setEnabled);
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
    style()->unpolish(ui->textFieldOffBeat);
    style()->unpolish(ui->textFieldAccentBeat);
    style()->unpolish(ui->browsePrimaryBeatButton);
    style()->unpolish(ui->browseOffBeatButton);
    style()->unpolish(ui->browseAccentBeatButton);

    style()->polish(ui->groupBoxCustomMetronome);
    style()->polish(ui->groupBoxBuiltInMetronomes);
    style()->polish(ui->textFieldPrimaryBeat);
    style()->polish(ui->textFieldOffBeat);
    style()->polish(ui->textFieldAccentBeat);
    style()->polish(ui->browsePrimaryBeatButton);
    style()->polish(ui->browseOffBeatButton);
    style()->polish(ui->browseAccentBeatButton);
}

void PreferencesDialog::initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> &jamRecorders)
{
    Q_UNUSED(initialTab);
    this->settings = settings;
    this->jamRecorders = jamRecorders;
    this->jamRecorderCheckBoxes = QMap<QCheckBox *, QString>();

    for (const QString &jamRecorder : jamRecorders.keys()) {
        QCheckBox *myCheckBox = new QCheckBox(this);
        myCheckBox->setObjectName(jamRecorder);
        myCheckBox->setText(jamRecorders.value(jamRecorder));
        ui->layoutRecorders->addWidget(myCheckBox);
        jamRecorderCheckBoxes[myCheckBox] = jamRecorder;
    }

    setupSignals();

    populateAllTabs();
}

void PreferencesDialog::setupSignals()
{
    // the 'accept' slot is overrided in inherited classes (StandalonePreferencesDialog and VstPreferencesDialog)
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui->prefsTab, SIGNAL(currentChanged(int)), this, SLOT(selectTab(int)));

    connect(ui->recordingCheckBox, SIGNAL(clicked(bool)), this, SLOT(toggleRecording(bool)));
    for (QCheckBox *myCheckBox : jamRecorderCheckBoxes.keys()) {
        connect(myCheckBox, &QCheckBox::toggled, [=](bool newStatus) {
            emit jamRecorderStatusChanged(jamRecorderCheckBoxes[myCheckBox], newStatus);
        });
    }

    connect(ui->browseRecPathButton, SIGNAL(clicked(bool)), this, SLOT(openRecordingPathBrowser()));

    connect(ui->groupBoxCustomMetronome, SIGNAL(toggled(bool)), this, SLOT(toggleCustomMetronomeSounds(bool)));
    connect(ui->groupBoxBuiltInMetronomes, SIGNAL(toggled(bool)), this, SLOT(toggleBuiltInMetronomeSounds(bool)));
    connect(ui->browsePrimaryBeatButton, SIGNAL(clicked(bool)), this, SLOT(openPrimaryBeatAudioFileBrowser()));
    connect(ui->browseOffBeatButton, SIGNAL(clicked(bool)), this, SLOT(openOffBeatAudioFileBrowser()));
    connect(ui->browseAccentBeatButton, SIGNAL(clicked(bool)), this, SLOT(openAccentBeatAudioFileBrowser()));

    connect(ui->comboBoxEncoderQuality, SIGNAL(activated(int)), this, SLOT(emitEncodingQualityChanged()));

    connect(ui->radioButtonLooperOggEncoding, &QCheckBox::toggled, this, &PreferencesDialog::looperAudioEncodingFlagChanged);
    connect(ui->lineEditLoopsFolder, &QLineEdit::textChanged, this,  &PreferencesDialog::looperFolderChanged);
    connect(ui->loopsFolderBrowseButton, &QPushButton::clicked, [=]() {
        QString currentLoopsFolder = ui->lineEditLoopsFolder->text();
        QFileDialog folderDialog(this, tr("Choosing loops folder ..."), currentLoopsFolder);
        folderDialog.setAcceptMode(QFileDialog::AcceptOpen);
        folderDialog.setFileMode(QFileDialog::DirectoryOnly);
        if (folderDialog.exec()) {
            QDir dir = folderDialog.directory();
            QString newLoopsFolder = dir.absolutePath();
            ui->lineEditLoopsFolder->setText(newLoopsFolder);
        }
    });
}

void PreferencesDialog::toggleRecording(bool recording)
{
    for (QCheckBox *checkbox : jamRecorderCheckBoxes.keys())
        checkbox->setChecked(recording);

    emit multiTrackRecordingStatusChanged(recording);
}

void PreferencesDialog::emitEncodingQualityChanged()
{
    QVariant currentData = ui->comboBoxEncoderQuality->currentData();
    if (!currentData.isNull()) {
        float selectedQuality = currentData.toFloat();
        emit encodingQualityChanged(selectedQuality);
    }
}

void PreferencesDialog::accept()
{
    if (ui->groupBoxBuiltInMetronomes->isChecked()) {
        emit builtInMetronomeSelected(ui->comboBuiltInMetronomes->currentText());
    }
    else {
        emit customMetronomeSelected(ui->textFieldPrimaryBeat->text(), ui->textFieldOffBeat->text(), ui->textFieldAccentBeat->text());
    }
    QDialog::accept();
}

void PreferencesDialog::populateEncoderQualityComboBox()
{
    ui->comboBoxEncoderQuality->clear();
    ui->comboBoxEncoderQuality->addItem(tr("Low (good for slow internet connections)"), VorbisEncoder::QUALITY_LOW);
    ui->comboBoxEncoderQuality->addItem(tr("Normal (default)"), VorbisEncoder::QUALITY_NORMAL);
    ui->comboBoxEncoderQuality->addItem(tr("High (for good internet connections only)"), VorbisEncoder::QUALITY_HIGH);

    bool usingCustomQuality = usingCustomEncodingQuality();
    if (usingCustomQuality)
        ui->comboBoxEncoderQuality->addItem(tr("Custom quality"));

    //select the correct item in combobox
    if (!usingCustomQuality) {
        float quality = settings->getEncodingQuality();
        if (qFuzzyCompare(quality, VorbisEncoder::QUALITY_LOW))
            ui->comboBoxEncoderQuality->setCurrentIndex(0);
        else if (qFuzzyCompare(quality, VorbisEncoder::QUALITY_NORMAL))
            ui->comboBoxEncoderQuality->setCurrentIndex(1);
        else if (qFuzzyCompare(quality, VorbisEncoder::QUALITY_HIGH))
                    ui->comboBoxEncoderQuality->setCurrentIndex(2);
    }
    else {
        ui->comboBoxEncoderQuality->setCurrentIndex(ui->comboBoxEncoderQuality->count() - 1);
    }
}

bool PreferencesDialog::usingCustomEncodingQuality()
{
    float currentQuality = settings->getEncodingQuality();

    if (qFuzzyCompare(currentQuality, VorbisEncoder::QUALITY_LOW))
        return false;

    if (qFuzzyCompare(currentQuality, VorbisEncoder::QUALITY_NORMAL))
        return false;

    if (qFuzzyCompare(currentQuality, VorbisEncoder::QUALITY_HIGH))
        return false;

    return true;
}

void PreferencesDialog::populateAllTabs()
{
    populateEncoderQualityComboBox();
    populateMultiTrackRecordingTab();
    populateMetronomeTab();
    populateLooperTab();
}

void PreferencesDialog::populateLooperTab()
{
    QSignalBlocker lineEditSignalBlocker(ui->lineEditLoopsFolder);
    QSignalBlocker radioButtonSignalBlocker(ui->radioButtonLooperOggEncoding);
    QSignalBlocker bitDepthCheckBoxBlocker(ui->comboBoxBitRate);

    ui->lineEditLoopsFolder->setText(settings->getLooperFolder());
    ui->radioButtonLooperOggEncoding->setChecked(settings->getLooperAudioEncodingFlag());
    ui->radioButtonWaveFile->setChecked(!ui->radioButtonLooperOggEncoding->isChecked());

    ui->comboBoxBitRate->setEnabled(!settings->getLooperAudioEncodingFlag());
    ui->labelBitRate->setEnabled(ui->comboBoxBitRate->isEnabled());

    quint8 bitDepth = settings->getLooperBitDepth();
    quint8 comboBoxIndex = 0; // 16 bits
    if (bitDepth == 32)
        comboBoxIndex = 1;

    ui->comboBoxBitRate->setCurrentIndex(comboBoxIndex);
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
    ui->textFieldOffBeat->setText(settings->getMetronomeOffBeatFile());

    // combo embedded metronome sounds
    QList<QString> metronomeAliases = Audio::MetronomeUtils::getBuiltInMetronomeAliases();
    ui->comboBuiltInMetronomes->clear();
    foreach (QString alias, metronomeAliases) {
        ui->comboBuiltInMetronomes->addItem(alias, alias);
    }

    // using built-in metronome?
    if (!settings->isUsingCustomMetronomeSounds()){
        ui->comboBuiltInMetronomes->setCurrentText(settings->getBuiltInMetronome());
    }
}

void PreferencesDialog::populateMultiTrackRecordingTab()
{
    Q_ASSERT(settings);
    Persistence::MultiTrackRecordingSettings recordingSettings = settings->getMultiTrackRecordingSettings();
    ui->recordingCheckBox->setChecked(recordingSettings.saveMultiTracksActivated);

    for (QCheckBox *myCheckBox : jamRecorderCheckBoxes.keys()) {
        myCheckBox->setChecked(recordingSettings.isJamRecorderActivated(jamRecorderCheckBoxes[myCheckBox]));
    }

    QDir recordDir(recordingSettings.recordingPath);
    ui->recordPathLineEdit->setText(recordDir.absolutePath());
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

QString PreferencesDialog::getAudioFilesFilter()
{
    return tr("Audio Files") + " (*.wav *.ogg)";
}

void PreferencesDialog::openPrimaryBeatAudioFileBrowser()
{
    QString caption = tr("Choosing Primary beat audio file...");
    QString filter = getAudioFilesFilter();
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldPrimaryBeat->setText(filePath);
    }
}

void PreferencesDialog::openOffBeatAudioFileBrowser()
{
    QString caption = tr("Choosing Off beat audio file...");
    QString filter = getAudioFilesFilter();
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldOffBeat->setText(filePath);
    }
}

void PreferencesDialog::openAccentBeatAudioFileBrowser()
{
    QString caption = tr("Choosing Accent beat audio file...");
    QString filter = getAudioFilesFilter();
    QString dir = ".";
    QString filePath = QFileDialog::getOpenFileName(this, caption, dir, filter);
    if (!filePath.isNull()) {
        ui->textFieldAccentBeat->setText(filePath);
    }
}

QString PreferencesDialog::openAudioFileBrowser(const QString caption)
{
    QString filter = tr("Audio Files") + " (*.wav, *.ogg)";
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
