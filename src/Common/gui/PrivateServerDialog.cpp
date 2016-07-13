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
        ui->textFieldName->setText(server.getName());
        ui->textFieldPassword->setText(server.getPassword());
        ui->textFieldPort->setText(QString::number(server.getPort()));
    }

    ui->okButton->setAutoDefault(true);
    connect(ui->okButton, &QPushButton::clicked, this, &PrivateServerDialog::accept);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    buildComboBoxItems();
}

void PrivateServerDialog::buildComboBoxItems()
{
    ui->comboBoxServers->clear();
    QList<PrivateServer> servers = mainController->getSettings().getPrivateServers();
    for (const PrivateServer &server : servers) {
        QString serverString = server.getName() + ":" + QString::number(server.getPort());
        ui->comboBoxServers->addItem(serverString, QVariant(serverString));
    }
}

void PrivateServerDialog::accept()
{
    QDialog::accept();
    QString serverName = ui->textFieldName->text();
    int serverPort = ui->textFieldPort->text().toInt();
    QString serverPassword = ui->textFieldPassword->text();

    emit connectionAccepted(serverName, serverPort, serverPassword);
}

QString PrivateServerDialog::getPassword() const
{
    return ui->textFieldPassword->text();
}

QString PrivateServerDialog::getServer() const
{
    return ui->textFieldName->text();
}

PrivateServerDialog::~PrivateServerDialog()
{
    delete ui;
}
