#ifndef COLOR_HPP
#define COLOR_HPP

#include <string>

enum Color {
    BLACK   = 30,
    RED     = 31,
    GREEN   = 32,
    YELLOW  = 33,
    BLUE    = 34,
    MAGENTA = 35,
    CYAN    = 36,
    WHITE   = 37,
    GRAY    = 90,

    BG_BLACK   = 40,
    BG_RED     = 41,
    BG_GREEN   = 42,
    BG_YELLOW  = 43,
    BG_BLUE    = 44,
    BG_MAGENTA = 45,
    BG_CYAN    = 46,
    BG_WHITE   = 47,
    BG_GRAY    = 100
};

struct ColorUtil {
    static const char* getColorStr(Color color);
    static const char* getResetColor();
};

#endif