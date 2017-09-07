#include "PreferencesDialogPlugin.h"

PreferencesDialogPlugin::PreferencesDialogPlugin(QWidget *parent) :
    PreferencesDialog(parent)
{
    // in Vst plugin some preferences are not available
    // remove the first 3 tabs (audio, midi and VSTs)
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
}

void PreferencesDialogPlugin::initialize(PreferencesTab initialTab, const Persistence::Settings *settings, const QMap<QString, QString> &jamRecorders)
{
    PreferencesDialog::initialize(initialTab, settings, jamRecorders);
    int tabIndex = static_cast<int>(initialTab) - 3; // the 3 first tabs are removed in plugins
    ui->prefsTab->setCurrentIndex(tabIndex);
}

void PreferencesDialogPlugin::selectTab(int index)
{
    switch (static_cast<PreferencesTab>(index)) {
    case PreferencesTab::TabMultiTrackRecording:
        populateMultiTrackRecordingTab();
        break;
    case PreferencesTab::TabLooper:
        populateLooperTab();
        break;
    case PreferencesTab::TabMetronome:
        populateMetronomeTab();
        break;
    }
}

void PreferencesDialogPlugin::populateAllTabs()
{
    // populating only the available tabs
    populateMultiTrackRecordingTab();
    populateMetronomeTab();
    populateLooperTab();
}
