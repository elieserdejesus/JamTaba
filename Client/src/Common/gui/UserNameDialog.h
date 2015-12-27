#ifndef USERNAMEDIALOG_H
#define USERNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class UserNameDialog;
}

class UserNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserNameDialog(QWidget *parent, QString lastUserName);
    ~UserNameDialog();
    QString getUserName() const;
private slots:
    void on_textEdited();
private:
    Ui::UserNameDialog *ui;
};

#endif // USERNAMEDIALOG_H
