#include "ThemeLoader.h"
#include <QFile>
#include <QDir>
#include <QDebug>

using namespace Theme;

QStringList Loader::getAvailableThemes(QString themesDir)
{
    QDir baseDir(themesDir);
    if (!baseDir.exists()) {
        qCritical() << "themesDir directory not exists! (" << baseDir.absolutePath() << ")";
        return QStringList();
    }

    return baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QString Loader::loadCSS(QString themeDir, QString themeName)
{
    //first load the common CSS shared by all themes
    QString commonCSSDir(":/style/");
    QString commonCSSName("common");
    QString css = Loader::loadThemeCSSFiles(commonCSSDir, commonCSSName);

    //load the theme and merge with common CSS
    css += Loader::loadThemeCSSFiles(themeDir, themeName);

    return css;
}

QString Loader::loadThemeCSSFiles(QString themesBaseDir, QString themeName)
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
        if (sectionFile.exists()){
            if(sectionFile.open(QFile::ReadOnly))
                themeCSS += sectionFile.readAll();
            else
                qCritical() << "Can't open " << QFileInfo(sectionFile).absoluteFilePath() << sectionFile.errorString();
        }
        else{
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
