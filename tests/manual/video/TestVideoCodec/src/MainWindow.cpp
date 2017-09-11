#include "MainWindow.h"

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QTimer>
#include <QGridLayout>
#include <QLabel>
#include <QPicture>
#include <QPixmap>
#include <QFile>
#include <QFileInfo>


MainWindow::MainWindow()
{
    QGridLayout *mainLayout = createWidgets();

    initializeCamera(mainLayout);

    initializeEncodingStuff();

    decodeTimer = new QTimer(this);

    QTimer *switchTimer = new QTimer(this);
    switchTimer->setInterval(5000); // simulate new video interval encoding/decoding
    switchTimer->setSingleShot(false);

    QObject::connect(switchTimer, &QTimer::timeout, [=](){
        muxer.startNewInterval();
    });

    switchTimer->start();

    QObject::connect(&demuxer, &FFMpegDemuxer::opened, this, &MainWindow::startDecoding);

    QObject::connect(&muxer, &FFMpegMuxer::dataEncoded, [=](const QByteArray &data, bool isFirstPacket){

        if (isFirstPacket && !encodedData.isEmpty()) {
            demuxer.open(encodedData);

            encodedData.clear();
        }

        encodedData.append(data);
    });

    connect(decodeTimer, &QTimer::timeout, [=](){
        QImage image = demuxer.decodeNextFrame();
        if (!image.isNull())
            outputLabel->setPixmap(QPixmap::fromImage(image));
        else
            decodeTimer->stop();

    });


    muxer.startNewInterval();

}

void MainWindow::startDecoding(uint frameRate)
{
    decodeTimer->setInterval(1000/frameRate);
    decodeTimer->start();
}

QGridLayout *MainWindow::createWidgets()
{
    QWidget *centralWidget = new QWidget();
    QGridLayout *mainLayout = new QGridLayout();
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // create output label
    outputLabel = new QLabel(this);
    mainLayout->addWidget(outputLabel, 0, 1, 1, 1);

    mainLayout->addWidget(new QLabel("Camera"), 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
    mainLayout->addWidget(new QLabel("Encoded Video"), 1, 1, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

    return mainLayout;
}

void MainWindow::initializeEncodingStuff()
{
    qreal fps = 10;
    QTimer *timer = new QTimer(this);
    QObject::connect(timer,  SIGNAL(timeout()), this, SLOT(takeCameraSnapshot()));
    timer->setInterval(1000/fps);
    timer->start();

    muxer.setVideoResolution(camera->viewfinderSettings().resolution());
    muxer.setVideoFrameRate(fps);
    muxer.setVideoQuality(FFMpegMuxer::LOW_VIDEO_QUALITY);
    muxer.setSaveToFile(false);

}

void MainWindow::initializeCamera(QGridLayout *layout)
{
    viewFinder = new QCameraViewfinder();
    camera = new QCamera(QCameraInfo::defaultCamera());
    camera->setViewfinder(viewFinder);

    camera->load(); // loading camera to get resolutions

    QCameraViewfinderSettings settings = camera->supportedViewfinderSettings().first();
    settings.setResolution(camera->supportedViewfinderResolutions().first());
    camera->setViewfinderSettings(settings);

    setMinimumWidth(settings.resolution().width() * 2);

    layout->addWidget(viewFinder, 0, 0, 1, 1);

    camera->setCaptureMode(QCamera::CaptureStillImage);
    camera->start();
}

void MainWindow::takeCameraSnapshot()
{
    muxer.encodeImage(viewFinder->grab().toImage());
}
