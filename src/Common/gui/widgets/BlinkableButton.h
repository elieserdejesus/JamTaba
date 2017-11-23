#ifndef BUTTONBLINKER_H
#define BUTTONBLINKER_H

#include <QPushButton>
#include <QTimer>

class BlinkableButton : public QPushButton
{
    Q_OBJECT

public:
    explicit BlinkableButton(QWidget *parent = nullptr);
    BlinkableButton(const QString &text, QWidget *parent = nullptr);
    ~BlinkableButton();
    void startBlink();
    void stopBlink();
    bool isBlinking() const;

    static void setBlinkTime(quint32 blinkTime);
    static void updateAllBlinkableButtons(); // called periodically from MainWindow timer

private:
    bool blinking;

    void setBlinkProperty(bool propertySetted);

    static QList<BlinkableButton *> instances;
    static quint64 lastBlinkToggle;
    static quint32 blinkTime;
    static bool blinkPropertySetted;
};

inline bool BlinkableButton::isBlinking() const
{
    return blinking;
}

#endif // BUTTONBLINKER_H
