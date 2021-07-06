//
// Created by nickolay on 01.02.2021.
//


#include <graph.h>
#include <QtCore/QSharedPointer>
#include "ServerNode.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <Utils/ColorMode.h>
#include <Utils/Settings.h>
#include "QObject"
#include <cstdlib>
#include <ctime>
#include <Utils/sout.h>
#include "string"

int ServerNode::localVotingSelectionAlgorithm(int prevNodeNum, int to)
{
    //sim::sout<<"localVotingSelectionAlgorithm"<<sim::endl;
    std::vector<std::tuple<float,float,float>> nodesLoad;
    std::vector<int> nodesId;

    float zSum=0;
    float uSum=0;
    float wSum=0;

    for (int i=0;i<connections.size();i++)
    {
        if (!connections[i]->sendingQueue.broken.get()){
            float z = 1/ (connections[i]->bufferLoad.get() + 1);
            zSum += z;
            float x = connections[i]->nodeLoad.get();
            float u = alpha * (- x + nodeLoad);
            u = u > 0 ? u : 0;
            uSum += u;
            float w = 1.0f/(pathLength(connections[i]->to,to) + 1);
            wSum += w;
            std::tuple<float,float,float> t(z,u,w);
            nodesLoad.push_back(t);
            nodesId.push_back(i);
        }
        //nodesLoad.push_back( (a + b + c * c * c * c) * (a + b + c * c * c * c) + 1);
        //sim::sout<<"Local voting:   "<<a<<" "<<b<<" "<<c<<" "<<sim::endl;
    }
    std::vector<float> nodesWeights;

    int isum = 0;
    for (int i=0; i < nodesLoad.size();i++)
    {
        float z = 1000 * std::get<0>(nodesLoad[i]) / zSum;
        float u = uSum == 0 ? 0 : 1000 * std::get<1>(nodesLoad[i]) / uSum;
        float w = 1000 * std::get<2>(nodesLoad[i]) / wSum;

        z *= Settings::getZCoef();
        u *= Settings::getUCoef();
        w *= Settings::getWCoef();

        //w = w*w;

        //sim::sout<<"z:  "<<z<<" u:  "<<u<<" w:  "<<w<<sim::endl;
        float weight = (float)qRound(z + u + w);
        nodesWeights.push_back(weight);
        isum += qRound(weight);
    }
    //srand(time(0));
    int result = -1;
    int counter = 0;
    if (nodesId.size()>1) {
         int nodeToRemove = -1;
         for (int j=0;j<nodesId.size();j++) {                       //removing last packet node
            if (connections[nodesId[j]]->to == prevNodeNum) {
                nodeToRemove = j;
            }
         }
         if (nodeToRemove != -1){
             nodesId.erase(nodesId.begin() + nodeToRemove);
             isum -= nodesWeights[nodeToRemove];
             nodesWeights.erase(nodesWeights.begin() + nodeToRemove);
             nodesLoad.erase(nodesLoad.begin() + nodeToRemove);
         }

        //while (result == prevNodeNum && nodesWeights.size()>1 ) {
        if (!nodesWeights.empty()) {
            //sim::sout<<"here2"<<sim::endl;
            counter ++;
            //if (counter > 100)
            //{
            //    sim::sout<<Color::ColorMode(Color::Code::FG_RED)<<"ERROR! Loop on "<<serverNum<<Color::ColorMode(Color::Code::FG_DEFAULT)<<sim::endl;
            //}
            int a = rand() % isum;
            for (int i = 0; i < nodesWeights.size(); i++) {
                a -= nodesWeights[i];
                if (a < 0) {
                    return nodesId[i];
                    /*if (connections[nodesId[i]]->to != prevNodeNum && !connections[nodesId[i]]->sendingQueue.broken.get()) {
                        result = nodesId[i];
                        sim::sout<<"LV out = "<<nodesId[i]<<sim::endl;
                        return nodesId[i];
                    } else {
                        i = nodesWeights.size();
                    }*/
                }
            }
        }
    }
    else{
        if (nodesId.size() == 1){
            //sim::sout<<"LV out = "<<nodesId[0]<<sim::endl;
            return nodesId[0];
        } else{
            //sim::sout<<"LV out = "<<-1<<sim::endl;
            return -1;
        }
    }
    return -1;
}
