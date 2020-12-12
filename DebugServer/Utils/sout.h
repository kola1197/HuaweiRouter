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
#include <Utils/sout.h>

#define MUTEX_MODE
#define WRITE_CONSOLE_LOG

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
#ifdef MUTEX_MODE
            Sout::mut.lock();
            threadNum = std::this_thread::get_id();
#endif
        }
        bool endOfLine;
        std::stringstream ss;
        ss<<t;
        std::string sData = ss.str();
        endOfLine = sData == endL;
        std::cout<< t;
        if (endOfLine)
        {
#ifdef WRITE_CONSOLE_LOG
            std::cout<< t ;
#endif
#ifdef MUTEX_MODE
            threadNum = std::thread::id();
            mut.unlock();
#endif
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
