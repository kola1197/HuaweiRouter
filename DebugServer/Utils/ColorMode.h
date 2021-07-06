//
// Created by nickolay on 28.09.2020.
// Class for color console output. Used for debug.
//
#include <ostream>

namespace Color {
#ifndef HUAWEIROUTER_COLORMODE_H
#define HUAWEIROUTER_COLORMODE_H

    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        FG_YELLOW   = 33,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_YELLOW   = 43
    };
    class ColorMode {
        Code code;
    public:
        ColorMode(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const ColorMode& mod) {
            return os << "\033[" << mod.code << "m";
        }

    };


#endif //HUAWEIROUTER_COLORMODE_H
}