#include "ChordProgressionCreationDialog.h"
#include "ui_ChordProgressionCreationDialog.h"

ChordProgressionCreationDialog::ChordProgressionCreationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChordProgressionCreationDialog)
{
    ui->setupUi(this);
}

ChordProgressionCreationDialog::~ChordProgressionCreationDialog()
{
    delete ui;
}
