#ifndef BUSYDIALOG_H
#define BUSYDIALOG_H

#include <QDialog>

namespace Ui {
class BusyDialog;
}

class BusyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BusyDialog(QWidget *parent = 0);
    ~BusyDialog();

    void show(const QString &message);
    virtual void show();

protected:
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private:
    Ui::BusyDialog *ui;
    static QRegion roundedRect(const QRect &rect, int round);
};

#endif // BUSYDIALOG_H
