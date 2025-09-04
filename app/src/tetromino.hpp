#ifndef TETROMINO_HPP
#define TETROMINO_HPP

#include <random>
#include <vector>
#include <queue>
#include "vector.hpp"
#include "canvas.hpp"

// We are going to be using modern Tetris 
// randomization of tetromino
// 7-bag shuffle

// We might want special behaviours later with our tetrominos
// so I will stick to inheritance on this case

// Imagine a tetromino center point resides on the 
// center of a 3x3 grid map
// Y is reversed

struct Tetromino
{
    Vec2 position;
    std::vector<Vec2> localPositions;
    char type = ' ';

    // Rotate clockwise
    void rotateCW();

    // Rotate counter-clockwise
    void rotateCCW();
};

struct Tetromino_T : Tetromino
{
    Tetromino_T() {
        localPositions = {{0, 0}, {-1, 0}, {1, 0}, {0, -1},};
        type = 'T';
    }
};

struct Tetromino_I : Tetromino
{
    Tetromino_I() {
        // Horizontal line centered
        localPositions = {
            { -2, 0 }, { -1, 0 }, { 0, 0 }, { 1, 0 }
        };
        type = 'I';
    }
};

struct Tetromino_O : Tetromino
{
    Tetromino_O() {
        // 2x2 square centered around (0,0)
        localPositions = {
            { 0, 0 }, { 1, 0 }, { 0, -1 }, { 1, -1 }
        };
        type = 'O';
    }
};

struct Tetromino_S : Tetromino
{
    Tetromino_S() {
        localPositions = {
            { 0, 0 }, { 1, 0 }, { 0, -1 }, { -1, -1 }
        };
        type = 'S';
    }
};

struct Tetromino_Z : Tetromino
{
    Tetromino_Z() {
        localPositions = {
            { 0, 0 }, { -1, 0 }, { 0, -1 }, { 1, -1 }
        };
        type = 'Z';
    }
};

struct Tetromino_J : Tetromino
{
    Tetromino_J() {
        localPositions = {
            { 0, 0 }, { -1, 0 }, { 1, 0 }, { -1, -1 }
        };
        type = 'J';
    }
};

struct Tetromino_L : Tetromino
{
    Tetromino_L() {
        localPositions = {
            { 0, 0 }, { -1, 0 }, { 1, 0 }, { 1, -1 }
        };
        type = 'L';
    }
};

struct TetrmoninoEntity
{
public:
    Tetromino m_data;
    unsigned int m_color;

    TetrmoninoEntity();
    TetrmoninoEntity(Tetromino& data);
    TetrmoninoEntity(Tetromino&& data);
};

struct TetrominoManager
{
private:
    Canvas* p_canvas = nullptr;
    TetrmoninoEntity m_current;
    TetrmoninoEntity m_hold;
    std::queue<TetrmoninoEntity> m_tetro_queue;
    Vec2 m_mapSize;
    void (*callback_onChange)(const TetrmoninoEntity teto);

public:
    TetrominoManager(Vec2 size, Canvas* pcanvas);

    // Just pass in a Derived class of Tetromino bro
    void onTetrominoChange(void (*callback)(const TetrmoninoEntity teto));
    void changeTetromino(Tetromino&& next, bool callback_invoke);
    void update();

    Vec2 isGrounded() const;
    void hold();
    void move(Vec2 offset);
    void hard_drop();
    void rotateCW();
    void rotateCCW();
    void resetPos();
    inline TetrmoninoEntity getTetromino() const { return m_current; }
    inline TetrmoninoEntity getHoldTetromino() const { return m_hold; }
    inline std::queue<TetrmoninoEntity> getQueueCopy() const { return m_tetro_queue; }
};

#endif