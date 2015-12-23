#ifndef MARQUEELINEEDIT_H
#define MARQUEELINEEDIT_H

#include <QLineEdit>
#include <QScopedPointer>
#include <QFocusEvent>

class MarqueeLineEdit : public QLineEdit
{
public:
    explicit MarqueeLineEdit(QWidget* parent);
    void updateMarquee();
    void setText(const QString &);
    void setFont(const QFont &);
protected:
    void paintEvent(QPaintEvent *);
    void focusInEvent(QFocusEvent *);
private:
    class MarqueeLineEditHelper;

    QScopedPointer<MarqueeLineEditHelper> helper;

};

#endif // MARQUEELINEEDIT_H
