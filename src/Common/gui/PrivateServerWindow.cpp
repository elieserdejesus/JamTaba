#include "PrivateServerWindow.h"
#include "ui_PrivateServerWindow.h"
#include "ninjam/server/Server.h"

#include <QDateTime>

PrivateServerWindow::PrivateServerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrivateServerWindow)
{
    ui->setupUi(this);

    connect(ui->pushButtonStart, &QPushButton::clicked, this, &PrivateServerWindow::startServer);
    connect(ui->pushButtonStop, &QPushButton::clicked, this, &PrivateServerWindow::stopServer);

    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

    setServerDetailsVisibility(false);
}

PrivateServerWindow::~PrivateServerWindow()
{

    server.reset(nullptr);

    delete ui;
}

void PrivateServerWindow::setupServer()
{
    if (!server)
        return;

    connect(server.data(), &Server::serverStarted, this, &PrivateServerWindow::serverStarted);
    connect(server.data(), &Server::serverStopped, this, &PrivateServerWindow::serverStopped);

    connect(server.data(), &Server::errorStartingServer, [=](const QString &errorMessage){
        appendTextInLog(errorMessage);
    });

    connect(server.data(), &Server::incommingConnection, [=](const QString &ip){
        appendTextInLog(QString("Incomming connection from %1").arg(ip));
    });

    connect(server.data(), &Server::userEntered, [=](const QString &userName){
        appendTextInLog(QString("%1 entered in the server").arg(userName));

        updateUserList();
    });

    connect(server.data(), &Server::userLeave, [=](const QString &userName){
        appendTextInLog(QString("%1 left the server").arg(userName));

        updateUserList();
    });


}

void PrivateServerWindow::appendTextInLog(const QString &text)
{
    QString now = QDateTime::currentDateTime().toUTC().toString("hh:mm:ss");
    ui->textBrowser->append(QString("[%1] %2").arg(now).arg(text));
}

void PrivateServerWindow::updateUserList()
{
    if (!server)
        return;

    ui->listWidget->clear();
    for (auto userName : server->getConnectedUsersNames()) {
        ui->listWidget->addItem(userName);
    }
}

void PrivateServerWindow::serverStarted()
{
    quint16 serverPort = server->getPort();
    QString serverIP = server->getIP();

    ui->textBrowser->append(QString());
    appendTextInLog("Server started");

    ui->lineEditIP->setText(serverIP);
    ui->lineEditPort->setText(QString::number(serverPort));

    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);

    setServerDetailsVisibility(true);

    ui->listWidget->clear();
}

void PrivateServerWindow::serverStopped()
{
    appendTextInLog("Server stopped!");

    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

    ui->lineEditIP->setText(QString());
    ui->lineEditPort->setText(QString());

    setServerDetailsVisibility(false);

    ui->listWidget->clear();
}

void PrivateServerWindow::setServerDetailsVisibility(bool visible)
{
    ui->labelIP->setVisible(visible);
    ui->labelPort->setVisible(visible);
    ui->lineEditIP->setVisible(visible);
    ui->lineEditPort->setVisible(visible);
}

void PrivateServerWindow::startServer()
{
    if (!server) {
        server.reset(new Server());
        setupServer();
    }

    if (!server->isStarted()) {
        quint16 port = 2049;
        server->start(port);
    }
}

void PrivateServerWindow::stopServer()
{
    if (!server)
        return;

    if (server->isStarted()) {
        server->shutdown();
    }
}
