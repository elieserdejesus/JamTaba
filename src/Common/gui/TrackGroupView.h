#ifndef TRACKGROUPVIEW_H
#define TRACKGROUPVIEW_H

#include <QFrame>
#include <QLineEdit>
#include <QBoxLayout>

namespace Ui {
class TrackGroupView;
}

class BaseTrackView;
class QMenu;

class TrackGroupView : public QFrame
{
    Q_OBJECT

public:
    explicit TrackGroupView(QWidget *parent = 0);
    ~TrackGroupView();
    virtual void setGroupName(const QString &groupName);
    QString getGroupName() const;

    virtual BaseTrackView *addTrackView(long trackID);

    void removeTrackView(BaseTrackView *trackView);
    void removeTrackView(int index);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void updateGuiElements();

    inline int getTracksCount() const
    {
        return trackViews.size();
    }

    void setUnlightStatus(bool unlighted);
    bool isUnlighted() const;

    //is not possible return a covariant container, so I'm using template to return a container of a more specific (derived) type
    template<class T>
    QList<T> getTracks() const
    {
        QList<T> castedTracks;
        foreach (BaseTrackView *trackView, trackViews) {
            castedTracks.append(dynamic_cast<T>(trackView));
        }
        return castedTracks;
    }

protected:
    //void paintEvent(QPaintEvent *);

    QList<BaseTrackView *> trackViews;

    virtual BaseTrackView *createTrackView(long trackID) = 0;

    QLineEdit *groupNameField;
    QWidget *topPanel;
    QBoxLayout *tracksLayout;
    QBoxLayout *topPanelLayout;
    QBoxLayout *mainLayout;

    virtual void refreshStyleSheet();

private:
    void setupUI();
};

#endif // TRACKGROUPVIEW_H
