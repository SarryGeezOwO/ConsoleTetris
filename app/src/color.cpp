#include "color.hpp"

const char* ColorUtil::getColorStr(Color color) {
    return (std::string("\x1b[") + std::to_string((int)color) + "m").c_str();
}

const char* ColorUtil::getResetColor() {
    return "\x1b[0m";
}