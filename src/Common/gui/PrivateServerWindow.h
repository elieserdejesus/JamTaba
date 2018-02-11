#ifndef PRIVATESERVERWINDOW_H
#define PRIVATESERVERWINDOW_H

#include <QDialog>

namespace Ui {
class PrivateServerWindow;
}

class PrivateServerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PrivateServerWindow(QWidget *parent = 0);
    ~PrivateServerWindow();

private:
    Ui::PrivateServerWindow *ui;
};

#endif // PRIVATESERVERWINDOW_H
