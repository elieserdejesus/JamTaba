#ifndef FXPANELITEM_H
#define FXPANELITEM_H

#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QWidget>
#include <QPushButton>

class PluginGui;

class FxPanelItem : public QLabel
{

    Q_OBJECT

public:
    FxPanelItem(QWidget* parent, QMenu* fxMenu);
    ~FxPanelItem();
    void setPlugin(PluginGui* pluginGui);
    void unsetPlugin();
    inline bool containPlugin() const{ return pluginGui; }
    bool pluginIsBypassed();
    //void setText(QString str){}
    Q_PROPERTY(bool pluginBypassed READ pluginIsBypassed())
signals:
    void editingPlugin(PluginGui*pluginGui);
    void pluginRemoved(PluginGui* pluginGui);
protected:
    void mousePressEvent ( QMouseEvent * event ) ;
    void paintEvent(QPaintEvent* );
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private slots:
    void on_contextMenu(QPoint p);
    void on_buttonClicked();
    void on_actionMenuTriggered(QAction* a);
private:
    QMenu* fxMenu;//used to list plugins
    QMenu* actionsMenu;//used to list actions(bypass, remove)
    PluginGui* pluginGui;
    QPushButton* button;

    QMenu* createActionsMenu();

    static const QString NEW_EFFECT_STRING;
};

#endif // FXPANELITEM_H
