#ifndef FXPANELITEM_H
#define FXPANELITEM_H

#include <QLabel>
#include "LocalTrackView.h"

class QMenu;
class QPushButton;

namespace Audio {
    class Plugin;
}

namespace Controller {
    class MainController;
}

class FxPanelItem : public QLabel
{

    Q_OBJECT

public:
    FxPanelItem(LocalTrackView* parent, Controller::MainController* mainController);
    ~FxPanelItem();
    //void setFxMenu(QMenu* fxMenu);
    void setPlugin(Audio::Plugin* plugin);
    void unsetPlugin();
    inline bool containPlugin() const{ return plugin; }
    bool pluginIsBypassed();
    const Audio::Plugin* getAudioPlugin() const{return plugin;}
    Q_PROPERTY(bool pluginBypassed READ pluginIsBypassed())//to use in stylesheet
//signals:
//    void editingPlugin(Audio::Plugin* plugin);
//    void pluginRemoved(Audio::Plugin* plugin);
protected:
    void mousePressEvent ( QMouseEvent * event ) ;
    void paintEvent(QPaintEvent* );
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private slots:
    void on_contextMenu(QPoint p);
    void on_buttonClicked();
    void on_actionMenuTriggered(QAction* a);
    void on_fxMenuActionTriggered(QAction* a);
private:
    Audio::Plugin* plugin;
    QPushButton* button;
    Controller::MainController* mainController;//used to ask about plugins

    void showPluginGui(Audio::Plugin *plugin);

    static const QString NEW_EFFECT_STRING;

    LocalTrackView* localTrackView;
};

#endif // FXPANELITEM_H
