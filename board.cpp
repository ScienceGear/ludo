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

void initializeBoard() {
    // 1. Initialize all safe cells to false
    for (int i = 0; i < TOTAL_PATH_CELLS; i++) {
        safeCells[i] = false;
    }

    // 2. Define the 52-cell main path in (gx, gy) grid coordinates
    // Red arm going right (cells 0 to 4)
    boardPathX[0] = 1;  boardPathY[0] = 6;  // Red Start Cell
    boardPathX[1] = 2;  boardPathY[1] = 6;
    boardPathX[2] = 3;  boardPathY[2] = 6;
    boardPathX[3] = 4;  boardPathY[3] = 6;
    boardPathX[4] = 5;  boardPathY[4] = 6;

    // Top arm going up (cells 5 to 10)
    boardPathX[5] = 6;  boardPathY[5] = 5;
    boardPathX[6] = 6;  boardPathY[6] = 4;
    boardPathX[7] = 6;  boardPathY[7] = 3;
    boardPathX[8] = 6;  boardPathY[8] = 2;  // Star / Safe Cell
    boardPathX[9] = 6;  boardPathY[9] = 1;
    boardPathX[10] = 6; boardPathY[10] = 0;

    // Top edge transition (cells 11, 12)
    boardPathX[11] = 7; boardPathY[11] = 0;
    boardPathX[12] = 8; boardPathY[12] = 0;

    // Top arm going down (cells 13 to 17)
    boardPathX[13] = 8; boardPathY[13] = 1; // Green Start Cell
    boardPathX[14] = 8; boardPathY[14] = 2;
    boardPathX[15] = 8; boardPathY[15] = 3;
    boardPathX[16] = 8; boardPathY[16] = 4;
    boardPathX[17] = 8; boardPathY[17] = 5;

    // Right arm going right (cells 18 to 23)
    boardPathX[18] = 9;  boardPathY[18] = 6;
    boardPathX[19] = 10; boardPathY[19] = 6;
    boardPathX[20] = 11; boardPathY[20] = 6;
    boardPathX[21] = 12; boardPathY[21] = 6; // Star / Safe Cell
    boardPathX[22] = 13; boardPathY[22] = 6;
    boardPathX[23] = 14; boardPathY[23] = 6;

    // Right edge transition (cells 24, 25)
    boardPathX[24] = 14; boardPathY[24] = 7;
    boardPathX[25] = 14; boardPathY[25] = 8;

    // Right arm going left (cells 26 to 30)
    boardPathX[26] = 13; boardPathY[26] = 8; // Yellow Start Cell
    boardPathX[27] = 12; boardPathY[27] = 8;
    boardPathX[28] = 11; boardPathY[28] = 8;
    boardPathX[29] = 10; boardPathY[29] = 8;
    boardPathX[30] = 9;  boardPathY[30] = 8;

    // Bottom arm going down (cells 31 to 36)
    boardPathX[31] = 8; boardPathY[31] = 9;
    boardPathX[32] = 8; boardPathY[32] = 10;
    boardPathX[33] = 8; boardPathY[33] = 11;
    boardPathX[34] = 8; boardPathY[34] = 12; // Star / Safe Cell
    boardPathX[35] = 8; boardPathY[35] = 13;
    boardPathX[36] = 8; boardPathY[36] = 14;

    // Bottom edge transition (cells 37, 38)
    boardPathX[37] = 7; boardPathY[37] = 14;
    boardPathX[38] = 6; boardPathY[38] = 14;

    // Bottom arm going up (cells 39 to 43)
    boardPathX[39] = 6; boardPathY[39] = 13; // Blue Start Cell
    boardPathX[40] = 6; boardPathY[40] = 12;
    boardPathX[41] = 6; boardPathY[41] = 11;
    boardPathX[42] = 6; boardPathY[42] = 10;
    boardPathX[43] = 6; boardPathY[43] = 9;

    // Left arm going left (cells 44 to 49)
    boardPathX[44] = 5; boardPathY[44] = 8;
    boardPathX[45] = 4; boardPathY[45] = 8;
    boardPathX[46] = 3; boardPathY[46] = 8;
    boardPathX[47] = 2; boardPathY[47] = 8;  // Star / Safe Cell
    boardPathX[48] = 1; boardPathY[48] = 8;
    boardPathX[49] = 0; boardPathY[49] = 8;

    // Left edge transition (cells 50, 51)
    boardPathX[50] = 0; boardPathY[50] = 7;
    boardPathX[51] = 0; boardPathY[51] = 6;

    // 3. Mark the 8 safe cells (4 Start cells + 4 Star cells)
    safeCells[0]  = true; // Red Start
    safeCells[13] = true; // Green Start
    safeCells[26] = true; // Yellow Start
    safeCells[39] = true; // Blue Start

    safeCells[8]  = true; // Red Star
    safeCells[21] = true; // Green Star
    safeCells[34] = true; // Yellow Star
    safeCells[47] = true; // Blue Star

    // 4. Define the 4 colored Home Corridors (5 cells each)
    // Red corridor: moving right on row 7
    for (int i = 0; i < 5; i++) {
        corridorX[0][i] = 1 + i;
        corridorY[0][i] = 7;
    }
    // Green corridor: moving down on col 7
    for (int i = 0; i < 5; i++) {
        corridorX[1][i] = 7;
        corridorY[1][i] = 1 + i;
    }
    // Yellow corridor: moving left on row 7
    for (int i = 0; i < 5; i++) {
        corridorX[2][i] = 13 - i;
        corridorY[2][i] = 7;
    }
    // Blue corridor: moving up on col 7
    for (int i = 0; i < 5; i++) {
        corridorX[3][i] = 7;
        corridorY[3][i] = 13 - i;
    }
}

// Returns the pixel center of a player's token slot inside their home base
Vector2f getHomePosition(int playerIndex, int tokenId) {
    float baseX = 0, baseY = 0;
    switch (playerIndex) {
        case 0: // Red: top-left (0..5, 0..5)
            baseX = BOARD_X;
            baseY = BOARD_Y;
            break;
        case 1: // Green: top-right (9..14, 0..5)
            baseX = BOARD_X + 9 * CELL_SIZE;
            baseY = BOARD_Y;
            break;
void drawBoard(RenderWindow &window) {}
