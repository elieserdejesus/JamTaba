#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QCheckBox>
#include "audio/core/AudioDriver.h"
#include "midi/MidiDriver.h"
#include "persistence/Settings.h"
#include "MainController.h"
#include "MainWindow.h"

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

    QObject::connect(mainController->getPluginFinder(),
                     SIGNAL(scanFinished(bool)),
                     this,
                     SLOT(on_pluginsScanFinished(bool)));
    populateAudioTab();
    populateMidiTab();
    populateVstTab();
    populateRecordingTab();

    runPostInitialization(); // see the comments below...
/**
    The member function 'runPostInitialization' is implemented in different files. When we are building the Jamtaba Standalone in Windows the file Standalone/WindowsPreferencesDialog.cpp is used. In MacOSX the file Standalone/MacPreferencesDialog.cpp is used.
    When we are building the Jamtaba VstPlugin the file VstPlugin/VstPreferencesDialog.cpp is used.
*/
}

void PreferencesDialog::selectRecordingTab()
{
    ui->prefsTab->setCurrentWidget(ui->tabRecording);
}

void PreferencesDialog::clearScanFolderWidgets()
{
    foreach (QWidget *removeButton, scanFoldersButtons) {
        ui->panelScanFolders->layout()->removeWidget(removeButton->parentWidget());
        delete removeButton->parentWidget();
    }
    scanFoldersButtons.clear();
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

// VST
void PreferencesDialog::on_pluginsScanFinished(bool finishedWithoutError)
{
    Q_UNUSED(finishedWithoutError);
    populateVstTab();
}

void PreferencesDialog::on_buttonAddVstScanFolder_clicked()
{
    QFileDialog fileDialog(this, "Adding VST path ...");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    if (fileDialog.exec()) {
        QDir dir = fileDialog.directory();
        addVstScanPath(dir.absolutePath());
    }
}

void PreferencesDialog::on_buttonRemoveVstPathClicked()
{
    QPushButton *buttonClicked = (QPushButton *)sender();
    // find the index of button clicked
    int buttonIndex = 0;
    while (scanFoldersButtons.at(buttonIndex) != buttonClicked)
        buttonIndex++;
    if (buttonIndex < scanFoldersButtons.size()) {
        ui->panelScanFolders->layout()->removeWidget(buttonClicked->parentWidget());
        scanFoldersButtons.removeOne(buttonClicked);
        delete buttonClicked->parentWidget();
        mainController->removePluginsScanPath(buttonIndex);
    }
}

void PreferencesDialog::UpdateVstList(QString path)
{
    ui->plainTextEdit->appendPlainText(path);
}

void PreferencesDialog::UpdateBlackBox(QString path, bool add)
{
    if (add) {
        ui->BlackBoxText->appendPlainText(path);
    } else {
        QString str = ui->BlackBoxText->toPlainText();
        if (str.contains(path)) {
            ui->BlackBoxText->clear();
            mainController->removeBlackVst(str.indexOf(path));
            QStringList list = mainController->getSettings().getBlackListedPlugins();
            foreach (QString s, list)
                ui->BlackBoxText->appendPlainText(s);
        }
    }
}

void PreferencesDialog::createWidgetsToNewScanFolder(QString path)
{
    QVBoxLayout *panelLayout = (QVBoxLayout *)ui->panelScanFolders->layout();
    QWidget *parent = new QWidget(this);
    QHBoxLayout *lineLayout = new QHBoxLayout(parent);
    lineLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton *removeButton = new QPushButton(QIcon(":/images/less.png"), "", parent);
    removeButton->setToolTip("Remove this folder from scanning");
    QObject::connect(removeButton, SIGNAL(clicked()), this, SLOT(on_buttonRemoveVstPathClicked()));
    lineLayout->addWidget(removeButton);
    lineLayout->addWidget(new QLabel(path, this), 1.0);
    panelLayout->addWidget(parent);
    scanFoldersButtons.push_back(removeButton);/** save the widget to easy remove all widgets when the VSt Scan path tab is populated */
}

void PreferencesDialog::addVstScanPath(QString path)
{
    createWidgetsToNewScanFolder(path);
    mainController->addPluginsScanPath(path);
}

// clear the vst cache and run a complete scan
void PreferencesDialog::on_buttonClearVstAndScan_clicked()
{
    if (mainController && mainWindow) {
        // save the config file before start scanning
        mainController->saveLastUserSettings(mainWindow->getInputsSettings());

        // clear
        mainController->clearPluginsCache();
        ui->plainTextEdit->clear();

        // scan
        mainController->scanPlugins();
    }
}

// REFRESH VST LIST
void PreferencesDialog::on_ButtonVst_Refresh_clicked()
{
    if (mainController && mainWindow) {
        // save the config file before start scanning
        mainController->saveLastUserSettings(mainWindow->getInputsSettings());

        // scan only new plugins
        mainController->scanPlugins(true);
    }

    // populateVstTab();
}

// ADD A VST IN BLACKLIST
void PreferencesDialog::on_ButtonVST_AddToBlackList_clicked()
{
    QFileDialog VstDialog(this, "Add Vst(s) to BlackBox ...");
    VstDialog.setNameFilter(tr("Dll(*.dll)"));
    QStringList foldersToScan = mainController->getSettings().getVstScanFolders();
    if (!foldersToScan.isEmpty())
        VstDialog.setDirectory(foldersToScan.first());
    VstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    VstDialog.setFileMode(QFileDialog::ExistingFiles);
    if (VstDialog.exec()) {
        QStringList VstNames = VstDialog.selectedFiles();
        foreach (QString string, VstNames) {
            UpdateBlackBox(string, true);// add to
            mainController->addBlackVstToSettings(string);
        }
    }
}

void PreferencesDialog::on_ButtonVST_RemFromBlkList_clicked()
{
    QFileDialog VstDialog(this, "Remove Vst(s) to BlackBox ...");
    VstDialog.setNameFilter(tr("Dll(*.dll)"));
    QStringList foldersToScan = mainController->getSettings().getVstScanFolders();
    if (!foldersToScan.isEmpty())
        VstDialog.setDirectory(foldersToScan.first());
    VstDialog.setAcceptMode(QFileDialog::AcceptOpen);
    VstDialog.setFileMode(QFileDialog::ExistingFiles);
    if (VstDialog.exec()) {
        QStringList VstNames = VstDialog.selectedFiles();
        foreach (QString string, VstNames) {
            UpdateBlackBox(string, false);// Remove from
            mainController->removeBlackVst(0);// index
        }
    }
}

// Recording TAB controls --------------------
void PreferencesDialog::on_browseRecPathButton_clicked()
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

void PreferencesDialog::on_recordingCheckBox_clicked()
{
    mainController->storeRecordingMultiTracksStatus(ui->recordingCheckBox->isChecked());
}

// +++++++++++++++++++++++++++

void PreferencesDialog::on_buttonControlPanel_clicked()
{
    AudioDriver *audioDriver = mainController->getAudioDriver();
    if (audioDriver->hasControlPanel())// just in case
        audioDriver->openControlPanel((void *)mainWindow->winId());
}
