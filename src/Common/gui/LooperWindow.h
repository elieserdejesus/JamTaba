#ifndef LOOPERWINDOW_H
#define LOOPERWINDOW_H

#include <QDialog>

#include "audio/AudioLooper.h"
#include "LooperWavePanel.h"

namespace Controller {
class NinjamController;
}

namespace Ui {
class LooperWindow;
}

class LooperWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LooperWindow(Audio::Looper *looper, Controller::NinjamController *controller, const QString &windowTitle);
    ~LooperWindow();

private slots:
    void addSamplesPeak(float peak, uint samplesCount, quint8 layerIndex);

    void updateBeats(int newBpi);

private:
    Ui::LooperWindow *ui;
    Audio::Looper *looper;
    QMap<quint8, LooperWavePanel*> wavePanels;
    Controller::NinjamController *controller;

};

#endif // LOOPERWINDOW_H
