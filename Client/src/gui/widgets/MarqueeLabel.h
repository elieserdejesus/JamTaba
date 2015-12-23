#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QLabel>
#include <QScopedPointer>

class MarqueeHelper;

class QPaintEvent;

class MarqueeLabel : public QLabel
{
public:
    explicit MarqueeLabel(QWidget* parent=0);
    void updateMarquee();//called by a external timer to run the marquee animation
    void setText(const QString &);
    void setFont(const QFont &);
protected:
    void paintEvent(QPaintEvent *evt);
private:

    class MarqueeLabelHelper;

    QScopedPointer<MarqueeHelper> helper;

};

#endif // MARQUEELABEL_H
