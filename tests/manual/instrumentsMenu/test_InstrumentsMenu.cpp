#include <QApplication>

#include <QMainWindow>
#include <QBoxLayout>
#include <QDir>

#include "widgets/InstrumentsMenu.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;

    window.setMinimumSize(QSize(400, 600));

    auto widget = new QWidget(&window);
    window.setCentralWidget(widget);

    widget->setLayout(new QVBoxLayout());

    QDir dir(":/instruments");
    auto infos = dir.entryInfoList();
    QList<QIcon> icons;
    for (auto info : infos) {
        icons.append(QIcon(":/instruments/" + info.completeBaseName()));
    }

    QIcon defaultIcon(":/instruments/jtba.png");
    auto instrumentsButton = new InstrumentsButton(defaultIcon, icons);

    widget->layout()->addWidget(instrumentsButton);

    window.show();

    return app.exec();
}
