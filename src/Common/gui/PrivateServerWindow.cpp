#include "PrivateServerWindow.h"
#include "ui_PrivateServerWindow.h"
#include "ninjam/server/Server.h"

#include <QDateTime>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

const quint16 PrivateServerWindow::PREFERRED_PORT = 2049;

PrivateServerWindow::PrivateServerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrivateServerWindow),
    networkUsageTimerID(0)
{
    ui->setupUi(this);
    
#ifdef Q_OS_MAC
    setWindowFlags(windowFlags() | Qt::Popup);
#endif

    connect(ui->pushButtonStart, &QPushButton::clicked, this, &PrivateServerWindow::startServer);
    connect(ui->pushButtonStop, &QPushButton::clicked, this, &PrivateServerWindow::stopServer);

    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

    connect(&upnpManager, &UPnPManager::portOpened, this, &PrivateServerWindow::portOpened);

    connect(&upnpManager, &UPnPManager::portClosed, [=](){
        appendTextInLog(tr("Port closed in your router!"));
    });

    connect(&upnpManager, &UPnPManager::errorDetected, this, &PrivateServerWindow::upnpError);

    setServerDetailsVisibility(false);

    ui->labelIPValue->setText(QString());
    ui->labelPortValue->setText(QString());
}

bool PrivateServerWindow::serverIsRunning() const
{
    if (!server)
        return false;

    return server->isStarted();
}

quint16 PrivateServerWindow::getServerPort() const
{
    if (!serverIsRunning())
        return 0;

    return server->getPort();
}

QString PrivateServerWindow::getServerExternalIP() const
{
    if (!serverIsRunning())
        return QString();

    return upnpManager.getExternalIp();
}

void PrivateServerWindow::upnpError(const QString &error)
{
    //: The '%1' symbol will be replaced by the UPnP error description, please keep the '%1' symbol in your translation
    appendTextInLog(tr("ERROR in UPnP protocol: %1").arg(error));
}

void PrivateServerWindow::portOpened(const QString &localIP, const QString &externalIP)
{
    Q_UNUSED(localIP);
    appendTextInLog(tr("Port opened in your router using UPnP protocol! Other ninjamers can connect in your server!"));
    appendTextInLog(tr("Your external IP is %1").arg(externalIP));

    ui->labelIPValue->setText(externalIP);
    ui->labelPortValue->setText(QString::number(PREFERRED_PORT));

    setServerDetailsVisibility(true);

    QString title = tr("Your Private Server");
    QString text = tr("Connect with private server ...");
    auto buttons = QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No);
    int result = QMessageBox::question(this, title, text, buttons);
    if (result == QMessageBox::Yes) {
        emit userConnectingInPrivateServer(localIP, PREFERRED_PORT);
    }
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
        appendTextInLog(tr("Incomming connection from %1").arg(ip));
    });

    connect(server.data(), &Server::userEntered, [=](const QString &userName){
        appendTextInLog(tr("%1 entered in the server").arg(userName));

        updateUserList();
    });

    connect(server.data(), &Server::userLeave, [=](const QString &userName){
        appendTextInLog(tr("%1 left the server").arg(userName));

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
    ui->textBrowser->append(QString());
    appendTextInLog(tr("Server started"));

    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);

    ui->listWidget->clear();

    if (networkUsageTimerID)
        killTimer(networkUsageTimerID);

    networkUsageTimerID = startTimer(3000);


    // UPnP
    appendTextInLog(tr("Trying to open the port %1 in your router (UPnP) to allow external connections ...").arg(server->getPort()));
    QtConcurrent::run(&upnpManager, &UPnPManager::openPort, server->getPort());
}

void PrivateServerWindow::serverStopped()
{
    appendTextInLog(tr("Server stopped!"));

    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);

    ui->labelIPValue->setText(QString());
    ui->labelPortValue->setText(QString());

    setServerDetailsVisibility(false);

    ui->listWidget->clear();

    if (networkUsageTimerID)
        killTimer(networkUsageTimerID);
}

void PrivateServerWindow::setServerDetailsVisibility(bool visible)
{
    ui->labelIP->setVisible(visible);
    ui->labelPort->setVisible(visible);
    ui->labelIPValue->setVisible(visible);
    ui->labelPortValue->setVisible(visible);

    ui->labelDownload->setVisible(visible);
    ui->labelDownloadValue->setVisible(visible);
    ui->labelUpload->setVisible(visible);
    ui->labelUploadValue->setVisible(visible);
}

void PrivateServerWindow::startServer()
{
    if (!server) {
        server.reset(new Server());
        setupServer();
    }

    if (!server->isStarted()) {
        quint16 port = PREFERRED_PORT;
        server->start(port);
    }
}

void PrivateServerWindow::stopServer()
{
    if (!server)
        return;

    if (server->isStarted()) {
        server->shutdown();

        QtConcurrent::run(&upnpManager, &UPnPManager::closePort, PREFERRED_PORT);
    }
}

void PrivateServerWindow::timerEvent(QTimerEvent *)
{
    if (!server)
        return;

    quint64 download = server->getDownloadTransferRate() / 1024 * 8; // Kbps
    quint64 upload = server->getUploadTransferRate() / 1024 * 8;
    ui->labelDownloadValue->setText(QString::number(download));
    ui->labelUploadValue->setText(QString::number(upload));
}

void PrivateServerWindow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::LanguageChange) {

        // translate strings typed directly in PrivateServerWindow.ui file
        ui->retranslateUi(this);
    }

    QDialog::changeEvent(ev);
}
