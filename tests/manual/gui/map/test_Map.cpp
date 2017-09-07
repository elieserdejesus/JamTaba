#include <QApplication>
#include <QDebug>
#include "widgets/MapWidget.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QPixmap>

MapWidget* createFourthMap()
{
    MapWidget *map = new MapWidget();

    QPointF canada(56.130366, -106.346771);
    QPointF uk(55.378051, -3.435973);

    QImage flag = QPixmap(":/flag").toImage();
    QList<MapMarker> markers;
    markers << MapMarker("Pete", "Canada", canada, flag);
    markers << MapMarker("Generic_person_3", "United Kingdom", uk, flag);

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
    markers << MapMarker("testing_user_with_very_big_name", "France", france4, flag);

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
    QPointF france(46.177929, 3.38);
    QPointF france2(46.20, 3.4);

    QList<MapMarker> markers;

    markers << MapMarker("Player 1", "venezuela", venezuela, flag);
    markers << MapMarker("Player 2", "Brazil", brazil, flag);
    markers << MapMarker("Player 3", "Israel", israel, flag);
    markers << MapMarker("Player 4", "Australia", australia, flag);
    markers << MapMarker("Player 5", "Japan", japan, flag);
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
    contentWidget->setMinimumWidth(900);
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
