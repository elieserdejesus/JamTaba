#include "PluginDescriptor.h"

#include <QStringList>
#include <QFile>

using namespace Audio;

PluginDescriptor::PluginDescriptor()
    : name(""),
      category(PluginDescriptor::Invalid_Plugin),
      manufacturer(""),
      path("")
{

}

PluginDescriptor::PluginDescriptor(const QString &name, Category category, const QString &manufacturer, const QString &path)
    : name(name),
      category(category),
      manufacturer(manufacturer),
      path(path)
{

}

PluginDescriptor::~PluginDescriptor()
{
    //qDebug() << "Plugin descriptor!";
}

QString PluginDescriptor::toString() const
{
    return name + ";" + PluginDescriptor::categoryToString(category) + ";" + path + ";" + manufacturer;
}

PluginDescriptor PluginDescriptor::fromString(const QString &str)
{
    QStringList parts = str.split(";");
    if (parts.size() != 4)
        return PluginDescriptor(); // returning invalid descriptor

    QString name = parts.at(0);

    PluginDescriptor::Category category = stringToCategory(parts.at(1));

    QString path = parts.at(2);

    QString manufacturer = parts.at(3);

    return PluginDescriptor(name, category, manufacturer, path);
}

QString PluginDescriptor::categoryToString(PluginDescriptor::Category category)
{
    switch (category)
    {
        case Category::AU_Plugin:       return "AU";

        case Category::VST_Plugin:      return "VST";

        case Category::Native_Plugin:   return "NATIVE";

        default:                        return "INVALID";
    }
}

PluginDescriptor::Category PluginDescriptor::stringToCategory(const QString &string)
{
    if (string == "AU")
        return Category::AU_Plugin;

    if (string == "VST")
        return Category::VST_Plugin;

    if (string == "NATIVE")
        return Category::Native_Plugin;

    return Category::Invalid_Plugin;
}

QString PluginDescriptor::getVstPluginNameFromPath(const QString &path)
{
    QString name = QFile(path).fileName();
    int indexOfDirSeparator = name.lastIndexOf("/");
    if(indexOfDirSeparator >= 0){
        name = name.right(name.length() - indexOfDirSeparator - 1);
    }
    int indexOfPoint = name.lastIndexOf(".");
    if(indexOfPoint > 0){
        name = name.left(indexOfPoint);
    }
    return name;
}
