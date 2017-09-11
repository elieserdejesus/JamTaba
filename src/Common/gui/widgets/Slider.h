#ifndef _JTBA_SLIDER_
#define _JTBA_SLIDER_

#include <QSlider>

class Slider : public QSlider
{
    Q_OBJECT

public:
    Slider(QWidget *parent = nullptr);

    enum SliderType
    {
        AudioSlider,
        PanSlider
    };

    void setSliderType(SliderType type);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void mouseDoubleClickEvent(QMouseEvent *ev) override;

private slots:
    void showToolTip();
    void updateToolTipValue();

private:

    void drawMarker(QPainter &painter);

    qreal getMarkerPosition() const;

    SliderType sliderType;

};

#endif
