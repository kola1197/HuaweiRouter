//
// Created by nickolay on 12.12.2020.
//

#include "sout.h"
#include <iostream>
#include <mutex>
#include <thread>
#include "string.h"

std::mutex Sout::mut;
std::string sim::endl = "\n";
Sout sim::sout = Sout(sim::endl);
