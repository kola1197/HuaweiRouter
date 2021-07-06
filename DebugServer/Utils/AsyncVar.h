//
// Created by nickolay on 26.11.2020.
// Async access for variables from different threads
//

#ifndef HUAWEIROUTER_ASYNCVAR_H
#define HUAWEIROUTER_ASYNCVAR_H


#include <mutex>

template <typename T>
class AsyncVar {
public:
    AsyncVar(){
        targetVar = T();
    };
    AsyncVar(T t){
        targetVar = t;
    };
    T get(){
        T result;
        mutex.lock();
        result = targetVar;
        mutex.unlock();
        return result;
    };
    void set(T i)
    {
        mutex.lock();
        targetVar = i;
        mutex.unlock();
    };
    void increase(T i)
    {
        mutex.lock();
        targetVar += i;
        mutex.unlock();
    }
private:
    std::mutex mutex;
    T targetVar;
};


#endif //HUAWEIROUTER_ASYNCVAR_H
