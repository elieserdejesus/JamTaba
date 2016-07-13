#ifndef PRIVATESERVERDIALOG_H
#define PRIVATESERVERDIALOG_H

#include <QDialog>

namespace Controller {
class MainController;
}

namespace Ui {
class PrivateServerDialog;
}

class PrivateServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrivateServerDialog(QWidget *parent, Controller::MainController *mainController);
    ~PrivateServerDialog();
    QString getServer() const;
    QString getPassword() const;
    int getServerPort() const;

public slots:
    void accept() override;

signals:
    void connectionAccepted(const QString &server, int serverPort, const QString &password);

private:
    Ui::PrivateServerDialog *ui;
    Controller::MainController *mainController;

    void buildComboBoxItems();
};

#endif // PRIVATESERVERDIALOG_H
