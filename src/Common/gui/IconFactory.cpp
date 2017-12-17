#include "IconFactory.h"

#include <QBitmap>
#include <QImage>
#include <QPushButton>

QIcon IconFactory::createLooperResetIcon(const QColor &tintColor)
{
    QImage image(":/images/delete_layer.png");

    IconFactory::tintImage(image, tintColor, true);

    return QIcon(QPixmap::fromImage(image));
}

QIcon IconFactory::createLooperSaveIcon(const QColor &tintColor)
{
    QImage image(":/images/save.png");

    IconFactory::tintImage(image, tintColor, true);

    return QIcon(QPixmap::fromImage(image));
}

QIcon IconFactory::createLooperLoadIcon(const QColor &tintColor)
{
    QImage image(":/images/load.png");

    IconFactory::tintImage(image, tintColor, true);

    return QIcon(QPixmap::fromImage(image));
}

QIcon IconFactory::createLooperRecordIcon(const QColor &tintColor)
{
    QImage image(":/images/rec.png");

    QIcon icon;

    icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::On);

    IconFactory::tintImage(image, tintColor, true);

    icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::Off);

    return icon;
}

QIcon IconFactory::createLooperPlayIcon(const QColor &tintColor)
{
    QImage image(":/images/play.png");

    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::On);

    IconFactory::tintImage(image, tintColor, true);

    icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::Off);

    return icon;
}

QIcon IconFactory::createWebcamIcon(const QColor &tintColor)
{
    QImage webcamImage(":/images/webcam.png");

    IconFactory::tintImage(webcamImage, tintColor, false);

    return QIcon(QPixmap::fromImage(webcamImage));
}

QIcon IconFactory::createLocalControlsIcon(const QColor &tintColor)
{
    QImage left(":/images/mini arrow left.png");
    QImage right(":/images/mini arrow right.png"); // checked

    IconFactory::tintImage(left, tintColor, false);
    IconFactory::tintImage(right, tintColor, false);

    QIcon icon;

    icon.addPixmap(QPixmap::fromImage(left), QIcon::Normal, QIcon::Off);
    icon.addPixmap(QPixmap::fromImage(right), QIcon::Active, QIcon::On);

    return icon;
}

QPixmap IconFactory::createHighLevelIcon(const QColor &tintColor)
{
    QImage image(":/images/level high.png");
    IconFactory::tintImage(image, tintColor, false);
    return QPixmap::fromImage(image);
}

QPixmap IconFactory::createLowLevelIcon(const QColor &tintColor)
{
    QImage image(":/images/level low.png");
    IconFactory::tintImage(image, tintColor, false);
    return QPixmap::fromImage(image);
}

QIcon IconFactory::createReceiveIcon(const QColor &tintColor)
{
    QImage image(":/images/receive.png");

    IconFactory::tintImage(image, tintColor);

    QIcon icon;

    icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::On);
    icon.addPixmap(QPixmap::fromImage(image), QIcon::Disabled, QIcon::On);

    return icon;
}

QList<QImage> IconFactory::createLowCutIcons(const QColor &tintColor)
{
    QImage off(":/images/low_cut_off.png");
    QImage normal(":/images/low_cut_normal.png");
    QImage drastic(":/images/low_cut_drastic.png");

    IconFactory::tintImage(off, tintColor);
    IconFactory::tintImage(normal, tintColor);
    IconFactory::tintImage(drastic, tintColor);

    QList<QImage> icons;

    icons << off;
    icons << normal;
    icons << drastic;

    return icons;
}

QIcon IconFactory::createLooperButtonIcon(const QColor &tintColor)
{
    QImage normal(":/images/loop.png");
    QImage disabled(":/images/loop.png");
    IconFactory::tintImage(normal, tintColor);
    IconFactory::tintImage(disabled, tintColor, false);

    QIcon icon;

    icon.addPixmap(QPixmap::fromImage(normal), QIcon::Normal, QIcon::On);
    icon.addPixmap(QPixmap::fromImage(disabled), QIcon::Disabled, QIcon::On);

    return icon;
}


QIcon IconFactory::createStereoInversionIcon(const QColor &tintColor)
{
    auto normalImage = QImage(":/images/stereo_normal.png");
    auto invertedImage = QImage(":/images/stereo_inverted.png");
    auto disabledNormalImage = QImage(":/images/stereo_normal.png");
    auto disabledInvertedImage = QImage(":/images/stereo_inverted.png");

    IconFactory::tintImage(normalImage, tintColor);
    IconFactory::tintImage(invertedImage, tintColor);
    IconFactory::tintImage(disabledNormalImage, tintColor, false);
    IconFactory::tintImage(disabledInvertedImage, tintColor, false);

    QIcon icon;

    icon.addPixmap(QPixmap::fromImage(normalImage), QIcon::Normal, QIcon::Off);
    icon.addPixmap(QPixmap::fromImage(invertedImage), QIcon::Normal, QIcon::On);

    icon.addPixmap(QPixmap::fromImage(disabledNormalImage), QIcon::Disabled, QIcon::Off);
    icon.addPixmap(QPixmap::fromImage(disabledInvertedImage), QIcon::Disabled, QIcon::On);

    return icon;
}


void IconFactory::tintImage(QImage &image, const QColor &tint, bool maximizeAlpha)
{
    float alphaMultiplier = 255.0f/findMaxAlphaValue(image);
    if (!maximizeAlpha)
        alphaMultiplier = 1.0f;

    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            int alpha = qAlpha(image.pixel(i, j));
            if (alpha > 0) {
                image.setPixel(i, j, qRgba(tint.red(), tint.green(), tint.blue(), qMin(255, static_cast<int>(alpha * alphaMultiplier))));
            }
        }
    }
}

int IconFactory::findMaxAlphaValue(const QImage &image)
{
    int w = image.width();
    int h = image.height();

    int maxAlpha = 1;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            int alpha = qAlpha(image.pixel(i, j));
            if (alpha > 0 && alpha > maxAlpha) {
                maxAlpha = alpha;
            }
        }
    }

    return maxAlpha;
}
