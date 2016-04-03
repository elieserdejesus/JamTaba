#include "MidiToolsDialog.h"
#include "ui_miditoolsdialog.h"
#include <QRegularExpressionValidator>

#include "LocalTrackViewStandalone.h"

MidiToolsDialog::MidiToolsDialog(const QString &lowerNote, const QString &higherNote, qint8 transpose) :
    QDialog(nullptr),
    ui(new Ui::MidiToolsDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WA_DeleteOnClose);
    setModal(true);
    setWindowTitle(tr("Midi Tools"));

    QRegularExpression regex("^[A-G][#b]?[0-8]");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex);

    ui->textFieldHigherNote->setValidator(validator);
    ui->textFieldLowerNote->setValidator(validator);
    ui->spinBoxTranspose->setValue(transpose);

    connect(ui->textFieldHigherNote, SIGNAL(editingFinished()), this, SLOT(higherNoteEditionFinished()));
    connect(ui->textFieldLowerNote, SIGNAL(editingFinished()), this, SLOT(lowerNoteEditionFinished()));
    connect(ui->buttonLearnLowerNote, SIGNAL(toggled(bool)), this, SLOT(learnLowerMidiNoteToggled(bool)));
    connect(ui->buttonLearnHigherNote, SIGNAL(toggled(bool)), this, SLOT(learnHigherMidiNoteToggled(bool)));

    ui->textFieldLowerNote->setText(lowerNote);
    ui->textFieldHigherNote->setText(higherNote);

    ui->spinBoxTranspose->setMaximum(24);
    ui->spinBoxTranspose->setMinimum(-24);
    connect(ui->spinBoxTranspose, SIGNAL(valueChanged(int)), SLOT(transposeValueChanged(int)));
}

MidiToolsDialog::~MidiToolsDialog()
{
    delete ui;
}

void MidiToolsDialog::learnLowerMidiNoteToggled(bool buttonChecked)
{
    if (buttonChecked){
        ui->buttonLearnHigherNote->setChecked(false);
    }
    emit learnMidiNoteClicked(buttonChecked);
}

void MidiToolsDialog::learnHigherMidiNoteToggled(bool buttonChecked)
{
    if (buttonChecked){
        ui->buttonLearnLowerNote->setChecked(false);
    }
    emit learnMidiNoteClicked(buttonChecked);
}

void MidiToolsDialog::setLearnedMidiNote(const QString &learnedNote)
{
    if (ui->buttonLearnHigherNote->isChecked()) {
        ui->textFieldHigherNote->setText(learnedNote);
        emit higherNoteChanged(learnedNote);
    }
    else{
        ui->textFieldLowerNote->setText(learnedNote);
        emit lowerNoteChanged(learnedNote);
    }
}

void MidiToolsDialog::transposeValueChanged(int transposeValue)
{
    emit transposeChanged((qint8)transposeValue);
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
