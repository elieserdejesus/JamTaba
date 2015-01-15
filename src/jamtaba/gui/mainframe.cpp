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
    connect(&dialog, SIGNAL(audioIOPropertiesChanged(int,int,int,int,int,int,int)), this, SLOT(on_audioIOPropertiesChanged(int,int,int,int,int,int,int)));
    dialog.exec();

    //audio driver is restarted in on_audioIOPropertiesChanged. This slot is always invoked when AudioIODialog is closed.
}

void MainFrame::on_audioIOPropertiesChanged(int selectedDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    AudioDriver* audioDriver = mainController->getAudioDriver();
#ifdef _WIN32
    audioDriver->setProperties(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#else
    //preciso de um outro on_audioIoPropertiesChanged que me dê o input e o output device
    //audioDriver->setProperties(selectedDevice, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
#endif
    audioDriver->start();
}
