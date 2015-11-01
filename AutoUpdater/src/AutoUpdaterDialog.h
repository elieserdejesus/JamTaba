#ifndef AUTOUPDATERDIALOG_H
#define AUTOUPDATERDIALOG_H

#include <QDialog>

namespace Ui {
class AutoUpdaterDialog;
}

class AutoUpdaterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoUpdaterDialog(QWidget *parent = 0);
    ~AutoUpdaterDialog();

    void setDownloadProgress(int progressValue);

    void setStatusText(QString text);

private:
    Ui::AutoUpdaterDialog *ui;
};

#endif // AUTOUPDATERDIALOG_H
