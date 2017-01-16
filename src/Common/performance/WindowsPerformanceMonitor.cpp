#include "PerformanceMonitor.h"
#include "../log/logging.h"

#include "Windows.h"
#include "psapi.h"
#include "fstream"
using namespace std;

//http://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c
//creates a static variable to convert Bytes to Megabytes
#define MB 1048576

//functions to calculate and retrieve CPU Load information
static float CalculateCPULoad();
static unsigned long long FileTimeToInt64();
float GetCPULoad();

PerformanceMonitor::PerformanceMonitor(){

    //loads the SYSTEMTIME
    SYSTEMTIME sysTime;
    //Retrieves data so that we have a way to Get it to output when using the pointers
    GetSystemTime(&sysTime);
}

//http://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c
static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long _previousTotalTicks = 0;
    static unsigned long long _previousIdleTicks = 0;

    unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;


    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

    _previousTotalTicks = totalTicks;
    _previousIdleTicks = idleTicks;
    return ret;
}

static unsigned long long FileTimeToInt64(const FILETIME & ft)
{
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}

//http://stackoverflow.com/questions/9783514/error-cannot-convert-float-to-float
double PerformanceMonitor::getTotalCpuUsage(){

    float percent;
    percent = GetCPULoad();
            //qInfo() << percent * 100;
            return percent * 100;
}

int PerformanceMonitor::getMemmoryUsed(){

    //http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    bool result = GlobalMemoryStatusEx(&memInfo);
    static const int DIVIDER = 1024 * 1024;
    if(result){
        //        qInfo() << "PrivateUsage:" << pmc.PrivateUsage/1024/1024;
        //        qInfo() << "WorkingSetSize:" << pmc.WorkingSetSize/1024/1024;
        //        qInfo() << "------------------------------";
        return (100-((memInfo.ullAvailPageFile/DIVIDER) * 100)/(memInfo.ullTotalPageFile/DIVIDER));
    }
    else{
        qWarning() << "Can't get total memory available! GlobalMemoryStatusEx fail!";
    }
    return 0;
}


/*
//http://hackage.haskell.org/package/criterion-1.1.0.0/src/cbits/time-windows.c

PerformanceMonitor::PerformanceMonitor(){
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
}

*/
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
