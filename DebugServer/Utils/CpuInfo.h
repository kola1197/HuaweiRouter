//
// Created by nickolay on 19.12.2020.
// Class for getting cpu temperature and cores load
//

#ifndef HUAWEIROUTER_CPUINFO_H
#define HUAWEIROUTER_CPUINFO_H

#include <string>

const int NUM_CPU_STATES = 10;

enum CPUStates
{
    S_USER = 0,
    S_NICE,
    S_SYSTEM,
    S_IDLE,
    S_IOWAIT,
    S_IRQ,
    S_SOFTIRQ,
    S_STEAL,
    S_GUEST,
    S_GUEST_NICE
};

typedef struct CPUData
{
    std::string cpu;
    size_t times[NUM_CPU_STATES];
} CPUData;

class CpuInfo {
public:
    static std::vector<float> getCPULoad();
    static float getCPUTemp();
    static std::string getCPUName();
private:
    static void ReadStatsCPU(std::vector<CPUData> & entries);
    static size_t GetIdleTime(const CPUData & e);
    static size_t GetActiveTime(const CPUData & e);
    static void PrintStats(const std::vector<CPUData> & entries1, const std::vector<CPUData> & entries2);
};


#endif //HUAWEIROUTER_CPUINFO_H
