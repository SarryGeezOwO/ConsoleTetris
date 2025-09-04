#include <iostream>
#include <chrono>
#include <string>
#include <Windows.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

#include "log.hpp"
#include "vector.hpp"
#include "canvas.hpp"
#include "tetromino.hpp"
#include "gui.hpp"
#include "consoleutil.hpp"

#define GRID_HEIGHT 20
#define GRID_WIDTH 10
#define FPS_MS 30 / 1000

#define TIME_TO_MS(x) std::chrono::duration_cast<std::chrono::milliseconds>(x).count()
#define TIME_NOW      std::chrono::high_resolution_clock::now()
#define TIME          std::chrono::high_resolution_clock::time_point

// This game is Windows specific, and not runnable 
// on other platforms. So don't even think about it
// This game heavily uses the Windows API for things

// TODO::
//      >> Swap and hold tetromino ✅
//      >> Show next tetromino by 4 ahead ✅
//      >> Have a lauch options [-NES (Use NES randomization)]
//          >> Without this flag call, it will utilize 7-bad shuffle algo
//      >> Game Over
//          >> Retry
//          >> Back to main menu
//      >> Pressing 'Q' will go back to main menu
//      >> Main menu
//          >> Play
//          >> Exit
//          >> About

bool isRunning = true;
int gravityMS = 300;
Canvas canvas(GRID_WIDTH, GRID_HEIGHT);
TIME gravityTick;
TIME moveTick;
Vec2 startingCursorPos;

// OMG, it's kasane teto!!
TetrominoManager teto_manager({GRID_WIDTH, GRID_HEIGHT}, &canvas);
std::unordered_map<Vec2, unsigned int> lockedCells;
int  highestClearRow = -1;
int  clearedRowCount =  0;
bool callProcessLines   = false;
bool callUpdateNextGUI  = true;
bool callUpdateSwaptGUI = true;

int score = 0;
GUI_TextView  scoreLabel({(GRID_WIDTH*2) + 2, 0}, {20, 1}, "Score: 0");
GUI_TextView  swapLabel({(GRID_WIDTH) - 2, GRID_HEIGHT+1}, {4, 1}, "HOLD");
GUI_Tetromino nextTetromino_1({(GRID_WIDTH*2) + 2,  2}, Tetromino_S(), 32);
GUI_Tetromino nextTetromino_2({(GRID_WIDTH*2) + 2,  2+6}, Tetromino_S(), 32);
GUI_Tetromino nextTetromino_3({(GRID_WIDTH*2) + 2,  2+6+6}, Tetromino_S(), 32);
GUI_Tetromino swapTetrominoGUI({GRID_WIDTH/2, GRID_HEIGHT+2}, Tetromino_S(), 32);

bool checkKey(int vKey) {
    bool b = GetAsyncKeyState(vKey) & 0x8000;
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    return b;
}

bool checkKeyPress(int vKey) {
    bool b = GetAsyncKeyState(vKey) & 0x01;
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    return b;
}

bool timeDiff(TIME last, int msToCheck) {
    return TIME_TO_MS(TIME_NOW - last) >= msToCheck;
}

void moveDownRows() {

    while(highestClearRow >= 0) {

        size_t row_size = canvas.get_grid()[highestClearRow].size();
        for(int x = 0; x < (int)row_size; x++) {
            // For every cell on that row move them down by one
            int cell = canvas.get_cell_data(x, highestClearRow);
            int spr  = (cell >> 8) & 0xF;
            unsigned int color = cell & 0xFF;

            if (spr != (int)SPRITE_LOCKED) {
                continue;
            }

            canvas.clearCell(x, highestClearRow);
            lockedCells.erase({x, highestClearRow});

            int moveY = highestClearRow + clearedRowCount;
            lockedCells[{x, moveY}] = color;
            canvas.draw_sprite(
                SPRITE_LOCKED, color,
                x, moveY
            );
        }

        highestClearRow--;
    }
}

void processLines() {
    callUpdateNextGUI = true;
    highestClearRow = -1;
    clearedRowCount =  0;

    bool lowestFound = false;
    for(size_t y = 0; y < canvas.get_grid().size(); y++) {
        const auto row = canvas.get_grid()[y];

        unsigned int cellR = 0;
        for(size_t i = 0; i < row.size(); i++) {
            // Including ghost cells
            int spr = (row[i] >> 8) & 0xF;
            if (spr == (int)SPRITE_LOCKED) {
                cellR++;
            }
        }

        if (cellR != row.size()) {
            continue;
        }

        // Set every cell to empty
        if (!lowestFound) {
            highestClearRow = (int)y;
            lowestFound = true;
        }

        clearedRowCount++;
        for(size_t x = 0; x < row.size(); x++) {
            canvas.draw_sprite(
                SPRITE_EMPTY, 37, x, y
            );

            // Remove this cell from lockedCells
            Vec2 pos((int)x, (int)y);
            lockedCells.erase(pos);
        }
    }

    // Give points
    switch(clearedRowCount)
    {
        case 1: score += 40; break;
        case 2: score += 100; break;
        case 3: score += 300; break;
        case 4: score += 1200; break;
    }
}

void callback_tetrominoChange(const TetrmoninoEntity ent) {
    for (const auto lp : ent.m_data.localPositions) {
        // Only lock cells within the visible grid
        Vec2 pos(ent.m_data.position.x + lp.x, ent.m_data.position.y + lp.y);
        lockedCells[pos] = ent.m_color;
    }
    callProcessLines = true;
}

