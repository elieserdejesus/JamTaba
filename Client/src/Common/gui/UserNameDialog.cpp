#include "UserNameDialog.h"
#include "ui_UserNameDialog.h"
#include <QPushButton>

UserNameDialog::UserNameDialog(QWidget *parent, QString lastUserName) :
    QDialog(parent),
    ui(new Ui::UserNameDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setText(lastUserName);
    QObject::connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateOkButtonAvailability()));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void UserNameDialog::updateOkButtonAvailability()
{
    QPushButton *button = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (button)
        button->setEnabled(!ui->lineEdit->text().isEmpty());
}

UserNameDialog::~UserNameDialog()
{
    delete ui;
}

QString UserNameDialog::getUserName() const
{
    return ui->lineEdit->text();
}
