#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QCamera>
#include <QCameraViewfinder>
#include <QLabel>
#include <QTimer>

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

    QTimer *decodeTimer;

    QGridLayout *createWidgets();
    void initializeEncodingStuff();
    void initializeCamera(QGridLayout *layout);

    FFMpegMuxer muxer;
    FFMpegDemuxer demuxer;

    QByteArray encodedData;

private slots:
    void startDecoding(uint frameRate);

};

#endif // MAINWINDOW_H
