//
// Created by nickolay on 27.09.2020.
//

#include <graph.h>
#include <QtCore/QSharedPointer>
#include "ServerNode.h"
#include <unistd.h>
#include <iostream>
#include <thread>


ServerNode::ServerNode(int _serverNum,int _debugSocketAdress, Graph g)
{
    graph = Graph(g);
    serverNum= _serverNum;
    debugSocketAdress = _debugSocketAdress;
    connections.clear();
}

void ServerNode::Start()       //on start we connect to debug server
{
    std::thread thr([this]() {
        addDebugConnection();
        std::cout<<"Node "<<serverNum<<": awaiting for debug server "<<std::endl;
        while (!debugConnection->connected.get())
        {
            usleep(400);
        }
        std::cout<<"Node "<<serverNum<<": debugServer active now "<<std::endl;
        for (int i=0;i<3;i++){
        TestMessage testMessage;
        for (int i =0;i<200;i++){
            testMessage.text[i] = ' ';
        }
        std::string s = "Hello to debug server from " +std::to_string(serverNum);
        //const char* text = s.c_str();
        if (s.length() < 200)                        //200 - m.text.size
        {
            for (int i=0;i<s.length();i++) {
                testMessage.text[i] = s[i];
            }
        }
        std::cout<<"test text "<<testMessage.text<<std::endl;
        debugConnection->sendMessage(testMessage);
    }});
    thr.detach();
}

void ServerNode::addDebugConnection()
{
    //auto* _debugConnection = new ServerConnection(debugSocketAdress, serverNum,-1);
    //debugConnection = QSharedPointer<ServerConnection>(_debugConnection);
    //ServerConnection debugConnection(debugSocketAdress + serverNum, serverNum,-1);
    debugConnection = new ServerConnection(debugSocketAdress + serverNum, serverNum,-1);
    std::cout<<"port to recive = "<<debugSocketAdress<<" + "<<serverNum<<std::endl;

    //connections.push_back(debugConnection);
    debugConnection->connectTo();
}

void ServerNode::addConnection(int to)
{
    int portDelta = serverNum>to?serverNum:to;
    int port = debugSocketAdress + portDelta;
    //std::cout<<"debugSocketAdress = "<<debugSocketAdress<<std::endl;
    std::cout<<"trying to add connection to "<<to<<" port - "<<port<<std::endl;
    ServerConnection* newConnection = new ServerConnection(port, serverNum, to);

    connections.push_back(newConnection);
    if (serverNum > to)
    {
        newConnection->connectTo();
     }
    else{
        newConnection->awaitConnection();
    }
}

