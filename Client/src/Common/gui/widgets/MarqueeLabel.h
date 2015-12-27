#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QLabel>

class MarqueeLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MarqueeLabel(QWidget* parent=0);
    void updateMarquee();//called by a external timer to run the marquee animation. I avoid a internal timer because the Jamtaba MainWindow is running a timer to update the gui elements.
    void setText(const QString &);
    void setFont(const QFont &);
protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *);
private:
    float textX;
    float textY;
    int textLength;
    bool animating;
    float speedDecay;
    QString originalText;

    static const int MARQUEE_SPEED;
    static const int TIME_BETWEEN_ANIMATIONS;

    quint64 lastUpdate;
    quint64 timeSinceLastAnimation;

    void updateTextY();
    void updateTextLenght();
    void reset();
    void resetAnimationProperties();

    bool needAnimation() const;

};

#endif // MARQUEELABEL_H
