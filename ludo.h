/*
 * LUDO GAME - HEADER FILE (ludo.h)
 * ---------------------------------
 * This file contains all constants, data structures, and function declarations.
 * 
 * DATA STRUCTURES USED IN THIS PROJECT (FOR VIVA / MAM):
 * 1. QUEUE (FIFO)       : `std::queue<int> diceHistory`
 *    - Stores recent dice rolls. New rolls are pushed to the back;
 *      when size > 5, the oldest roll is popped from the front.
 * 2. STACK (LIFO)       : `std::stack<MoveRecord> moveHistory`
 *    - Implements the "UNDO" feature (Press 'U').
 *    - Every move pushes a `MoveRecord` onto the stack.
 *    - Pressing 'U' pops the top record and restores previous positions!
 * 3. LINKED LIST        : `std::list<string> actionLog`
 *    - Stores the live history log of game events (captures, moves, exits).
 * 4. 1D & 2D ARRAYS     :
 *    - `boardPathX[52]`, `boardPathY[52]` for O(1) path coordinate lookup.
 *    - `corridorX[4][5]`, `corridorY[4][5]` for player home corridors.
 *    - `safeCells[52]` boolean array for O(1) safe-cell lookup.
 * 5. STRUCTS            :
 *    - `struct Token`: id, step (0..57), status (0..2).
 *    - `struct Player`: name, color, tokens[4], tokensFinished.
 */

#ifndef LUDO_H
#define LUDO_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;
using namespace sf;

// ========================== CONSTANTS ==========================

const int MAX_PLAYERS = 4;           // 4 players: Red, Green, Yellow, Blue
const int TOKENS_PER_PLAYER = 4;     // 4 tokens per player
const int TOTAL_PATH_CELLS = 52;     // 52 cells in the main circular loop
const int HOME_CORRIDOR_CELLS = 5;   // 5 colored cells in each player's private corridor
const int FINISH_STEP = 57;          // Reaching step 57 means token reached the finish!

// Window and Board dimensions
const float WINDOW_WIDTH = 960.0f;
const float WINDOW_HEIGHT = 700.0f;
const float BOARD_SIZE = 600.0f;
const float CELL_SIZE = 40.0f;       // 15 grid cells * 40 px = 600 px
const float BOARD_X = 20.0f;         // Board position X
const float BOARD_Y = 50.0f;         // Board position Y
const float TOKEN_RADIUS = 13.0f;    // Radius of token circle


#endif // LUDO_H
