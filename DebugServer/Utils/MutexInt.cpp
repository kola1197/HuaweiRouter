//
// Created by nickolay on 26.11.2020.
//

#include "MutexInt.h"
void MutexInt::set(int i)
{
    mutex.lock();
    targetInt = i;
    mutex.unlock();
}

int MutexInt::get()
{
    int result;
    mutex.lock();
    result = targetInt;
    mutex.unlock();
    return result;
}
MutexInt::MutexInt(int i)
{
    targetInt = i;
}

MutexInt::MutexInt()
{
    targetInt = false;
}