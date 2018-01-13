#include <QApplication>

#include <QFrame>
#include <QLayout>
#include <QLabel>

#include "gui/widgets/Slider.h"

class TestMainWindow : public QFrame
{

public:
    TestMainWindow()
        :QFrame()
    {
        QHBoxLayout *layout = new QHBoxLayout();

        Slider *verticalSlider = new Slider();
        verticalSlider->setValue(80);
        verticalSlider->setOrientation(Qt::Vertical);
        layout->addWidget(verticalSlider);

        layout->addSpacing(32);

        Slider *horizontalSlider = new Slider();
        horizontalSlider->setValue(80);
        horizontalSlider->setOrientation(Qt::Horizontal);
        layout->addWidget(horizontalSlider, 1);

        layout->addSpacing(32);

        Slider *panSlider = new Slider();
        panSlider->setMaximumWidth(100);
        panSlider->setOrientation(Qt::Horizontal);
        panSlider->setSliderType(Slider::PanSlider);

        QHBoxLayout *panLayout = new QHBoxLayout();
        panLayout->addWidget(new QLabel(QStringLiteral("L")));
        panLayout->addWidget(panSlider);
        panLayout->addWidget(new QLabel(QStringLiteral("R")));

        layout->addLayout(panLayout);

        setLayout(layout);

        setMinimumHeight(400);
        setMinimumWidth(800);
    }

private:

};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMainWindow window;

    window.show();
    return app.exec();
}
