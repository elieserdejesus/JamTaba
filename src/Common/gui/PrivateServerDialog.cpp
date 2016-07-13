#include "PrivateServerDialog.h"
#include "ui_PrivateServerDialog.h"
#include "MainController.h"
#include "persistence/Settings.h"
#include <QDebug>

using namespace Persistence;

PrivateServerDialog::PrivateServerDialog(QWidget *parent, Controller::MainController *mainController) :
    QDialog(parent),
    ui(new Ui::PrivateServerDialog),
    mainController(mainController)
{
    ui->setupUi(this);

    QList<PrivateServer> servers = mainController->getSettings().getPrivateServers();
    if (!servers.isEmpty()) {
        PrivateServer server = servers.first();
        ui->serverTextField->setText(server.getName());
        ui->passwordTextField->setText(server.getPassword());
        ui->serverPortTextField->setText(QString::number(server.getPort()));
    }

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QObject::connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(on_okButtonTriggered()));
}

void PrivateServerDialog::buildComboBoxItems()
{

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
