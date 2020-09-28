//
// Created by nickolay on 09.06.2020.
//

#include "MutexBool.h"
void MutexBool::set(bool b)
{
    mutex.lock();
    targetBool = b;
    mutex.unlock();
}

bool MutexBool::get()
{
    bool result;
    mutex.lock();
    result = targetBool;
    mutex.unlock();
    return result;
}
MutexBool::MutexBool(bool b)
{
    targetBool = b;
}

MutexBool::MutexBool()
{
    targetBool = false;
}
