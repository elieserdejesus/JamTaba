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
//    inline QSize minimumSize() const{return minimumSizeHint();}
//    inline QSize maximumSize() const{return minimumSizeHint();}
//    inline int minimumWidth() const{return minimumSizeHint().width();}

    void updatePeaks();
    void refreshInputSelectionName(int inputTrackIndex);
    inline int getTracksCount() const{return trackViews.size();}

    void setUnlightStatus(bool unlighted);

    virtual void setFaderOnlyMode(bool faderOnly);
    virtual void toggleFaderOnlyMode();
    bool isFaderOnly() const{return faderOnly;}

protected:
    void paintEvent(QPaintEvent* );

    QList<BaseTrackView*> trackViews;

    Ui::TrackGroupView *ui;

    bool faderOnly;

//private slots:
//    void on_toolButton_clicked();

//    void onAddSubChannelClicked();
//    void on_toolButtonActionTriggered(QAction*);
//    void on_toolButtonActionHovered(QAction*);


};

#endif // TRACKGROUPVIEW_H
