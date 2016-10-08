#include <QApplication>
#include "widgets/MultiStateButton.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QFrame>

class Window : public QMainWindow
{
public:
    Window()
    {
        setMaximumWidth(250);
        setMaximumHeight(200);

        QFrame *content = new QFrame();
        content->setLayout(new QHBoxLayout());
        setCentralWidget(content);

        button = new MultiStateButton(3, this);
        button->setObjectName("button");
        connect(button, &MultiStateButton::clicked, this, &Window::changeButtonText);
        button->setCheckable(true);

        QString css = "#button[state=\"0\"]{ background-color: red; }";
        css += "#button[state=\"1\"]{ background-color: blue; }";
        css += "#button[state=\"2\"]{ background-color: green; }";
        button->setStyleSheet(css);

        content->layout()->addWidget(button);
    }

private:
    MultiStateButton *button;

private slots:
    void changeButtonText() {
        quint8 currentText = button->getCurrentState();
        button->setText(QString::number(currentText));
    }

};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window mainWindow;

    mainWindow.show();

    return app.exec();
}
