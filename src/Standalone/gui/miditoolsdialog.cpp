#include "MidiToolsDialog.h"
#include "ui_miditoolsdialog.h"
#include <QRegularExpressionValidator>

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

    QRegularExpression regex("^[A-G][#b]?[0-8]");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->textFieldHigherNote->setValidator(validator);
    ui->textFieldLowerNote->setValidator(validator);
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
