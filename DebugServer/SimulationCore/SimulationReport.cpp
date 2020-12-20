//
// Created by nickolay on 20.12.2020.
//
#include <sys/stat.h>
#include <fstream>
#include <string>
#include "SimulationReport.h"
#include <unistd.h>
#include <Utils/CpuInfo.h>
#include "Utils/sout.h"

void SimulationReport::generateReport(Graph* g, std::string alg)
{
    sim::sout<<"Generate report"<<sim::endl;
    mkdir("Reports", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    std::ofstream fout;
    std::string fileName = "Reports//Report";
    int i = 1;
    while ( access( std::string (fileName + QString::number(i).toStdString()+".txt").c_str(), F_OK ) != -1 )
    {
        i++;
    }
    fout.open(std::string (fileName + QString::number(i).toStdString()+".txt").c_str(),std::ios_base::out);
    fout<<"----------- Test №"<<i<<" -----------\n\n";
    fout<<"             "<<g->packets.size()<<" Packets                  \n";
    fout<<" Algorithm:        "<<alg<<"   \n";
    fout<<" CPU:              "<<CpuInfo::getCPUName()<<"                  \n";
    std::string cpu = g->cpuCorrect?"OK":"OVERLOAD";
    fout<<" CPU status:       "<<cpu<<"                  \n";
    fout<<" Average time:     "<<g->averageTime<<"          \n\n";
    for (int j=0;j<g->packets.size();j++)
    {
        PacketMessage* p = &g->packets[j];
        fout<<" Packet "<<g->packets[j].id<<"  from "<<p->from<<" to "<<p->to<<" delivered for "<<p->timeOnCreation.count()<<" ms"<<"\n";
    }
    fout.close();
    sim::sout<<"Log "<<i<<" generated"<<sim::endl;
}