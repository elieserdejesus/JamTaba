#include "PluginDescriptor.h"

#include <QStringList>
#include <QFile>

using namespace Audio;

PluginDescriptor::PluginDescriptor()
    :name(""), group(""), path("")
{

}

PluginDescriptor::PluginDescriptor(QString name, QString group)
    :name(name), group(group), path("")
{

}

PluginDescriptor::PluginDescriptor(QString name, QString group, QString path)
    :name(name), group(group), path(path)
{

}

PluginDescriptor::~PluginDescriptor(){
    //qDebug() << "Plugin descriptor!";
}

QString PluginDescriptor::toString() const{
    return name + ";" + group + ";" + path;
}

PluginDescriptor PluginDescriptor::fromString(QString str){
    QStringList parts = str.split(";");
    return PluginDescriptor(parts.at(0), parts.at(1), parts.at(2));
}

QString PluginDescriptor::getPluginNameFromPath(QString path){
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
