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


MainWindow::MainWindow() :
    demuxer(nullptr),
    muxer(nullptr)
{
    QGridLayout *mainLayout = createWidgets();

    initializeCamera(mainLayout);

    initializeEncodingStuff();

    timer = new QTimer(this);

    QTimer *switchTimer = new QTimer(this);
    switchTimer->setInterval(2000); // simulate new video interval encoding/decoding
    switchTimer->setSingleShot(false);

    QObject::connect(switchTimer, &QTimer::timeout, [=](){
        muxer->startNewInterval();
    });

    switchTimer->start();

    connect(muxer, &FFMpegMuxer::dataEncoded, [=](const QByteArray &data, bool isFirstPacket){

        if (isFirstPacket && !encodedData.isEmpty()) {
            if (demuxer)
                demuxer->deleteLater();

            demuxer = new FFMpegDemuxer(nullptr, encodedData);

            connect(demuxer, &FFMpegDemuxer::imagesDecoded, this, [=](const QList<QImage> &images, uint frameRate) {
                qDebug() << "decoded images:" << images.size() << " rate:" << frameRate;
                decodedImages << images;

                if (frameRate > 0) {
                    timer->setInterval(1000/frameRate);
                    timer->start();
                }
            });

            demuxer->decode();
            encodedData.clear();
        }

        encodedData.append(data);
    });

    connect(timer, &QTimer::timeout, [=](){

        if (decodedImages.isEmpty())
            return;

        QImage image = decodedImages.takeFirst();
        if (!image.isNull()) {

            //static uint index = 0;
            //if(!image.save(QString("image%1.png").arg(index++)))
            //    qDebug() << "ERROR saving the image";

            QPixmap pixMap = QPixmap::fromImage(image);
            outputLabel->setPixmap(pixMap);
        }
        else
            timer->stop();

    });


    muxer->startNewInterval();

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
    QObject::connect(timer,  &QTimer::timeout, this, &MainWindow::takeCameraSnapshot);
    timer->setInterval(1000/fps);
    timer->start();

    muxer = new FFMpegMuxer();
    muxer->setVideoResolution(camera->viewfinderSettings().resolution());
    muxer->setVideoFrameRate(fps);
    muxer->setVideoQuality(FFMpegMuxer::VideoQualityMedium);
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
    muxer->encodeImage(viewFinder->grab().toImage());
}
