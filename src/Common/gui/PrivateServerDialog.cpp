#include "PrivateServerDialog.h"
#include "ui_PrivateServerDialog.h"
#include <QDebug>

PrivateServerDialog::PrivateServerDialog(QWidget *parent, QString lastServer, int lastServerPort,
                                         QString lastPassword) :
    QDialog(parent),
    ui(new Ui::PrivateServerDialog)
{
    ui->setupUi(this);
    ui->serverTextField->setText(lastServer);
    ui->passwordTextField->setText(lastPassword);
    ui->serverPortTextField->setText(QString::number(lastServerPort));

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QObject::connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(on_okButtonTriggered()));
}

void PrivateServerDialog::on_okButtonTriggered()
{
    accept();
    emit connectionAccepted(ui->serverTextField->text(),
                            ui->serverPortTextField->text().toInt(), ui->passwordTextField->text());
}

QString PrivateServerDialog::getPassword() const
{
    return ui->passwordTextField->text();
}

QString PrivateServerDialog::getServer() const
{
    return ui->serverTextField->text();
}

PrivateServerDialog::~PrivateServerDialog()
{
    delete ui;
}
