#include "ThemeLoader.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QFontDatabase>
#include <QRegularExpression>

using theme::Loader;

QStringList Loader::getAvailableThemes(QString themesDir)
{
    QDir baseDir(themesDir);
    if (!baseDir.exists()) {
        qCritical() << "themesDir directory not exists! (" << baseDir.absolutePath() << ")";
        return QStringList();
    }

    return baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

bool Loader::canLoad(const QString &themesDir, const QString &themeName)
{
    QDir baseDir(themesDir);
    if (!baseDir.exists()) {
        qCritical() << "Themes base directory not exists! (" << baseDir.absolutePath() << ")";
        return false;
    }

    QDir themeDir(baseDir.absoluteFilePath(themeName));
    if (!themeDir.exists()) {
        qCritical() << "Theme directory not exists! (" << themeDir.absolutePath() << ")";
        return false;
    }

    if (themeDir.entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty()) // theme directory can't be empty
        return false;

    return true;
}

QString Loader::loadCSS(QString themeDir, QString themeName)
{
    // first load the common CSS shared by all themes
    QString commonCSSDir(":/css/");
    QString commonCSSName("common");
    QString commonCss = Loader::loadThemeCSSFiles(commonCSSDir, commonCSSName);

    // load the theme and merge with common CSS
    if (!canLoad(themeDir, themeName))
        return ""; // can't load the theme CSS

    QString themeCss = Loader::loadThemeCSSFiles(themeDir, themeName);
    if (themeCss.isEmpty())
        return ""; // can't load the theme CSS


    resolveRelativeImagePaths(themeCss, QDir(themeDir).absoluteFilePath(themeName));

    loadFonts(themeDir, themeName);

    return commonCss + themeCss;
}

void Loader::loadFonts(QString themesDir, const QString &themeName)
{
    QDir themeDir(QDir(themesDir).absoluteFilePath(themeName));
    if (!themeDir.exists()) {
        qCritical() << "Theme dir not exists! " << themeDir.absolutePath();
        return;
    }

    QDir fontsDir(themeDir.absoluteFilePath("fonts"));
    if (!fontsDir.exists()) {
        return;
    }

    auto fonts = fontsDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    for(auto fontFile : fonts) {
        int fontID = QFontDatabase::addApplicationFont(fontsDir.absoluteFilePath(fontFile));
        if (fontID < 0)
            qCritical() << "Can't load the font" << fontFile;
    }
}

void Loader::resolveRelativeImagePaths(QString &styleSheet, const QString &imagesPath)
{
    QRegularExpression regex("(url[ ]?\\(['\"]?)([^:^'].+)(\\))");

    styleSheet.replace(regex, "\\1" + imagesPath + "\\2\\3");
}

QString Loader::loadThemeCSSFiles(QString themesBaseDir, const QString &themeName)
{
    QDir baseDir(themesBaseDir);
    if (!baseDir.exists()) {
        qCritical() << "ThemesBaseDir directory not exists! (" << baseDir.absolutePath() << ")";
        return "";
    }

    QDir themeDir(baseDir.absoluteFilePath(themeName));
    if (!themeDir.exists()) {
        qCritical() << "Theme directory not exists! (" << themeDir << ")";
        return "";
    }

    QString themeCSS = "";
    QStringList themeSectionNames = getThemeSectionNames();
    foreach (const QString &sectionName, themeSectionNames) {
        QFile sectionFile(themeDir.absoluteFilePath(sectionName));
        if (sectionFile.exists()) {
            if (sectionFile.open(QFile::ReadOnly))
                themeCSS += sectionFile.readAll();
            else
                qCritical() << "Can't open " << QFileInfo(sectionFile).absoluteFilePath() << sectionFile.errorString();
        }
        else {
            qCritical() << QFileInfo(sectionFile).absoluteFilePath() << " not exists! " << sectionFile.errorString();
        }
    }

    return themeCSS;
}


QStringList Loader::getThemeSectionNames()
{
    QStringList themeSections;
    themeSections << "General.css"
                  << "MainWindow.css"
                  << "Dialogs.css"
                  << "PublicRooms.css"
                  << "BaseTrack.css"
                  << "LocalTrack.css"
                  << "NinjamWindow.css"
                  << "Chat.css"
                  << "Chords.css";

    return themeSections;
}
