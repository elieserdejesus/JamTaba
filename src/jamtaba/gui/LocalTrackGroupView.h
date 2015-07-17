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

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void updatePeaks();
    void refreshInputSelectionName(int inputTrackIndex);
    inline QList<LocalTrackView*> getTracks() const{return trackViews;}

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
    static const int MAX_SUB_CHANNELS = 3;
};

#endif // LOCALTRACKGROUPVIEW_H
