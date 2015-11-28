#ifndef PLUGINDESCRIPTOR_H
#define PLUGINDESCRIPTOR_H

#include <QString>

namespace Audio {



class PluginDescriptor {

private:
    QString name;
    QString group;
    QString path;
public:
    PluginDescriptor(QString name, QString group);
    PluginDescriptor(QString name, QString group, QString path);
    PluginDescriptor();
    //PluginDescriptor(const PluginDescriptor&);
    virtual ~PluginDescriptor();
    inline QString getName() const{return name;}
    inline QString getGroup() const{return group;}
    inline QString getPath() const{return path;}
    inline bool isValid() const{ return !name.isEmpty() && !group.isEmpty();}

    inline bool isVST() const{return group.toLower() == "vst";}
    inline bool isNative() const{return group.toLower() == "jamtaba";}

    static QString getPluginNameFromPath(QString path);

    QString toString() const;
    static PluginDescriptor fromString(QString);
};

}

#endif // PLUGINDESCRIPTOR_H
