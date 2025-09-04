#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <vector>

enum SpriteType 
{
    SPRITE_EMPTY  =  0,
    SPRITE_FILL   =  1,
    SPRITE_LOCKED =  2, // Like you know not the selected one
    SPRITE_GHOST  =  3
};

struct Canvas 
{
private:
    int m_height;
    int m_width;
    int m_startX;
    int m_startY;
    std::vector<std::vector<unsigned int>> m_grid;

public:
    Canvas(int w, int h);
    ~Canvas();

    void init();
    void clear();
    void clearCell(int x, int y);
    void render();
    void draw_sprite(SpriteType sprite, unsigned int colorCode, int x, int y);

    inline int get_width() const { return m_width; }
    inline int get_height() const { return m_height; }
    inline int get_cell_data(int x, int y) const { 
        if (x < 0 || x >= m_width) {
            return -1;
        }

        if (y < 0 || y >= m_height) {
            return -1;
        }
        return m_grid[y][x];
    };
    inline std::vector<std::vector<unsigned int>> get_grid() const { return m_grid; }
};


#endif