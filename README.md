# Console Tetris
A badly written Tetris that uses only the console for graphics
only windows specific because this program utilizes the Windows API.
Written in C++11 and makefile for building.

## Info
it has no gameover state yet, and doesn't use the modern
8-Bag shuffle algorithm for the generation of tetrominos.
Colors are also dependent on the console/terminal used.

## Build
You can use the makefile to build and generate the game. Though I am not sure
if it will work. (After all, I am just an amateur)
 - `make run` - to build and immediately run the game.
 - `make` - build the game.

## Inputs
 - `A` - Move left
 - `D` - Move Right
 - `S` - Soft drop
 - `SPACE` - Hard drop
 - `Q` - Swap / Hold tetromino
 - `X` - Rotate Counter-clockwise
 - `C` - Rotate Clockwise
 - `ESC` - Exit game

# Preview:

![WindowsTerminal_8pktVBDlD4](https://github.com/user-attachments/assets/7f6938f7-81fe-45ec-ac68-6aa3139bf5c3)
