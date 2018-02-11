#include "PrivateServerWindow.h"
#include "ui_PrivateServerWindow.h"

PrivateServerWindow::PrivateServerWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrivateServerWindow)
{
    ui->setupUi(this);
}

PrivateServerWindow::~PrivateServerWindow()
{
    delete ui;
}
