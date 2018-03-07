#ifndef _JTBA_SLIDER_
#define _JTBA_SLIDER_

#include <QSlider>

class AudioSlider : public QSlider
{
    Q_OBJECT

public:
    explicit AudioSlider(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mouseDoubleClickEvent(QMouseEvent *ev) override;

private slots:
    void showToolTip();
    void updateToolTipValue();

private:
    void drawMarker(QPainter &painter);
    qreal getMarkerPosition() const;

};

class PanSlider : public QSlider
{
    Q_OBJECT

public:
    explicit PanSlider(QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *ev) override;

private slots:
    void updateToolTipValue();

private:
    void drawMarker(QPainter &painter);
    void showToolTip();
};

#endif
