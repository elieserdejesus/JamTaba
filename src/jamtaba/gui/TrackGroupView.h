#ifndef TRACKGROUPVIEW_H
#define TRACKGROUPVIEW_H

#include <QWidget>
//#include "LocalTrackView.h"

namespace Ui {
class TrackGroupView;
}

class BaseTrackView;
class QMenu;

class TrackGroupView : public QWidget
{
    Q_OBJECT

public:
    explicit TrackGroupView(QWidget *parent = 0);
    ~TrackGroupView();
    void setGroupName(QString groupName);
    QString getGroupName() const;

    virtual void addTrackView(BaseTrackView* trackView);
    void removeTrackView(BaseTrackView* trackView);
    void removeTrackView(int index);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updatePeaks();
    void refreshInputSelectionName(int inputTrackIndex);
    inline int getTracksCount() const{return trackViews.size();}

    void setUnlightStatus(bool unlighted);


protected:
    void paintEvent(QPaintEvent* );

    QList<BaseTrackView*> trackViews;

    Ui::TrackGroupView *ui;

//private slots:
//    void on_toolButton_clicked();

//    void onAddSubChannelClicked();
//    void on_toolButtonActionTriggered(QAction*);
//    void on_toolButtonActionHovered(QAction*);


};

#endif // TRACKGROUPVIEW_H
