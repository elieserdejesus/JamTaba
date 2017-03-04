#ifndef MULTI_STATE_BUTTON_H
#define MULTI_STATE_BUTTON_H

#include <QPushButton>
#include <QMap>

class MultiStateButton : public QPushButton
{
    Q_OBJECT

    //custom properties defined in stylesheet files
    Q_PROPERTY(int state MEMBER currentState)

public:
    explicit MultiStateButton(quint8 states, QWidget *parent = 0);
    inline quint8 getCurrentState() const { return currentState; }
    void setText(const QString &text, quint8 state);
    void setState(quint8 newState);

signals:
    void stateChanged();

private:
    const quint8 states;
    quint8 currentState;
    QMap<quint8, QString> texts;

protected:
    void nextCheckState() override;

};

#endif
