#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include <QWidget>
#include "LocalTrackView.h"

namespace Ui {
class LocalTrackGroupView;
}

class QMenu;

class LocalTrackGroupView : public QWidget
{
    Q_OBJECT

public:
    explicit LocalTrackGroupView(QWidget *parent = 0);
    ~LocalTrackGroupView();
    void setGroupName(QString groupName);
    QString getGroupName() const;

    void addTrackView(LocalTrackView* trackView);

protected:
    void paintEvent(QPaintEvent* );

private slots:
    void on_toolButton_clicked();

    void onAddSubChannelClicked();
    void on_toolButtonActionTriggered(QAction*);
    void on_toolButtonActionHovered(QAction*);

private:
    Ui::LocalTrackGroupView *ui;
    QList<LocalTrackView*> trackViews;

};

#endif // LOCALTRACKGROUPVIEW_H
