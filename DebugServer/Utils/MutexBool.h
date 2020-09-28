//
// Created by nickolay on 09.06.2020.
//

#ifndef DRONEAPP_MUTEXBOOL_H
#define DRONEAPP_MUTEXBOOL_H


#include <mutex>

class MutexBool {
public:
    MutexBool();
    MutexBool(bool b);
    bool get();
    void set(bool b);
private:
    std::mutex mutex;
    bool targetBool = false;
};


#endif //DRONEAPP_MUTEXBOOL_H
