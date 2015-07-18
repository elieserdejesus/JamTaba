#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"

class LocalTrackView;

class QPushButton;

namespace Ui {
class LocalTrackGroupView;
}


class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT
public:
    LocalTrackGroupView(int index);
    ~LocalTrackGroupView();
    void refreshInputSelectionName(int inputTrackIndex);
    QList<LocalTrackView*> getTracks() const;
    virtual void addTrackView(BaseTrackView *trackView);
    inline int getIndex() const{return index;}
signals:
    void nameChanged();
private:
    QPushButton* toolButton;
    static const int MAX_SUB_CHANNELS = 3;
    int index;
private slots:
    void on_toolButtonClicked();
    void onAddSubChannelClicked();
    void on_toolButtonActionHovered(QAction *action);
    void on_toolButtonActionTriggered(QAction *action);

};


#endif // LOCALTRACKGROUPVIEW_H
