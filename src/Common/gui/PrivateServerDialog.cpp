#include "PrivateServerDialog.h"
#include "ui_PrivateServerDialog.h"
#include "MainController.h"
#include "persistence/Settings.h"
#include <QDebug>

using namespace persistence;

PrivateServerDialog::PrivateServerDialog(QWidget *parent, controller::MainController *mainController) :
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

    // setting initial values
    const auto &settings = mainController->getSettings();
    ui->textFieldPassword->setText(settings.getLastPrivateServerPassword());
    ui->textFieldPort->setText(QString::number(settings.getLastPrivateServerPort()));
    ui->textFieldUserName->setText(mainController->getUserName());

    ui->textFieldUserName->forceCenterAlignment(false);
    ui->textFieldUserName->setAlignment(Qt::AlignLeft);

    ui->textFieldPassword->setEchoMode(QLineEdit::Password);

    ui->textFieldPassword->setAttribute(Qt::WA_MacShowFocusRect, 0); // remove focus border in Mac
    ui->textFieldPort->setAttribute(Qt::WA_MacShowFocusRect, 0); // remove focus border in Mac
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
    QString userName = ui->textFieldUserName->text();

    emit connectionAccepted(serverName, serverPort, userName, serverPassword);
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
