/*
 * LUDO GAME - BOARD IMPLEMENTATION (board.cpp)
 * -------------------------------------------
 * This file handles the 15x15 grid layout, coordinates, and all drawing operations.
 * 
 * DESIGN FOR EASY EXPLANATION:
 * 1. The board is divided into a 15x15 grid of square cells (CELL_SIZE = 40 pixels).
 * 2. Corners are 6x6 blocks: Red (top-left), Green (top-right), Yellow (bottom-right), Blue (bottom-left).
 * 3. The arms are 6x3 blocks of cells.
 * 4. The center is a 3x3 block containing 4 colored triangles meeting in the middle.
 * 5. Everything is drawn using simple SFML shapes (RectangleShape, CircleShape, ConvexShape, Text).
 */

#include "ludo.h"
#include <iomanip>

int boardPathX[TOTAL_PATH_CELLS];
int boardPathY[TOTAL_PATH_CELLS];
int corridorX[MAX_PLAYERS][HOME_CORRIDOR_CELLS];
int corridorY[MAX_PLAYERS][HOME_CORRIDOR_CELLS];
bool safeCells[TOTAL_PATH_CELLS];

// Converts grid coordinates (gx: 0..14, gy: 0..14) to screen pixel coordinates
Vector2f getGridPixel(int gx, int gy) {
    return Vector2f(BOARD_X + gx * CELL_SIZE + CELL_SIZE / 2.0f,
                    BOARD_Y + gy * CELL_SIZE + CELL_SIZE / 2.0f);
}

void initializeBoard() {}
void drawBoard(RenderWindow &window) {}
