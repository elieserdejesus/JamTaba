#ifndef PLUGINDESCRIPTOR_H
#define PLUGINDESCRIPTOR_H

#include <QString>

namespace Audio {
class PluginDescriptor
{
private:
    QString name;
    QString group;
    QString path;
public:
    PluginDescriptor(const QString &name, const QString &group, const QString &path = "");
    PluginDescriptor();
    virtual ~PluginDescriptor();
    inline QString getName() const
    {
        return name;
    }

    inline QString getGroup() const
    {
        return group;
    }

    inline QString getPath() const
    {
        return path;
    }

    inline bool isValid() const
    {
        return !name.isEmpty() && !group.isEmpty();
    }

    inline bool isVST() const
    {
        return group.toLower() == "vst";
    }

    inline bool isNative() const
    {
        return group.toLower() == "jamtaba";
    }

    static QString getPluginNameFromPath(const QString &path);

    QString toString() const;
    static PluginDescriptor fromString(const QString &);
};
}

#endif // PLUGINDESCRIPTOR_H
