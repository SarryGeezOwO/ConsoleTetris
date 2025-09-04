#include "gui.hpp"
#include "consoleutil.hpp"
#include <iostream>
#include <algorithm>

// Position is Top-Left origin point
Vec2 GUI::cursorStart = Vec2(0, 0);

GUI_Component::GUI_Component(Vec2 position, Vec2 size)
    : m_position(position), m_dimension(size)
{
    m_depth = 0;
    m_foreground = WHITE;
    m_background = BG_BLACK;
}

GUI_TextView::GUI_TextView(Vec2 position, Vec2 size, const char* str)
    : GUI_Component(position, size), m_text(str)
{
    setForeground(WHITE);
    setBackground(BG_BLACK);
}

void GUI_TextView::render() const
{
    int textLen = static_cast<int>(m_text.length());
    const char* fg = std::to_string(getForeground()).c_str();
    const char* bg = std::to_string(getBackground()).c_str();
    int printed = 0;

    if (!m_drawBG) {
        bg = "40"; // BG_BLACK
    }

    std::string colorSeq = std::string("\x1b[") + fg + ";" + bg + "m";
    for (int y = 0; y < m_dimension.y; y++) {

        // Move cursor and apply color
        Console::setCursorPosition(m_position.x + GUI::cursorStart.x, m_position.y + GUI::cursorStart.y + y);
        std::cout << colorSeq;

        for (int x = 0; x < m_dimension.x; x++) {
            if (printed < textLen) {
                putchar(m_text[printed++]);
            } else {
                putchar(' ');
            }
        }

        // Reset color
        std::cout << "\033[0m";
    }
}

GUI_Tetromino::GUI_Tetromino(Vec2 pos, Tetromino tetromino, unsigned int color)
    : GUI_Component(pos, {10, 5}), m_tetromino(tetromino), m_color(color)
{}

void GUI_Tetromino::render() const
{
    // Prepare color sequence for tetromino
    const char* fg = std::to_string(getForeground()).c_str();
    std::string colorSeq = std::string("\x1b[") + fg + "m";

    // Draw tetromino blocks
    std::cout << colorSeq;
    Vec2 center(
        (m_position.x + GUI::cursorStart.x) + m_dimension.x / 2,
        (m_position.y + GUI::cursorStart.y) + m_dimension.y / 2
    );
    for (const Vec2& lp : m_tetromino.localPositions) {
        int drawX = center.x + (lp.x * 2);
        int drawY = center.y + lp.y;

        // Only draw if inside the component's area
        if (drawX >= m_position.x + GUI::cursorStart.x &&
            drawX < m_position.x + GUI::cursorStart.x + m_dimension.x &&
            drawY >= m_position.y + GUI::cursorStart.y &&
            drawY < m_position.y + GUI::cursorStart.y + m_dimension.y) {
            Console::setCursorPosition(drawX, drawY);
            puts("██");
        }
    }
    std::cout << "\033[0m";
}

void GUI_Tetromino::renderBG() const
{
    const char* bg = std::to_string(getBackground()).c_str();
    if (!m_drawBG) {
        bg = "40"; // BG_BLACK
    }

    std::cout << std::string("\x1b[") + bg + "m";
    for (int y = 0; y < m_dimension.y; y++) {
        Console::setCursorPosition(m_position.x + GUI::cursorStart.x, m_position.y + GUI::cursorStart.y + y);
        for (int x = 0; x < m_dimension.x; x++) {
            putchar(' ');
        }
    }
    std::cout << "\033[0m";
    
}