void updateNextTetrominoGUI() {   
    // 🐐🐐🐐
    auto queue = teto_manager.getQueueCopy();
    nextTetromino_1.setTetromino(queue.front().m_data);
    nextTetromino_1.setForeground((Color)queue.front().m_color);
    queue.pop();
    nextTetromino_2.setTetromino(queue.front().m_data);
    nextTetromino_2.setForeground((Color)queue.front().m_color);
    queue.pop();
    nextTetromino_3.setTetromino(queue.front().m_data);
    nextTetromino_3.setForeground((Color)queue.front().m_color); 

    nextTetromino_1.renderBG();
    nextTetromino_2.renderBG();
    nextTetromino_3.renderBG();

    nextTetromino_1.render();
    nextTetromino_2.render();
    nextTetromino_3.render();
}

void updateSwapTetrominoGUI() {
    auto t = teto_manager.getHoldTetromino();
    swapTetrominoGUI.setTetromino(t.m_data);
    swapTetrominoGUI.setForeground((Color)t.m_color);
    swapTetrominoGUI.renderBG();
    swapTetrominoGUI.render();
}

void start() {
    // Something
    teto_manager.onTetrominoChange(callback_tetrominoChange);
    scoreLabel.setForeground(GREEN);
    scoreLabel.setBackground(BG_BLACK);

    nextTetromino_1.setBackground(BG_GRAY);
    nextTetromino_2.setBackground(BG_BLACK);
    nextTetromino_3.setBackground(BG_BLACK);

    swapTetrominoGUI.setForeground(RED);
    swapTetrominoGUI.setBackground(BG_GRAY);
}

void update() {
    static bool firstCall = true;
    if (firstCall) {
        // Yes I know, I truly am the goat 🐐🐐🐐                                                           
        teto_manager.resetPos();
    }

    // Gravity simulate
    if (timeDiff(gravityTick, gravityMS)) {
        if(!firstCall) {
            teto_manager.update();
        }
        else {
            teto_manager.resetPos();
            firstCall = false;
        }
        gravityTick = TIME_NOW;
    }
    
    // Rotate CW
    if (checkKeyPress('C')) {
        teto_manager.rotateCW();
    }

    // Rotate CCW
    if (checkKeyPress('X')) {
        teto_manager.rotateCCW();
    }

    // Hard drop
    if (checkKeyPress(VK_SPACE)) {
        teto_manager.hard_drop();

        // Force update
        if(!firstCall) teto_manager.update();
        else teto_manager.resetPos();
    }

    // Hold current tetromino
    if (checkKeyPress('Q')) {
        teto_manager.hold();
        callUpdateSwaptGUI = true;
    }

    if (timeDiff(moveTick, 60)) {
        // Left
        if (checkKey('A')) {
            teto_manager.move({-1, 0});
        }

        // Right
        if (checkKey('D')) {
            teto_manager.move({1, 0});
        }

        // Soft drop
        if (checkKey('S')) {
            teto_manager.move({0, 1});
        }
        moveTick = TIME_NOW;
    }
}

void preDraw() {
    // Draw all locked cells
    for (const auto& cell : lockedCells) {
        canvas.draw_sprite(
            SPRITE_LOCKED, 
            cell.second,
            cell.first.x,
            cell.first.y
        );
    }

    // Happens after locked cells, to
    // check
    if (callProcessLines) {
        processLines();
        callProcessLines = false;
    }
}

void draw() {
    TetrmoninoEntity ent = teto_manager.getTetromino();
    bool ground = false;

    Vec2 ghostPos = ent.m_data.position;
    while(!ground) {
        for (const Vec2& p : ent.m_data.localPositions) {
            // Out of bounds
            int checkY = ghostPos.y + p.y + 1;
            if (checkY >= GRID_HEIGHT) {
                ground = true;
                break;
            }

            // Tetromino spotted
            int cell = canvas.get_cell_data(ghostPos.x + p.x, checkY);
            int spr = (cell >> 8) & 0xF;
            if (spr == (int)SPRITE_FILL || spr == (int)SPRITE_LOCKED) {
                ground = true;
                break;
            }
        }

        if (!ground) {
            ghostPos.y++;
        }
    }

    for (const Vec2& pos : ent.m_data.localPositions) {
        // Ghost
        canvas.draw_sprite(
            SPRITE_GHOST,
            37,
            ghostPos.x + pos.x,
            ghostPos.y + pos.y
        );
    }

    // Make tetromino appear in front of ghost
    for (const Vec2& pos : ent.m_data.localPositions) {
        canvas.draw_sprite(
            SPRITE_FILL, 
            ent.m_color,
            pos.x + ent.m_data.position.x, 
            pos.y + ent.m_data.position.y
        );
    }
}

void app_quit() {
    Console::setCursorPosition(0, GRID_HEIGHT + startingCursorPos.y + 1);
}

void renderGUI() {
    if (callUpdateNextGUI) {
        updateNextTetrominoGUI();
        callUpdateNextGUI = false;
    }

    if (callUpdateSwaptGUI) {
        updateSwapTetrominoGUI();
        callUpdateSwaptGUI = false;
    }

    scoreLabel.setText("SCORE: " + std::to_string(score));
    scoreLabel.render();
    swapLabel.render();
}

int main(int argc, const char *argv[]) {
    Console::getCursorPosition(startingCursorPos.x, startingCursorPos.y);
    Console::getCursorPosition(GUI::cursorStart.x, GUI::cursorStart.y);
    canvas.init();
    start();

    gravityTick = TIME_NOW;
    moveTick    = TIME_NOW;

    while(isRunning) {
        if (checkKeyPress(VK_ESCAPE)) {
            isRunning = false;
        }

        renderGUI();
        canvas.clear();
        preDraw();
        update();
        draw();

        moveDownRows();
        canvas.render();
        Sleep(FPS_MS);
    }

    app_quit();
    return 0;
}