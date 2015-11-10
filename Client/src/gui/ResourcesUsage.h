#ifndef RESOURCESUSAGE_H
#define RESOURCESUSAGE_H

#include <qglobal.h>

class ResourcesUsage
{
public:
    ResourcesUsage(int memoryUsage, int cpuUsage);
    inline int getMemoryUsage() const{return memoryUsage;}
    inline int getCpuUsage() const{return cpuUsage;}
    inline void setMemoryUsage(int memoryUsage){this->memoryUsage = memoryUsage;}
    inline void setCpuUsage(int cpuUsage){this->cpuUsage = cpuUsage;}
private:
    int memoryUsage;
    int cpuUsage;
};

class CpuUsageReader{
public:
    virtual double readCpuUsage() = 0;
};

class ResourcesUsageReader{
public:
    explicit ResourcesUsageReader();
    virtual ~ResourcesUsageReader();
    ResourcesUsage getResourcesUsage();
private:
    CpuUsageReader* cpuUsageReader;
    static const long TIME_BETWEEN_UPDATES;
    qint64 lastUpdate;
    ResourcesUsage lastReadedResourceUsage;
};

#endif // RESOURCESUSAGE_H
