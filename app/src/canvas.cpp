#include "canvas.hpp"
#include "consoleutil.hpp"
#include <unordered_map>
#include <iostream>
#include <string>

// Platform specific Canvas indeed. Fuck you linux users
// not that I hate you guys, but the idea of 
// cross platform is scratching my head hard

static std::unordered_map<SpriteType, const char*> sprite_map = {
    {SPRITE_EMPTY,    "░░"},
    {SPRITE_FILL,     "██"},
    {SPRITE_LOCKED,   "██"},
    {SPRITE_GHOST,    "▒▒"},
};

Canvas::Canvas(int w, int h) 
    : m_width(w), m_height(h), m_grid(h, std::vector<unsigned int>(w, 0)){}

Canvas::~Canvas() {}

void Canvas::init()
{
    // Set console output codepage to UTF-8
    SetConsoleOutputCP(CP_UTF8);
    Console::hideCursor();
    Console::getCursorPosition(m_startX, m_startY);
}

void Canvas::clear()
{
    Console::setCursorPosition(m_startX, m_startY);
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            clearCell(x, y);
        }
    }
}

void Canvas::clearCell(int x, int y)
{
    m_grid[y][x] = SPRITE_EMPTY;
}

void Canvas::render()
{
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            unsigned int cellValue = m_grid[y][x];
            
            // Draw cell
            SpriteType spr = static_cast<SpriteType>((cellValue >> 8) & 0xF);
            unsigned int color = cellValue & 0xFF;
            std::cout << "\x1B[" << std::to_string(color).c_str() << "m" << (sprite_map[spr]) << "\033[0m";
        }
        std::cout << std::endl;
    }
}

void Canvas::draw_sprite(SpriteType sprite, unsigned int colorCode, int x, int y)
{
    // We know color code (foreground)
    // only ranges from 30 - 37 for non high intensive colors
    
    if (x < 0 || x >= m_width) {
        return;
    }

    if (y < 0 || y >= m_height) {
        return;
    }
    m_grid[y][x] = (sprite << 8) | (unsigned int)colorCode;
}
