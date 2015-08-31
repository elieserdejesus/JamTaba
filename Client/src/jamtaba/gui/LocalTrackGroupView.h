#ifndef LOCALTRACKGROUPVIEW_H
#define LOCALTRACKGROUPVIEW_H

#include "TrackGroupView.h"

class LocalTrackView;

class QPushButton;
class MainFrame;

namespace Ui {
class LocalTrackGroupView;
}


class LocalTrackGroupView : public TrackGroupView
{
    Q_OBJECT
public:
    LocalTrackGroupView(int index, MainFrame* mainFrame);
    ~LocalTrackGroupView();
    void refreshInputSelectionName(int inputTrackIndex);
    void refreshInputSelectionNames();
    QList<LocalTrackView*> getTracks() const;
    virtual void addTrackView(BaseTrackView *trackView);
    inline int getIndex() const{return index;}

    virtual void setFaderOnlyMode(bool faderOnly);
    virtual void toggleFaderOnlyMode();
    bool isFaderOnly() const{return faderOnly;}

    //QSize sizeHint() const;

signals:
    void nameChanged();
private:
    QPushButton* toolButton;
    static const int MAX_SUB_CHANNELS = 2;
    int index;
    MainFrame* mainFrame;
    bool eventFilter(QObject *target, QEvent *event);

    bool faderOnly;

private slots:
    void on_toolButtonClicked();

    void on_addSubChannelClicked();
    void on_addChannelClicked();

    void on_removeSubchannelHovered();
    void on_removeChannelHovered();

    void on_removeSubChannelClicked();
    void on_removeChannelClicked();

};


#endif // LOCALTRACKGROUPVIEW_H
