#ifndef TRACKIOPANEL_H
#define TRACKIOPANEL_H

#include <QWidget>

namespace Ui {
class TrackIOPanel;
}

class TrackIOPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TrackIOPanel(QWidget *parent = 0);
    ~TrackIOPanel();

private:
    Ui::TrackIOPanel *ui;
};

#endif // TRACKIOPANEL_H
