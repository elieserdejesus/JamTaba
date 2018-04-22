#ifndef PLUGINDESCRIPTOR_H
#define PLUGINDESCRIPTOR_H

#include <QString>

namespace audio {

class PluginDescriptor
{

public:

    enum Category {
        Native_Plugin,
        VST_Plugin,
        AU_Plugin,
        Invalid_Plugin
    };

    PluginDescriptor(const QString &name, Category category, const QString &manufacturer, const QString &path = "");

    PluginDescriptor();

    virtual ~PluginDescriptor();

    QString getName() const;

    Category getCategory() const;

    QString getPath() const;

    QString getManufacturer() const;

    bool isValid() const;

    bool isVST() const;

    bool isAU() const;

    bool isNative() const;

    static QString getVstPluginNameFromPath(const QString &path);

    QString toString() const;

    static PluginDescriptor fromString(const QString &);

    static QString categoryToString(PluginDescriptor::Category category);

private:
    QString name;
    Category category;
    QString path;
    QString manufacturer;

    static PluginDescriptor::Category stringToCategory(const QString &string);

};

inline QString PluginDescriptor::getManufacturer() const
{
    return manufacturer;
}

inline QString PluginDescriptor::getName() const
{
    return name;
}

inline PluginDescriptor::Category PluginDescriptor::getCategory() const
{
    return category;
}

inline QString PluginDescriptor::getPath() const
{
    return path;
}

inline bool PluginDescriptor::isValid() const
{
    return !name.isEmpty() && category != PluginDescriptor::Invalid_Plugin;
}

inline bool PluginDescriptor::isVST() const
{
    return category == Category::VST_Plugin;
}

inline bool PluginDescriptor::isAU() const
{
    return category == Category::AU_Plugin;
}

inline bool PluginDescriptor::isNative() const
{
    return category == Category::Native_Plugin;
}

}

#endif // PLUGINDESCRIPTOR_H
