#include "ResourcesUsage.h"
#include "../log/logging.h"
#include <QDateTime>

#ifdef _WIN32
    #include "Windows.h"
    #include "psapi.h"
#endif


const long ResourcesUsageReader::TIME_BETWEEN_UPDATES = 1000;

class WindowsCpuUsageReader : public CpuUsageReader{
public:
    WindowsCpuUsageReader(){
        init();
    }

    double readCpuUsage(){
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;


        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(thisProcessHande, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = (sys.QuadPart - lastSysCPU.QuadPart) +
            (user.QuadPart - lastUserCPU.QuadPart);
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;

        return percent * 100;
        //return sys.
    }
private:
    void init(){
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;


        //GetSystemInfo(&sysInfo);
        GetNativeSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;
        //sysInfo.


        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));


        thisProcessHande = GetCurrentProcess();
        GetProcessTimes(thisProcessHande, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
    }

    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    int numProcessors;
    HANDLE thisProcessHande;
};
//+++++++++++++++++++++++++++++

ResourcesUsage::ResourcesUsage(int memoryUsage, int cpuUsage)
    :memoryUsage(memoryUsage), cpuUsage(cpuUsage){

}

ResourcesUsageReader::ResourcesUsageReader()
    :cpuUsageReader(new WindowsCpuUsageReader() ), lastUpdate(0),
      lastReadedResourceUsage(0,0){

}

ResourcesUsageReader::~ResourcesUsageReader(){
    delete cpuUsageReader;
}

//http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
ResourcesUsage ResourcesUsageReader::getResourcesUsage(){
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if(now - lastUpdate < TIME_BETWEEN_UPDATES){
        return lastReadedResourceUsage;
    }
    lastUpdate = QDateTime::currentMSecsSinceEpoch();
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    bool result = GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
    if(result){
        lastReadedResourceUsage.setMemoryUsage(pmc.WorkingSetSize/1024/1024);
    }
    else{
        qWarning() << "Can't get memory usage! GetProcessMemoryInfo fail!";
    }

    lastReadedResourceUsage.setCpuUsage( (int)cpuUsageReader->readCpuUsage() );
#endif
    return lastReadedResourceUsage;
}
