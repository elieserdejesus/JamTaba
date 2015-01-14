#include "mainframe.h"
#include <QCloseEvent>
#include "audioiodialog.h"
#include <QDebug>

MainFrame::MainFrame(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
    mainController = new MainController();

}

void MainFrame::closeEvent(QCloseEvent *)
 {
    if(mainController != NULL){
        mainController->stop();
    }
}

void MainFrame::showEvent(QShowEvent *)
{
    mainController->start();
}

MainFrame::~MainFrame()
{
    delete mainController;
}

//audio preferences
void MainFrame::on_actionAudio_triggered()
{
    PortAudioDriver* driver = (PortAudioDriver*)mainController->getAudioDriver();
    driver->stop();
    AudioIODialog dialog(*driver, this);
    int result = dialog.exec();
    if(result == QDialog::Accepted){
        qDebug() << "accepted";
    }
    else{
        qDebug() << "rejected";
    }
    driver->start();
}
