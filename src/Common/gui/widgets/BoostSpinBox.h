#ifndef _BOOST_SPIN_BOX_
#define _BOOST_SPIN_BOX_

#include <QToolButton>
#include <QFrame>

class BoostSpinBox : public QFrame
{
    Q_OBJECT

public:
    explicit BoostSpinBox(QWidget *parent);

    void setToMax();
    void setToMin();
    void setToOff();
    void updateToolTip();

    void setOrientation(Qt::Orientation orientation);

    void updateStyleSheet();

signals:
    void boostChanged(int boostValue);

private slots:
    void increaseBoost();
    void decreaseBoost();

private:

    bool canIncrease() const;
    bool canDecrease() const;

    void updateButtons();

    void showToolTip();

    static const int BOOST_STEP = 12;
    static const int MIN_BOOST_VALUE = -BOOST_STEP;
    static const int MAX_BOOST_VALUE = BOOST_STEP;

    int boostValue;

    QToolButton *buttonDecrease;
    QToolButton *buttonIncrease;
};

inline bool BoostSpinBox::canDecrease() const
{
    return boostValue > MIN_BOOST_VALUE;
}

inline bool BoostSpinBox::canIncrease() const
{
    return boostValue < MAX_BOOST_VALUE;
}

#endif
