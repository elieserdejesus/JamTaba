#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QCamera>
#include <QCameraViewfinder>
#include <QLabel>
#include <QTimer>
#include <QImage>

#include "FFMpegMuxer.h"
#include "FFMpegDemuxer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void takeCameraSnapshot();

private:
    QCamera *camera;
    QCameraViewfinder *viewFinder;
    QLabel *outputLabel;

    QTimer *timer;

    QGridLayout *createWidgets();
    void initializeEncodingStuff();
    void initializeCamera(QGridLayout *layout);

    FFMpegMuxer *muxer;
    FFMpegDemuxer *demuxer;

    QByteArray encodedData;

    QList<QImage> decodedImages;

};

#endif // MAINWINDOW_H
