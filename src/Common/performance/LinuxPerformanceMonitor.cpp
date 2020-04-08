#include "PerformanceMonitor.h"

#include "sys/sysinfo.h"

PerformanceMonitor::PerformanceMonitor(){

}

PerformanceMonitor::~PerformanceMonitor(){

}

int PerformanceMonitor::getMemmoryUsed(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long virtualMemUsed = memInfo.totalram - memInfo.freeram ;
    return 100 * virtualMemUsed / memInfo.totalram;
}

int PerformanceMonitor::getBatteryUsed()
{

    return 0;
}
