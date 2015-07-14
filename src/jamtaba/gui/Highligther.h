#ifndef HIGHLIGTHER_H
#define HIGHLIGTHER_H

#include <QTimer>
#include <QObject>

class QWidget;

class Highligther : public QObject
{
    Q_OBJECT

public:

    static Highligther* getInstance();

    void highlight(QWidget*);

private slots:
    void on_timerStopped();
    void on_objectDestroyed(QObject*);

private:
    Highligther();
    ~Highligther();
    QTimer timer;
    QWidget* lastHighlightedWidget;
    static Highligther* instance;
    void setWidgetHighlightStatus(QWidget* w, bool highlighted);
    static const int HIGHLIGHT_TIME = 4000;
};

#endif // HIGHLIGTHER_H
