#include "vstpathsdialog.h"
#include "ui_vstpathsdialog.h"

VSTPathsDialog::VSTPathsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VSTPathsDialog)
{
    ui->setupUi(this);
}

VSTPathsDialog::~VSTPathsDialog()
{
    delete ui;
}
