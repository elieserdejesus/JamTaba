#ifndef TRACKGROUPVIEW_H
#define TRACKGROUPVIEW_H

#include <QWidget>

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
    virtual void setGroupName(QString groupName);
    QString getGroupName() const;

    virtual BaseTrackView *addTrackView(long trackID);

    void removeTrackView(BaseTrackView *trackView);
    void removeTrackView(int index);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void updateGuiElements();

    //void refreshInputSelectionName(int inputTrackIndex);
    inline int getTracksCount() const
    {
        return trackViews.size();
    }

    void setUnlightStatus(bool unlighted);
    bool isUnlighted() const;

protected:
    void paintEvent(QPaintEvent *);

    QList<BaseTrackView *> trackViews;

    Ui::TrackGroupView *ui;

    virtual BaseTrackView *createTrackView(long trackID) = 0;
};

#endif // TRACKGROUPVIEW_H
