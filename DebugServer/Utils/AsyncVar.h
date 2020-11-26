//
// Created by nickolay on 26.11.2020.
//

#ifndef HUAWEIROUTER_ASYNCVAR_H
#define HUAWEIROUTER_ASYNCVAR_H


#include <mutex>

template <typename T>
class AsyncVar {
public:
    AsyncVar();
    AsyncVar(T i);
    T get();
    void set(T i);
private:
    std::mutex mutex;
    T targetVar;
};


#endif //HUAWEIROUTER_ASYNCVAR_H
