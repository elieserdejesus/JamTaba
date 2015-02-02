#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainframe.h"

namespace Controller {
    class MainController;
}

namespace Gui{
    class MainFrame;


class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
    MainFrame(Controller::MainController* mainController, QWidget *parent = 0);
	~MainFrame();
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent*);
    virtual void changeEvent(QEvent *);
    virtual void timerEvent(QTimerEvent *);

private slots:
    void on_actionAudio_triggered();
    void on_audioIOPropertiesChanged(int selectedDevice, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);

private:
	Ui::MainFrameClass ui;
    Controller::MainController* mainController;
    int timerID;
};
}



