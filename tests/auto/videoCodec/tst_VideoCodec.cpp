#include <QObject>
#include <QtTest/QtTest>
#include <QImage>

#include "FFMpegMuxer.h"
#include "FFMpegDemuxer.h"

class TestVideoCodec: public QObject
{
    Q_OBJECT

private slots:
    void encodeDecode();
};

void TestVideoCodec::encodeDecode()
{
    const QSize resolution(320, 180);
    const qreal frameRate = 10;
    const quint8 videoTime = 3; // in seconds
    const quint64 framesToEncode = videoTime * frameRate;

    QByteArray encodedData;

    bool encoding = true;

    FFMpegMuxer *muxer = new FFMpegMuxer();

    QObject::connect(muxer, &FFMpegMuxer::encodingFinished, [&]() {
        encoding = false;

        //    QFile file("C:/Users/Elieser/Documents/build-TestLibx264-Qt_MSVC_64bits-Debug/teste.h264");
        //    if(!file.open(QIODevice::WriteOnly))
        //        qCritical() << file.errorString();
        //    file.write(encodedData);
        //    file.close();

        FFMpegDemuxer demuxer(nullptr, encodedData);
        QObject::connect(&demuxer, &FFMpegDemuxer::imagesDecoded, [&](QList<QImage> images, uint fps){
            qDebug() << "images decoded:" << images.size() << " fps:" << fps;
            QCOMPARE((uint)frameRate, fps);
            QCOMPARE((quint64)images.size(), framesToEncode);
        });

        demuxer.decode();
    });

    QObject::connect(muxer, &FFMpegMuxer::dataEncoded, [&](const QByteArray &data, bool isFirstPacket) {

        if (encoding)
            encodedData.append(data);

    });

    muxer->setVideoFrameRate(frameRate);
    muxer->setVideoResolution(resolution);
    muxer->startNewInterval();

    for (int i = 0; i < framesToEncode; ++i) {
        QImage img(resolution, QImage::Format_RGB32);
        img.fill(QColor(rand() % 255, rand() % 255, rand() % 255));
        muxer->encodeImage(img, false); // encoding without threads
    }

    // finish the interval
    muxer->startNewInterval();
    muxer->encodeImage(QImage(), false); // encoding without threads
}


int main(int argc, char *argv[])
{
    int status = 0;

    TestVideoCodec test;
    status |= QTest::qExec(&test, argc, argv);

    return status;
}

#include "tst_VideoCodec.moc"
