//
// Created by nickolay on 12.12.2020.
//

#ifndef HUAWEIROUTER_SOUT_H
#define HUAWEIROUTER_SOUT_H

#include <iostream>
#include <mutex>
#include <thread>
#include <QString>
#include <sstream>
#include "string.h"
#include "AsyncVar.h"

class Sout
{
private:
    static std::mutex mut;
public:
    Sout()
    {
    }

    Sout(std::string endl) // the ofstream needs a path
    {
        endL = endl;
    }
    std::string endL;
    std::thread::id threadNum;

    template <typename T>
    Sout& operator<<(T&& t) // provide a generic operator<<
    {
        if ( threadNum != std::this_thread::get_id() )
        {
            Sout::mut.lock();
            threadNum = std::this_thread::get_id();
        }
        bool endOfLine;
        std::string name ( typeid(T).name());
            std::stringstream ss;
            ss<<t;
            std::string sData = ss.str();
            endOfLine = sData == endL;
        if (!endOfLine)
            //if (strcmp(hData, ttData) != 0)
        {
            std::cout<< t;
            return *this;
        }
        else {
            std::cout<< t ;
            threadNum = std::thread::id();
            mut.unlock();
            return *this;
        }
    }
};

class sim {
public:
    static std::string endl;
    static Sout sout;
};


#endif //HUAWEIROUTER_SOUT_H
