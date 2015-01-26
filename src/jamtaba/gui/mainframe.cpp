#include "mainframe.h"
#include <QCloseEvent>
#include "audioiodialog.h"
#include "../ConfigStore.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include "../JamtabaFactory.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::MainFrame(MainController *mainController, QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
    this->mainController = mainController;

    if(ConfigStore::windowWasMaximized()){
        setWindowState(Qt::WindowMaximized);
    }
    else{
        QPointF location = ConfigStore::getLastWindowLocation();
        QDesktopWidget* desktop = QApplication::desktop();
        int desktopWidth = desktop->width();
        int desktopHeight = desktop->height();
        int x = desktopWidth * location.x();
        int y = desktopHeight * location.y();
        this->move(x, y);
    }
    timerID = startTimer(50);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::timerEvent(QTimerEvent *){
    //AbstractAudioDriver* audioDriver = (AbstractAudioDriver*)mainController->getAudioDriver();
    //const float* peaks = audioDriver->getOutputBuffer()->getPeaks();
    //peakMeter->setPeak(peaks[0]);
}

//++++++++++++=
void MainFrame::changeEvent(QEvent *ev){
    if(ev->type() == QEvent::WindowStateChange){
        ConfigStore::storeWindowState(isMaximized());
    }
    ev->accept();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::closeEvent(QCloseEvent *)
 {
    qDebug() << "MainFrame::closeEvent";
    if(mainController != NULL){
        mainController->stop();
    }
    //store window position
    QRect screen = QApplication::desktop()->screenGeometry();
    float x = (float)this->pos().x()/screen.width();
    float y = (float)this->pos().y()/screen.height();
    QPointF location(x, y) ;
    ConfigStore::storeWindowLocation( location ) ;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MainFrame::showEvent(QShowEvent *)
{
    mainController->start();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MainFrame::~MainFrame()
{
    killTimer(timerID);
    //delete mainController;
    //delete peakMeter;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//audio preferences
void MainFrame::on_actionAudio_triggered()
{
    PortAudioDriver* driver = (PortAudioDriver*)mainController->getAudioDriver();
    driver->stop();
    AudioIODialog dialog(*driver, this);
    connect(&dialog, SIGNAL(audioIOPropertiesChanged(int,int,int,int,int,int,int)), this, SLOT(on_audioIOPropertiesChanged(int,int,int,int,int,int,int)));
    dialog.exec();
    driver->start();
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

    ConfigStore::storeAudioSettings(firstIn, lastIn, firstOut, lastOut, selectedDevice, selectedDevice, sampleRate, bufferSize);
}


