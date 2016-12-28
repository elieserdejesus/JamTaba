#ifndef PLUGINDESCRIPTOR_H
#define PLUGINDESCRIPTOR_H

#include <QString>

namespace Audio {

class PluginDescriptor
{

public:

    enum Category {
        Native_Plugin,
        VST_Plugin,
        AU_Plugin,
        Invalid_Plugin
    };

    PluginDescriptor(const QString &name, Category category, const QString &path = "");

    PluginDescriptor();

    virtual ~PluginDescriptor();

    inline QString getName() const;

    inline Category getCategory() const;

    inline QString getPath() const;

    inline bool isValid() const;

    inline bool isVST() const;

    inline bool isAU() const;

    inline bool isNative() const;

    static QString getPluginNameFromPath(const QString &path);

    QString toString() const;

    static PluginDescriptor fromString(const QString &);

    static QString categoryToString(PluginDescriptor::Category category);

private:
    QString name;
    Category category;
    QString path;


    static PluginDescriptor::Category stringToCategory(const QString &string);

};

QString PluginDescriptor::getName() const
{
    return name;
}

PluginDescriptor::Category PluginDescriptor::getCategory() const
{
    return category;
}

QString PluginDescriptor::getPath() const
{
    return path;
}

bool PluginDescriptor::isValid() const
{
    return !name.isEmpty() && category != PluginDescriptor::Invalid_Plugin;
}

bool PluginDescriptor::isVST() const
{
    return category == Category::VST_Plugin;
}

bool PluginDescriptor::isAU() const
{
    return category == Category::AU_Plugin;
}

bool PluginDescriptor::isNative() const
{
    return category == Category::Native_Plugin;
}

}

#endif // PLUGINDESCRIPTOR_H
