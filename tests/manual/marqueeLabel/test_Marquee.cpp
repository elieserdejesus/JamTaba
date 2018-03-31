#include <QApplication>
#include "widgets/MarqueeLabel.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>

class Window : public QMainWindow
{
public:
    Window()
    {
        setMaximumWidth(250);
        setMaximumHeight(40);

        QFrame *content = new QFrame();
        content->setLayout(new QHBoxLayout());
        setCentralWidget(content);

        MarqueeLabel::setTimeBetweenAnimations(5000);

        marqueeLabel = new MarqueeLabel();
        marqueeLabel->setMaximumWidth(100);
        marqueeLabel->setText("Testing a big text to see the marquee behavior");

        commonLabel = new MarqueeLabel();
        commonLabel->setText("test");

        marqueeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        commonLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        content->layout()->addWidget(marqueeLabel);
        content->layout()->addWidget(commonLabel);
        QLabel *label = new QLabel("other label");
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        content->layout()->addWidget(label);

        startTimer(50);
    }

protected:
    void timerEvent(QTimerEvent *) override{
        //qDebug() << "updating marquees";
        marqueeLabel->updateMarquee();
        commonLabel->updateMarquee();
    }

private:
    MarqueeLabel *marqueeLabel;
    MarqueeLabel *commonLabel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Window mainWindow;

    mainWindow.show();

    return app.exec();
}
