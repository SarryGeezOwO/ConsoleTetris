#include "tetromino.hpp"
#include "color.hpp"
#include <string>
#include <iostream>

// Returns a random Windows console color attribute (31-36, skipping 30 and 37 for black and white)
// BTW Tetrominos actually have their own custom color, but
// with this shit implemented I don't ahve to actually know what 
// color value a tetromino should have lmao, very robust shit indeed

/*  **HISTORY**
static unsigned int getRandomConsoleColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(31, 37);
    return dis(gen);
}
*/

// Read from a settings file for more customization
// If possible even
static unsigned int getTetrominoColor(char type) {
    switch (type) {
        case 'I': return (int)CYAN;
        case 'O': return (int)YELLOW;
        case 'T': return (int)MAGENTA;
        case 'S': return (int)GREEN;
        case 'Z': return (int)RED;
        case 'J': return (int)BLUE;
        case 'L': return (int)YELLOW;
    }
    return 30;
}

static Tetromino getRandomTetromino_NES() {
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(0, 6);

    switch (dist(rng)) {
        case 0: return Tetromino_I();
        case 1: return Tetromino_O();
        case 2: return Tetromino_T();
        case 3: return Tetromino_S();
        case 4: return Tetromino_Z();
        case 5: return Tetromino_J();
        case 6: return Tetromino_L();
    }
}

static Tetromino getTetrominoByChar(char c) {
    switch (c) {
        case 'I': return Tetromino_I();
        case 'O': return Tetromino_O();
        case 'T': return Tetromino_T();
        case 'S': return Tetromino_S();
        case 'Z': return Tetromino_Z();
        case 'J': return Tetromino_J();
        case 'L': return Tetromino_L();
    }
}

void Tetromino::rotateCW()
{
    for (auto& pos : localPositions) {
        int temp = pos.x;
        pos.x = pos.y;
        pos.y = -temp;
    }
}

void Tetromino::rotateCCW()
{
    for (auto& pos : localPositions) {
        int temp = pos.x;
        pos.x = -pos.y;
        pos.y = temp;
    }
}

TetrmoninoEntity::TetrmoninoEntity()
    : m_data(), m_color(getTetrominoColor(m_data.type))
{}

TetrmoninoEntity::TetrmoninoEntity(Tetromino &data)
{
    m_data = data;
    m_color = getTetrominoColor(m_data.type);
}

TetrmoninoEntity::TetrmoninoEntity(Tetromino &&data)
{
    m_data = data;
    m_color = getTetrominoColor(m_data.type);
}

TetrominoManager::TetrominoManager(Vec2 size, Canvas* pcanvas)
    : m_mapSize(size), p_canvas(pcanvas)
{
    changeTetromino(getRandomTetromino_NES(), false);
}

void TetrominoManager::onTetrominoChange(void (*callback)(const TetrmoninoEntity teto))
{
    if (callback) {
        callback_onChange = callback;
    }
}

void TetrominoManager::changeTetromino(Tetromino &&next, bool callback_invoke)
{
    if (callback_onChange && callback_invoke) {
        callback_onChange(m_current);
    }

    while(m_tetro_queue.size() < 5) {
        m_tetro_queue.push(TetrmoninoEntity(getRandomTetromino_NES()));
    }

    m_current = next;
    resetPos();
}

void TetrominoManager::update()
{
    if (isGrounded().y == 0) {
        m_current.m_data.position.y += 1;
    }
    else {
        changeTetromino(getTetrominoByChar(m_tetro_queue.front().m_data.type), true);
        m_tetro_queue.pop();
    }
}

// Check for each tetromino cell if ground is touched
Vec2 TetrominoManager::isGrounded() const
{
    Vec2 result;
    for (const Vec2& p : m_current.m_data.localPositions) {

        // Out of bounds
        int checkY = m_current.m_data.position.y + p.y + 1;
        int checkX = m_current.m_data.position.x + p.x;
        if (checkY >= m_mapSize.y) {
            result.y = 1;
        }

        if (checkX < 0 || checkX >= m_mapSize.x) {
            result.x = 1;
        }

        // Tetromino spotted
        int cellX = p_canvas->get_cell_data(checkX, m_current.m_data.position.y + p.y);
        int cellY = p_canvas->get_cell_data(checkX, checkY);
        int sprX = (cellX >> 8) & 0xF;
        int sprY = (cellY >> 8) & 0xF;
        if (sprX == (int)SPRITE_FILL || sprX == (int)SPRITE_LOCKED) {
            result.x = 1;
        }

        if (sprY == (int)SPRITE_FILL || sprY == (int)SPRITE_LOCKED) {
            result.y = 1;
        }
    }
    return result;
}

void TetrominoManager::hold()
{
    char temp = m_current.m_data.type;

    // Check for no hold
    if (m_hold.m_data.type == ' ') {
        // Simply store the current and move on the next tetromino
        m_hold = getTetrominoByChar(temp);
        changeTetromino(getTetrominoByChar(m_tetro_queue.front().m_data.type), false);
        m_tetro_queue.pop();
        return;
    }

    // If m_hold is carrying somehing
    changeTetromino(getTetrominoByChar(m_hold.m_data.type), false);
    m_hold = getTetrominoByChar(temp);
}

void TetrominoManager::move(Vec2 offset)
{
    Vec2 before = m_current.m_data.position;
    m_current.m_data.position.x += offset.x;
    m_current.m_data.position.y += offset.y;

    // Revert back if new position is invalid
    if(isGrounded().x == 1) {
        m_current.m_data.position.x = before.x;        
    }

    if(isGrounded().y == 1) {
        m_current.m_data.position.y = before.y;        
    }
}

void TetrominoManager::hard_drop()
{
    // CODE whatever
    while(isGrounded().y == 0) {
        m_current.m_data.position.y++;
    }
}

void TetrominoManager::rotateCW()
{
    m_current.m_data.rotateCW();
    Vec2 check = isGrounded();
    if(check.x == 1 || check.y == 1) {
        m_current.m_data.rotateCCW();
    }
}

void TetrominoManager::rotateCCW() 
{
    m_current.m_data.rotateCCW();
    Vec2 check = isGrounded();
    if(check.x == 1 || check.y == 1) {
        m_current.m_data.rotateCW();
    }
}

void TetrominoManager::resetPos()
{
    m_current.m_data.position.x = m_mapSize.x/2;
    m_current.m_data.position.y = 0;
}
