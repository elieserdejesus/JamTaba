#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QLabel>

class MarqueeLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MarqueeLabel(QWidget* parent=0);
    void updateMarquee();//called by an external timer to run the marquee animation. I avoid a internal timer because the Jamtaba MainWindow is running a timer to update the gui elements.
    void setFont(const QFont &);
    static void setTimeBetweenAnimations(quint32 newTime);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    float textX;
    float textY;
    bool animating;
    float speedDecay;

    static const int MARQUEE_SPEED;
    static quint64 TIME_BETWEEN_ANIMATIONS;

    quint64 lastUpdate;
    quint64 timeSinceLastAnimation;

    void updateTextY();
    int getTextLenght() const;
    void reset();
    void resetAnimationProperties();
    QRectF getSelectedTextRect() const;
    bool needAnimation() const;

};

#endif // MARQUEELABEL_H
