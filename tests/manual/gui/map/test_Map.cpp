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

    QPointF portugal(39.290734, -8.107910);
    QPointF france1(43.698658, 5);
    QPointF france2(43, 5);
    QPointF france3(44, 5.5);
    QPointF france4(43.5, 5);

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    markers << MapMarker("portugal", "Portugal", portugal, flag);
    markers << MapMarker("France 1", "France", france1, flag);
    markers << MapMarker("France 2", "France", france2, flag);
    markers << MapMarker("France 3", "France", france3, flag);
    markers << MapMarker("France 4", "France", france4, flag);

    map->setMarkers(markers);

    return map;
}


MapWidget* createFirstMap()
{
    MapWidget *map = new MapWidget();

    QImage flag = QPixmap(":/flag").toImage();
    QPointF venezuela(7.62388685, -65.25878906);
    QPointF brazil(-27.0, -50.0);
    QPointF israel(31.95216224, 34.23339844);
    QPointF australia(-24.84656535, 132.01171875);
    QPointF japan(37.99616268, 140.80078125);
    QPointF japan2(38, 140.80078125);
    QPointF japan3(37, 140.80078125);
    QPointF france(46.177929, 3.38);
    QPointF france2(46.20, 3.4);

    QList<MapMarker> markers;

    markers << MapMarker("Player 1", "venezuela", venezuela, flag);
    markers << MapMarker("Player 2", "Brazil", brazil, flag);
    markers << MapMarker("Player 3", "Israel", israel, flag);
    markers << MapMarker("Player 4", "Australia", australia, flag);
    markers << MapMarker("Player 4.1", "Australia 2", QPointF(-24.84656535, 132.01171875), flag);
    markers << MapMarker("Player 4.3", "Australia 3", QPointF(-24, 132), flag);
    markers << MapMarker("Player 4.4", "Australia 4", QPointF(-24, 131), flag);
    markers << MapMarker("Player 5", "Japan", japan, flag);
    markers << MapMarker("Player 5.1", "Japan", japan2, flag);
    markers << MapMarker("Player 5.2", "Japan", japan3, flag);
    markers << MapMarker("Player 6", "France", france, flag);
    markers << MapMarker("Player 7", "France", france2, flag);

    map->setMarkers(markers);
    return map;
}

MapWidget* createSecondMap()
{
    MapWidget *map = new MapWidget();

    QPointF france(46.29933031, 5.41520975);
    QPointF malaysia(4.210484, 101.975766);
    QPointF usa1(39.97712029, -80.244141);
    QPointF usa2(36.66841912, -79.71679725);
    QPointF usa3(40.91351276, -108.19335975);

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    markers << MapMarker("Player 1", "france", france, flag);
    markers << MapMarker("Player 2", "malaysia", malaysia, flag);
    markers << MapMarker("Player 3", "USA", usa1, flag);
    markers << MapMarker("Player 4", "USA", usa2, flag);
    markers << MapMarker("Player 5", "USA", usa3, flag);

    map->setMarkers(markers);

    return map;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;

    //mainWindow.setMaximumHeight(0);

    QWidget *contentWidget = new QWidget();
    contentWidget->setMinimumWidth(800);
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
