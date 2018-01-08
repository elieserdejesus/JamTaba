#ifndef _ICON_FACTORY_
#define _ICON_FACTORY_

#include <QIcon>
#include <QColor>

class IconFactory {

public:
    static QIcon createChatEmojiIcon(const QColor &tintColor);
    static QIcon createLooperButtonIcon(const QColor &tintColor);
    static QIcon createStereoInversionIcon(const QColor &tintColor);
    static QIcon createReceiveIcon(const QColor &tintColor);
    static QList<QImage> createLowCutIcons(const QColor &tintColor);
    static QPixmap createLowLevelIcon(const QColor &tintColor);
    static QPixmap createHighLevelIcon(const QColor &tintColor);
    static QIcon createLocalControlsIcon(const QColor &tintColor);
    static QIcon createWebcamIcon(const QColor &tintColor);
    static QIcon createLooperRecordIcon(const QColor &tintColor);
    static QIcon createLooperPlayIcon(const QColor &tintColor);
    static QIcon createLooperSaveIcon(const QColor &tintColor);
    static QIcon createLooperLoadIcon(const QColor &tintColor);
    static QIcon createLooperResetIcon(const QColor &tintColor);

private:
    static void tintImage(QImage &image, const QColor &tint, bool maximizeAlpha = true);

    static int findMaxAlphaValue(const QImage &image);

};

#endif
