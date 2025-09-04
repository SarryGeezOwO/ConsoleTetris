#ifndef GUI_HPP
#define GUI_HPP

#include "tetromino.hpp"
#include "vector.hpp"
#include "color.hpp"
#include <string>

struct GUI {
    static Vec2 cursorStart;
};

// Lower depth means in front more

struct GUI_Component 
{
private:
    Color m_foreground; // ASCII chars
    Color m_background; // Background duh

public:
    Vec2 m_position;
    Vec2 m_dimension;
    int  m_depth;
    bool m_drawBG = true;
    
    GUI_Component(Vec2 position, Vec2 size);

    virtual void render() const = 0;
    inline void setForeground(Color col) { m_foreground = col; }
    inline void setBackground(Color col) { m_background = col; }
    inline Color getForeground() const { return m_foreground; }
    inline Color getBackground() const { return m_background; }
};

struct GUI_TextView : GUI_Component
{
private:
    std::string m_text;

public:
    GUI_TextView(Vec2 position, Vec2 size, const char* str);
    void render() const override;
    inline void setText(std::string text) { m_text = text; }
    inline std::string getText() const { return m_text; }
};

struct GUI_Tetromino : GUI_Component
{
private:
    Tetromino m_tetromino;
    unsigned int m_color;

public:
    GUI_Tetromino(Vec2 pos, Tetromino tetromino, unsigned int color);
    void render() const override;
    void renderBG() const;

    inline void setTetromino(Tetromino t) { m_tetromino = t; }
    inline unsigned int getTetrominoColor() const { return m_color; }
    inline char getTetrominoType() const { return m_tetromino.type; }
}; 

#endif