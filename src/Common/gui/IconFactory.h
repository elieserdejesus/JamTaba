#ifndef _ICON_FACTORY_
#define _ICON_FACTORY_

#include <QIcon>
#include <QColor>

class IconFactory {

public:
    static QPixmap createFontSizePixmap(const QColor &tintColor);
    static QIcon createFontSizeIncreaseIcon(const QColor &tintColor);
    static QIcon createFontSizeDecreaseIcon(const QColor &tintColor);

    static QPixmap createReceivePixmap(const QColor &tintColor);
    static QPixmap createTransmitPixmap(const QColor &tintColor);
    static QPixmap createMidiIcon(const QColor &tintColor);
    static QPixmap createNoInputIcon(const QColor &tintColor);
    static QPixmap createMonoInputIcon(const QColor &tintColor);
    static QPixmap createStereoInputIcon(const QColor &tintColor);
    static QIcon createChatEmojiIcon(const QColor &tintColor, bool happyFace);
    static QIcon createChatOnOffIcon(const QColor &tintColor);
    static QIcon createLooperButtonIcon(const QColor &tintColor);
    static QIcon createStereoInversionIcon(const QColor &tintColor);
    static QIcon createReceiveIcon(const QColor &tintColor);
    static QList<QImage> createLowCutIcons(const QColor &tintColor);
    static QIcon createLocalControlsIcon(const QColor &tintColor);
    static QIcon createWebcamIcon(const QColor &tintColor);
    static QIcon createLooperRecordIcon(const QColor &tintColor);
    static QIcon createLooperPlayIcon(const QColor &tintColor);
    static QIcon createLooperSaveIcon(const QColor &tintColor);
    static QIcon createLooperLoadIcon(const QColor &tintColor);
    static QIcon createLooperResetIcon(const QColor &tintColor);

    static QIcon getDefaultInstrumentIcon();
    static QList<QIcon> getInstrumentIcons();

private:
    static void tintImage(QImage &image, const QColor &tint, bool maximizeAlpha = true);

    static int findMaxAlphaValue(const QImage &image);

};

#endif
