#ifndef THEME_LOADER_H
#define THEME_LOADER_H

#include <QString>

namespace Theme {

class Loader {

public:
    static QString loadCSS(QString themeDir, QString themeName);

private:
    static QStringList getThemeSectionNames();
    static QString loadThemeCSSFiles(QString themeDir, QString themeName);
};

}//namespace

#endif
