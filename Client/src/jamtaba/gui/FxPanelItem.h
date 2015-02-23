#ifndef FXPANELITEM_H
#define FXPANELITEM_H

#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QWidget>

class PluginGui;

class FxPanelItem : public QLabel
{

    Q_OBJECT

public:
    FxPanelItem(QWidget* parent, QMenu* fxMenu);
    ~FxPanelItem();
    void setPlugin(PluginGui* pluginGui);
    inline bool containPlugin() const{ return pluginGui; }
signals:
    void editingPlugin(PluginGui*pluginGui);

protected:
    void mousePressEvent ( QMouseEvent * event ) ;
    void paintEvent(QPaintEvent* );
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
private slots:
    void on_contextMenu(QPoint p);
private:
    QMenu* menu;
    PluginGui* pluginGui;

};

#endif // FXPANELITEM_H
