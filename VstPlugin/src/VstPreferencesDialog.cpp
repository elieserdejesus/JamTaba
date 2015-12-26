#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

void PreferencesDialog::runPostInitialization()
{
    // in Vst plugin some preferences are not available

    // remove the first 3 tabs (audio, midi and VSTs)
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
    ui->prefsTab->removeTab(0);
}

void PreferencesDialog::on_okButton_released()
{
    this->accept();// just accept and close the Preferences dialog
}

void PreferencesDialog::on_prefsTab_currentChanged(int index)
{
    Q_UNUSED(index)
    populateRecordingTab();
}

// a lot of empty implementations because these features are not available in Vst Plugin
void PreferencesDialog::selectAudioTab()
{
}

void PreferencesDialog::selectMidiTab()
{
}

void PreferencesDialog::populateVstTab()
{
}

void PreferencesDialog::selectVstPluginsTab()
{
}

void PreferencesDialog::populateMidiTab()
{
}

void PreferencesDialog::populateAudioTab()
{
}

void PreferencesDialog::populateAsioDriverCombo()
{
}

void PreferencesDialog::populateFirstInputCombo()
{
}

void PreferencesDialog::populateLastInputCombo()
{
}

void PreferencesDialog::populateFirstOutputCombo()
{
}

void PreferencesDialog::populateLastOutputCombo()
{
}

void PreferencesDialog::populateSampleRateCombo()
{
}

void PreferencesDialog::populateBufferSizeCombo()
{
}

// ++++++++++++
void PreferencesDialog::on_comboAudioDevice_activated(int)
{
}

void PreferencesDialog::on_comboFirstInput_currentIndexChanged(int)
{
}

void PreferencesDialog::on_comboFirstOutput_currentIndexChanged(int)
{
}
