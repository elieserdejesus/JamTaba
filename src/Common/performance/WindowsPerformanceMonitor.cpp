#include "PerformanceMonitor.h"
#include "../log/logging.h"

#include "Windows.h"
#include "psapi.h"

PerformanceMonitor::PerformanceMonitor()
{

}

PerformanceMonitor::~PerformanceMonitor()
{

}

int PerformanceMonitor::getMemmoryUsed()
{

    //http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    bool result = GlobalMemoryStatusEx(&memInfo);
    if(result){
        return 100 - (memInfo.ullAvailPhys * 100 / memInfo.ullTotalPhys);
    }
    else{
        qWarning() << "Can't get total memory available! GlobalMemoryStatusEx fail!";
    }
    return 0;
}

int PerformanceMonitor::getBatteryUsed()
{

    //http://www.cplusplus.com/forum/beginner/72594/
        SYSTEM_POWER_STATUS status; // note not LPSYSTEM_POWER_STATUS
    GetSystemPowerStatus(&status);

    int life = status.BatteryLifePercent; //if value is 255 that means unknown

return life;
}
