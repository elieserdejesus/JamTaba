#ifndef PRIVATESERVERDIALOG_H
#define PRIVATESERVERDIALOG_H

#include <QDialog>

namespace Ui {
class PrivateServerDialog;
}

class PrivateServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrivateServerDialog(QWidget *parent, QString lastServer, int lastServerPort,
                                 const QString &lastPassword);
    ~PrivateServerDialog();
    QString getServer() const;
    QString getPassword() const;
    int getServerPort() const;
signals:
    void connectionAccepted(const QString &server, int serverPort, const QString &password);
private slots:
    void on_okButtonTriggered();
private:
    Ui::PrivateServerDialog *ui;
};

#endif // PRIVATESERVERDIALOG_H
