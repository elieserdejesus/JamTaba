#ifndef BUTTONBLINKER_H
#define BUTTONBLINKER_H

#include <QPushButton>
#include <QTimer>

class BlinkableButton : public QPushButton
{
    Q_OBJECT

public:
    BlinkableButton(const QString &text);
    BlinkableButton(const QString &text, quint32 blinkTime = 500);
    void startBlink();
    void stopBlink();
    void setBlinkProperty(const QString &propertyName);
    void setBlinkTime(quint32 blinkTime);

private slots:
    void updateBlinkProperty();
    void updateBlinkProperty(bool propertySetted);

private:
    QTimer timer;

};

#endif // BUTTONBLINKER_H
