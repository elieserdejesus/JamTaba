#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

/**

    This class is implemented in different files for multiplatform purposes.
    The implementation files are WindowsPerformanceMonitor.cpp, MacPerformanceMonitor.cpp
    and in the future LinuxPerformanceMonitor
    The correct implementation file is selected in Jamtaba-common.pri

*/

class PerformanceMonitor
{

public:
    explicit PerformanceMonitor();
    ~PerformanceMonitor();
    //int getMemmoryUsage();
    int getMemmoryUsed();
    //double getCpuUsage();
    //double getTotalCpuUsage();

//private:
//    int processorsCount;

};

#endif // PERFORMANCE_MONITOR_H
