#ifndef FXPANELITEM_H
#define FXPANELITEM_H

#include <QFrame>

class LocalTrackViewStandalone;

class QMenu;
class QPushButton;
class QLabel;

namespace audio {
class Plugin;
}

namespace controller {
class MainControllerStandalone;
}

class FxPanelItem : public QFrame
{
    Q_OBJECT

public:
    FxPanelItem(LocalTrackViewStandalone *parent, controller::MainControllerStandalone *mainController);
    virtual ~FxPanelItem();
    void setPlugin(audio::Plugin *plugin);
    void unsetPlugin();
    inline bool containPlugin() const
    {
        return plugin;
    }

    bool pluginIsBypassed();
    const audio::Plugin *getAudioPlugin() const
    {
        return plugin;
    }

    Q_PROPERTY(bool bypassed READ pluginIsBypassed()) // to use in stylesheet
    Q_PROPERTY(bool containPlugin READ containPlugin()) // to use in stylesheet

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;

private slots:
    void on_contextMenu(QPoint p);
    void on_buttonClicked();
    void on_actionMenuTriggered(QAction *a);
    void loadSelectedPlugin();

private:
    audio::Plugin *plugin;
    QPushButton *bypassButton;
    QLabel *label;
    controller::MainControllerStandalone *mainController; // used to ask about plugins

    void showPluginGui(audio::Plugin *plugin);

    void showPluginsListMenu(const QPoint &p);

    static const QString NEW_EFFECT_STRING;

    LocalTrackViewStandalone *localTrackView;

    void updateStyleSheet();
};

#endif // FXPANELITEM_H
