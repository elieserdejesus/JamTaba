#ifndef PRIVATESERVERWINDOW_H
#define PRIVATESERVERWINDOW_H

#include <QDialog>
#include <QScopedPointer>

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

private slots:
    void startServer();
    void stopServer();

    void serverStarted();
    void serverStopped();

private:
    Ui::PrivateServerWindow *ui;

    void setupServer();
    void setServerDetailsVisibility(bool enabled);

    void updateUserList();

    void appendTextInLog(const QString &text);

    QScopedPointer<Server> server;
};

#endif // PRIVATESERVERWINDOW_H
