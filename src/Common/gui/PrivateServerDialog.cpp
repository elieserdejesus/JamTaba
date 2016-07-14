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

    ui->okButton->setAutoDefault(true);
    connect(ui->okButton, &QPushButton::clicked, this, &PrivateServerDialog::accept);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    buildComboBoxItems();

    //setting initial values
    const Persistence::Settings &settings = mainController->getSettings();
    ui->textFieldPassword->setText(settings.getLastPrivateServerPassword());
    ui->textFieldPort->setText(QString::number(settings.getLastPrivateServerPort()));
}

void PrivateServerDialog::buildComboBoxItems()
{
    ui->comboBoxServers->clear();
    QList<QString> servers = mainController->getSettings().getLastPrivateServers();
    for (const QString &server : servers) {
        ui->comboBoxServers->addItem(server, server);
    }
}

void PrivateServerDialog::accept()
{
    QDialog::accept();
    QString serverName = ui->comboBoxServers->currentText();
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
    return ui->comboBoxServers->currentText();
}

PrivateServerDialog::~PrivateServerDialog()
{
    delete ui;
}
