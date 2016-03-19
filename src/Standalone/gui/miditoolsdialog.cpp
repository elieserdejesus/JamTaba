#include "MidiToolsDialog.h"
#include "ui_miditoolsdialog.h"

#include "LocalTrackViewStandalone.h"

MidiToolsDialog::MidiToolsDialog(LocalTrackViewStandalone *trackView) :
    QDialog(nullptr),
    ui(new Ui::MidiToolsDialog),
    trackView(trackView)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);
    setModal(true);
    setWindowTitle("Midi Tools");
}

MidiToolsDialog::~MidiToolsDialog()
{
    delete ui;
}

void MidiToolsDialog::closeEvent(QCloseEvent *event)
{
    emit dialogClosed();
    event->accept();
}
