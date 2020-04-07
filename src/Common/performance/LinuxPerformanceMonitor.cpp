#include "PerformanceMonitor.h"

#include "sys/sysinfo.h"

PerformanceMonitor::PerformanceMonitor(){

}

PerformanceMonitor::~PerformanceMonitor(){

}

int PerformanceMonitor::getMemmoryUsed(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long virtualMemUsed = memInfo.totalram + memInfo.totalswap + memInfo.totalhigh - memInfo.freeram - memInfo.freeswap - memInfo.freehigh;
    return 100 * virtualMemUsed / (memInfo.totalram + memInfo.freeswap + memInfo.freehigh);
}

int PerformanceMonitor::getBatteryUsed()
{

    return 0;
}
