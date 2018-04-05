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
    const qreal frameRate = 25;
    const quint8 videoTime = 10; // in seconds

    QByteArray encodedData;

    bool encoding = true;

    FFMpegMuxer muxer;

    QObject::connect(&muxer, &FFMpegMuxer::encodingFinished, [&]() {
        encoding = false;
    });

    QObject::connect(&muxer, &FFMpegMuxer::dataEncoded, [&](const QByteArray &data, bool isFirstPacket) {

        if (encoding)
            encodedData.append(data);

    });

    muxer.setVideoFrameRate(frameRate);
    muxer.setVideoResolution(resolution);
    muxer.startNewInterval();

    const quint64 framesToEncode = videoTime * frameRate;
    for (int i = 0; i < framesToEncode; ++i) {
        QImage img(resolution, QImage::Format_RGB32);
        img.fill(QColor(rand() % 255, rand() % 255, rand() % 255));
        muxer.encodeImage(img);
    }

    // finish the interval
    muxer.startNewInterval();
    muxer.encodeImage(QImage());

//    QFile mp4("C:/Users/Elieser/Downloads/jellyfish-25-mbps-hd-hevc.mp4");
//    QVERIFY(mp4.open(QIODevice::ReadOnly));
//    encodedData.append(mp4.readAll());

    qDebug() << "Encoded data:" << encodedData.size();
    QFile mp4("C:/Users/Elieser/Documents/build-TestLibx264-Qt_MSVC_64bits-Debug/teste.mp4");
    if(!mp4.open(QIODevice::WriteOnly))
        qCritical() << mp4.errorString();
    mp4.write(encodedData);
    mp4.close();

//    FFMpegDemuxer demuxer(nullptr, encodedData);

//    QObject::connect(&demuxer, &FFMpegDemuxer::imagesDecoded, [&](QList<QImage> images, uint fps){
//        //qDebug() << "frames decoded:" << images.size();
//        //QCOMPARE(fps, (quint32)30);
//        QCOMPARE(fps, (uint)frameRate);
//        QCOMPARE(framesToEncode, (quint64)images.size());
//    });

//    demuxer.decode();

}


int main(int argc, char *argv[])
{
    int status = 0;

    TestVideoCodec test;
    status |= QTest::qExec(&test, argc, argv);

    return status;
}

#include "tst_VideoCodec.moc"
