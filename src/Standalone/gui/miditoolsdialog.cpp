#include "MidiToolsDialog.h"
#include "ui_miditoolsdialog.h"
#include <QRegularExpressionValidator>

#include "LocalTrackViewStandalone.h"

MidiToolsDialog::MidiToolsDialog(const QString &lowerNote, const QString &higherNote) :
    QDialog(nullptr),
    ui(new Ui::MidiToolsDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);
    setModal(true);
    setWindowTitle("Midi Tools");

    QRegularExpression regex("^[A-G][#b]?[0-8]");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->textFieldHigherNote->setValidator(validator);
    ui->textFieldLowerNote->setValidator(validator);

    connect(ui->textFieldHigherNote, SIGNAL(editingFinished()), this, SLOT(higherNoteEditionFinished()));
    connect(ui->textFieldLowerNote, SIGNAL(editingFinished()), this, SLOT(lowerNoteEditionFinished()));

    ui->textFieldHigherNote->setText(higherNote);
    ui->textFieldLowerNote->setText(lowerNote);
}

MidiToolsDialog::~MidiToolsDialog()
{
    delete ui;
}

void MidiToolsDialog::lowerNoteEditionFinished()
{
    emit lowerNoteChanged(ui->textFieldLowerNote->text());
}

void MidiToolsDialog::higherNoteEditionFinished()
{
    emit higherNoteChanged(ui->textFieldHigherNote->text());
}

void MidiToolsDialog::closeEvent(QCloseEvent *event)
{
    emit dialogClosed();
    event->accept();
}
