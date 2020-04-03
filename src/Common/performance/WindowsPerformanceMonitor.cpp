#include "PerformanceMonitor.h"
#include "../log/logging.h"

#include "Windows.h"
#include "psapi.h"

//http://hackage.haskell.org/package/criterion-1.1.0.0/src/cbits/time-windows.c

PerformanceMonitor::PerformanceMonitor(){
/*
    HANDLE thisProcessHande = GetCurrentProcess();
    SYSTEM_INFO sysInfo;
    BOOL runningInWow64 = false;
    IsWow64Process(thisProcessHande, &runningInWow64);
    if(runningInWow64){
        GetNativeSystemInfo(&sysInfo);
    }
    else{
        GetSystemInfo(&sysInfo);
    }
    this->processorsCount = sysInfo.dwNumberOfProcessors;
*/
}

PerformanceMonitor::~PerformanceMonitor(){

}

/*
//http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
double PerformanceMonitor::getCpuUsage(){
    static ULARGE_INTEGER lastCPU;
    static ULARGE_INTEGER lastSysCPU;
    static ULARGE_INTEGER lastUserCPU;
    static HANDLE thisProcessHande = GetCurrentProcess();

    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    if(GetProcessTimes(thisProcessHande, &ftime, &ftime, &fsys, &fuser)){
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = (sys.QuadPart - lastSysCPU.QuadPart) +
                (user.QuadPart - lastUserCPU.QuadPart);
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent /= this->processorsCount;
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;
        //qInfo() << "percent:" << percent * 100;
        return percent * 100;
    }
    return 0;
}

int PerformanceMonitor::getMemmoryUsage(){

    //http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    PROCESS_MEMORY_COUNTERS_EX pmc;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
    static const int DIVIDER = 1024 * 1024;
    if(result){
        //        qInfo() << "PrivateUsage:" << pmc.PrivateUsage/1024/1024;
        //        qInfo() << "WorkingSetSize:" << pmc.WorkingSetSize/1024/1024;
        //        qInfo() << "------------------------------";
        return pmc.PrivateUsage/DIVIDER;
    }
    else{
        qWarning() << "Can't get memory usage! GetProcessMemoryInfo fail!";
    }
    return 0;
}
*/

int PerformanceMonitor::getMemmoryUsed(){

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

int PerformanceMonitor::getBatteryUsed(){

    //http://www.cplusplus.com/forum/beginner/72594/
        SYSTEM_POWER_STATUS status; // note not LPSYSTEM_POWER_STATUS
    GetSystemPowerStatus(&status);

    int life = status.BatteryLifePercent; //if value is 255 that means unknown
    //int BTime = status.BatteryLifeTime;

return life;
}
