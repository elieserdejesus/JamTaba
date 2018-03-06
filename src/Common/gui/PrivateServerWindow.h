#ifndef PRIVATESERVERWINDOW_H
#define PRIVATESERVERWINDOW_H

#include <QDialog>
#include <QScopedPointer>

#include "upnp/UPnPManager.h"

namespace Ui {
class PrivateServerWindow;
}

namespace ninjam { namespace server {
    class Server;
}}

using ninjam::server::Server;

class PrivateServerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PrivateServerWindow(QWidget *parent = 0);
    ~PrivateServerWindow();
    bool serverIsRunning() const;
    quint16 getServerPort() const;
    QString getServerExternalIP() const;

signals:
    void userConnectingInPrivateServer(QString server, quint16 port);

protected:
    void timerEvent(QTimerEvent *) override;
    void changeEvent(QEvent *) override;

private slots:
    void startServer();
    void stopServer();

    void serverStarted();
    void serverStopped();

    void portOpened(const QString &localIP, const QString &externalIP);
    void upnpError(const QString &error);

private:
    Ui::PrivateServerWindow *ui;

    void setupServer();
    void setServerDetailsVisibility(bool enabled);

    void updateUserList();

    void appendTextInLog(const QString &text);

    QScopedPointer<Server> server;
    UPnPManager upnpManager;

    int networkUsageTimerID;

    static const quint16 PREFERRED_PORT;
};

#endif // PRIVATESERVERWINDOW_H
