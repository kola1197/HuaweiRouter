//
// Created by nickolay on 20.12.2020.
//
#include <sys/stat.h>
#include <fstream>
#include <cstring>
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
    std::string cpuLoadStatus = g->cpuLoadCriticalFrames < 1 ? "OK" : "OVERLOAD ( we've got " + QString::number(g->cpuLoadCriticalFrames).toStdString() + " overload frames: "+QString::number(g->cpuLoadCriticalFrames*100/g->cpuFrames).toStdString()+"% of time )";
    fout<<" CPU load status:       "<<cpuLoadStatus<<"                  \n";
    std::string cpuTempStatus = g->cpuTermCriticalFrames < 1 ? "OK" : "OVEHEAT ( we've got " + QString::number(g->cpuTermCriticalFrames).toStdString() +" overheat frames: "+QString::number(g->cpuTermCriticalFrames*100/g->cpuFrames).toStdString()+"% of time )";
    fout<<" CPU temp status:       "<<cpuTempStatus<<"                  \n";
    fout<<" Average time:     "<<g->averageTime<<"          \n";
    fout<<" Max time:         "<<g->maxTime<<"          \n";
    fout<<" Max edge load:    "<<g->maxLoad<<"%         \n\n";

    for (int k=0;k<g->edges.size();k++){
        Edge* e = &g->edges[k];
        fout<<" Edge from "<<e->from<<"  to "<<e->to<<" max load:"<<e->maxLoadFromTo <<"%\n";
        fout<<" Edge from "<<e->to<<"  to "<<e->from<<" max load:"<<e->maxLoadToFrom <<"%\n";
    }

    fout<<"\n";
    for (int j=0;j<g->packets.size();j++)
    {
        Packet* p = &g->packets[j];
        fout<<" Packet "<<g->packets[j].id<<"  from "<<p->from<<" to "<<p->to<<" delivered for "<<p->timeOnCreation.count()<<" ms"<<"\n";
    }
    fout.close();
    sim::sout<<"Log "<<i<<" generated"<<sim::endl;
}