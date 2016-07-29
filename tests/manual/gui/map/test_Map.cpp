#include <QApplication>
#include <QDebug>
#include "widgets/MapWidget.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QPixmap>

MapWidget* createFourthMap()
{
    MapWidget *map = new MapWidget();

    QPointF australia(-24.84656535, 132.01171875);
    QPointF japan(37.99616268, 140.80078125);

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    markers << MapMarker("Player 8", "Japan", japan, flag);
    markers << MapMarker("Player 9", "australia", australia, flag);

    map->setMarkers(markers);

    return map;
}

MapWidget* createThirdMap()
{
    MapWidget *map = new MapWidget();

    QPointF venezuela(7.62388685, -65.25878906);
    QPointF malvines(-51.65892665, -58.87573242);

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    markers << MapMarker("Player 6", "venezuela", venezuela, flag);
    markers << MapMarker("Player 7", "Malvines", malvines, flag);

    map->setMarkers(markers);

    return map;
}


MapWidget* createFirstMap()
{
    MapWidget *map = new MapWidget();

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    for (int i = 0; i < 7; ++i) {
        markers << MapMarker("Player " + QString::number(i), "test " + QString::number(i), QPointF(-27 + i, -50 + i), flag);
    }

    //markers << MapMarker("Player 2", "test 2", QPointF(-27, -50), flag);
    //markers << MapMarker("Player 3", "test 3", QPointF(-27.6, -50), flag);

    map->setMarkers(markers);

    return map;
}

MapWidget* createSecondMap()
{
    MapWidget *map = new MapWidget();

    QPointF israel(31.95216224, 34.23339844);
    QPointF australia(-24.84656535, 132.01171875);
    QPointF japan(37.99616268, 140.80078125);

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    markers << MapMarker("Player 3", "Japan", japan, flag);
    markers << MapMarker("Player 4", "israel", israel, flag);
    markers << MapMarker("Player 5", "australia", australia, flag);

    map->setMarkers(markers);

    return map;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setMinimumWidth(800);
    //mainWindow.setMaximumHeight(0);

    QWidget *contentWidget = new QWidget();
    //contentWidget->setMaximumHeight(250);

    QGridLayout *layout = new QGridLayout();
    contentWidget->setLayout(layout);

    layout->addWidget(createFirstMap(), 0, 0);
    layout->addWidget(createSecondMap(), 0, 1);
    layout->addWidget(createThirdMap(), 1, 0);
    layout->addWidget(createFourthMap(), 1, 1);

    mainWindow.setCentralWidget(contentWidget);
    mainWindow.show();

    return app.exec();
}
