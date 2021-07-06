//
// Created by nickolay on 26.11.2020.
// Async access for variables from different threads
//

#ifndef HUAWEIROUTER_MUTEXINT_H
#define HUAWEIROUTER_MUTEXINT_H


#include <mutex>

class MutexInt {
public:
    MutexInt();
    MutexInt(int i);
    int get();
    void set(int i);
private:
    std::mutex mutex;
    int targetInt = 0;
};



#endif //HUAWEIROUTER_MUTEXINT_H
