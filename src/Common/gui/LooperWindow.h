#ifndef LOOPERWINDOW_H
#define LOOPERWINDOW_H

#include <QDialog>

#include "audio/AudioLooper.h"
#include "WavePeakPanel.h"

namespace Ui {
class LooperWindow;
}

class LooperWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LooperWindow(Audio::Looper *looper);
    ~LooperWindow();

private slots:
    void drawNewBufferedSamplesPeak(float peak, quint8 layerIndex);

private:
    Ui::LooperWindow *ui;
    Audio::Looper *looper;
    QMap<quint8, WavePeakPanel*> wavePanels;

};

#endif // LOOPERWINDOW_H
