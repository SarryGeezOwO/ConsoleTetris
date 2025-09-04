#ifndef CONSOLEUTIL_HPP
#define CONSOLEUTIL_HPP

#include <Windows.h>

struct Console
{
    static void hideCursor();

    static void setCursorPosition(int x, int y);

    static void getCursorPosition(int& x_out, int& y_out);
};

#endif