//
// Created by nickolay on 26.11.2020.
//

#include "AsyncVar.h"

template <typename T> void AsyncVar<T>::set(T i)
{
    mutex.lock();
    targetVar = i;
    mutex.unlock();
}

template <typename T> T AsyncVar<T>::get()
{
    T result;
    mutex.lock();
    result = targetVar;
    mutex.unlock();
    return result;
}
template <typename T> AsyncVar<T>::AsyncVar(T t)
{
    targetVar = t;
}

template <typename T> AsyncVar<T>::AsyncVar()
{
    targetVar = T();
}