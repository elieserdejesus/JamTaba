#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainframe.h"
#include "../maincontroller.h"

class MainFrame : public QMainWindow
{
	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0);
	~MainFrame();
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent*);

private slots:
    void on_actionAudio_triggered();

private:
	Ui::MainFrameClass ui;
    MainController* mainController;
};
