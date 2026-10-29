/*
 * LUDO GAME - BOARD IMPLEMENTATION (board.cpp)
 * ============================================
 * This file handles the 15x15 grid layout, coordinate tables, and all drawing code.
 * 
 * BOARD LAYOUT OVERVIEW:
 * 1. 15x15 GRID: The whole board is a grid of 15 x 15 squares (each 40x40 pixels).
 * 2. FOUR BASES (6x6 cells each):
 *    - Red: top-left (0..5, 0..5)
 *    - Green: top-right (9..14, 0..5)
 *    - Yellow: bottom-right (9..14, 9..14)
 *    - Blue: bottom-left (0..5, 9..14)
 * 3. FOUR ARMS (6x3 cells each):
 *    - Contains the 52 white walking path cells.
 *    - Includes 4 Start cells ('S') and 4 Star safe cells ('*').
 *    - Contains the 4 colored corridors (5 cells each).
 * 4. CENTER FINISH (3x3 cells in middle):
 *    - 4 colored triangles meeting at the golden center circle.
 * 
 * CORE DATA STRUCTURES MANAGED IN THIS FILE:
 * 1. 1D ARRAYS:
 *    - `boardPathX[52]` & `boardPathY[52]`: Stores (X,Y) grid coordinates for the 52-cell track.
 *    - `safeCells[52]`: Boolean array marking which cells are safe from capture.
 * 2. 2D ARRAYS (Matrices):
 *    - `corridorX[4][5]` & `corridorY[4][5]`: Stores coordinates for 5 corridor cells for 4 players.
 * 3. RENDERING THE DATA STRUCTURES ON SCREEN (in drawUI):
 *    - QUEUE: Displays `diceHistory` as a sequence of recent dice numbers.
 *    - STACK: Displays `moveHistory.size()` on the Undo button.
 *    - LINKED LIST: Displays the last 4 event messages from `actionLog`.
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
        case 2: // Yellow: bottom-right (9..14, 9..14)
            baseX = BOARD_X + 9 * CELL_SIZE;
            baseY = BOARD_Y + 9 * CELL_SIZE;
            break;
        case 3: // Blue: bottom-left (0..5, 9..14)
            baseX = BOARD_X;
            baseY = BOARD_Y + 9 * CELL_SIZE;
            break;
    }

    // Inside the 6x6 base, the white box is at offset (40, 40) with size (160, 160)
    // 4 token slots are placed at (80, 80), (160, 80), (80, 160), (160, 160)
    float col = (tokenId % 2 == 0) ? 75.0f : 165.0f;
    float row = (tokenId < 2)      ? 75.0f : 165.0f;
    return Vector2f(baseX + col, baseY + row);
}

// Returns the finish spot inside the central triangle for each player
Vector2f getFinishPosition(int playerIndex, int tokenId) {
    float cx = BOARD_X + 7.5f * CELL_SIZE;
    float cy = BOARD_Y + 7.5f * CELL_SIZE;
    float shift = (tokenId - 1.5f) * 8.0f;

    switch (playerIndex) {
        case 0: return Vector2f(cx - 35.0f, cy + shift); // Red (left triangle)
        case 1: return Vector2f(cx + shift, cy - 35.0f); // Green (top triangle)
        case 2: return Vector2f(cx + 35.0f, cy + shift); // Yellow (right triangle)
        case 3: return Vector2f(cx + shift, cy + 35.0f); // Blue (bottom triangle)
        default: return Vector2f(cx, cy);
    }
}

// Calculates the exact pixel position of any token based on its `step`
Vector2f getTokenPixelPosition(int playerIndex, int tokenId) {
    const Token &tok = players[playerIndex].tokens[tokenId];

    // Case 1: In Home Base
    if (tok.step == 0) {
        return getHomePosition(playerIndex, tokenId);
    }
    // Case 2: On Main 52-cell track
    if (tok.step >= 1 && tok.step <= 51) {
        int startCell = playerIndex * 13;
        int cellIndex = (startCell + (tok.step - 1)) % TOTAL_PATH_CELLS;
        return getGridPixel(boardPathX[cellIndex], boardPathY[cellIndex]);
    }
    // Case 3: In Home Corridor (52 to 56)
    if (tok.step >= 52 && tok.step <= 56) {
        int corridorIndex = tok.step - 52;
        return getGridPixel(corridorX[playerIndex][corridorIndex], corridorY[playerIndex][corridorIndex]);
    }
    // Case 4: Reached Finish (57)
    return getFinishPosition(playerIndex, tokenId);
}

// Draws the complete 15x15 Ludo board
void drawBoard(RenderWindow &window) {
    // 1. Overall Window Background
    RectangleShape windowBg(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    windowBg.setFillColor(Color(245, 247, 250));
    window.draw(windowBg);

    // 2. Board Background Card with shadow effect
    RectangleShape boardShadow(Vector2f(BOARD_SIZE + 8, BOARD_SIZE + 8));
    boardShadow.setPosition(Vector2f(BOARD_X - 4, BOARD_Y - 4));
    boardShadow.setFillColor(Color(200, 205, 215));
    window.draw(boardShadow);

    RectangleShape boardBg(Vector2f(BOARD_SIZE, BOARD_SIZE));
    boardBg.setPosition(Vector2f(BOARD_X, BOARD_Y));
    boardBg.setFillColor(Color::White);
    boardBg.setOutlineColor(Color(30, 30, 30));
    boardBg.setOutlineThickness(2.0f);
    window.draw(boardBg);

    // 3. Draw the 4 Corner Home Bases (6x6 cells each = 240x240 px)
    struct BaseConfig {
        int gx, gy;
        Color color;
    } bases[4] = {
        {0, 0, players[0].color},  // Red: top-left
        {9, 0, players[1].color},  // Green: top-right
        {9, 9, players[2].color},  // Yellow: bottom-right
        {0, 9, players[3].color}   // Blue: bottom-left
    };

    for (int p = 0; p < 4; p++) {
        float bx = BOARD_X + bases[p].gx * CELL_SIZE;
        float by = BOARD_Y + bases[p].gy * CELL_SIZE;
        float baseDim = 6 * CELL_SIZE;

        // Colored Base Box
        RectangleShape outerBox(Vector2f(baseDim, baseDim));
        outerBox.setPosition(Vector2f(bx, by));
        outerBox.setFillColor(bases[p].color);
        outerBox.setOutlineColor(Color(30, 30, 30));
        outerBox.setOutlineThickness(1.5f);
        window.draw(outerBox);

        // Inner White Box
        RectangleShape innerWhite(Vector2f(baseDim - 80, baseDim - 80));
        innerWhite.setPosition(Vector2f(bx + 40, by + 40));
        innerWhite.setFillColor(Color::White);
        innerWhite.setOutlineColor(Color(30, 30, 30));
        innerWhite.setOutlineThickness(1.5f);
        window.draw(innerWhite);

        // 4 Token circles inside home
        for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
            Vector2f slotPos = getHomePosition(p, t);
            CircleShape slot(TOKEN_RADIUS + 5);
            slot.setOrigin(Vector2f(TOKEN_RADIUS + 5, TOKEN_RADIUS + 5));
            slot.setPosition(slotPos);
            slot.setFillColor(bases[p].color);
            slot.setOutlineColor(Color(30, 30, 30));
            slot.setOutlineThickness(1.5f);
            window.draw(slot);

            CircleShape innerSlot(TOKEN_RADIUS - 1);
            innerSlot.setOrigin(Vector2f(TOKEN_RADIUS - 1, TOKEN_RADIUS - 1));
            innerSlot.setPosition(slotPos);
            innerSlot.setFillColor(Color::White);
            window.draw(innerSlot);
        }
    }

    // 4. Draw Path Cells (52 main loop cells)
    for (int i = 0; i < TOTAL_PATH_CELLS; i++) {
        RectangleShape cell(Vector2f(CELL_SIZE, CELL_SIZE));
        cell.setPosition(Vector2f(BOARD_X + boardPathX[i] * CELL_SIZE, BOARD_Y + boardPathY[i] * CELL_SIZE));
        cell.setOutlineColor(Color(80, 80, 80));
        cell.setOutlineThickness(1.0f);

        // Color specific cells
        if (i == 0) {
            cell.setFillColor(players[0].color); // Red Start
        } else if (i == 13) {
            cell.setFillColor(players[1].color); // Green Start
        } else if (i == 26) {
            cell.setFillColor(players[2].color); // Yellow Start
        } else if (i == 39) {
            cell.setFillColor(players[3].color); // Blue Start
        } else if (safeCells[i]) {
            cell.setFillColor(Color(230, 235, 245)); // Safe Star Cell
        } else {
            cell.setFillColor(Color::White);
        }
        window.draw(cell);

        // Draw Star on Safe cells
        if (safeCells[i] && i != 0 && i != 13 && i != 26 && i != 39) {
            Text star(gameFont, "*", 26);
            star.setStyle(Text::Bold);
            star.setFillColor(Color(50, 50, 50));
            star.setOrigin(Vector2f(6.0f, 12.0f));
            Vector2f cp = getGridPixel(boardPathX[i], boardPathY[i]);
            star.setPosition(Vector2f(cp.x, cp.y + 4.0f));
            window.draw(star);
        }
        // Draw Start indicator on Start cells
        if (i == 0 || i == 13 || i == 26 || i == 39) {
            Text startIcon(gameFont, "S", 18);
            startIcon.setStyle(Text::Bold);
            startIcon.setFillColor(Color::White);
            startIcon.setOrigin(Vector2f(6.0f, 12.0f));
            Vector2f cp = getGridPixel(boardPathX[i], boardPathY[i]);
            startIcon.setPosition(Vector2f(cp.x, cp.y - 1.0f));
            window.draw(startIcon);
        }
    }

    // 5. Draw the 4 Colored Home Corridors (5 cells each)
    for (int p = 0; p < 4; p++) {
        for (int c = 0; c < HOME_CORRIDOR_CELLS; c++) {
            RectangleShape cell(Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setPosition(Vector2f(BOARD_X + corridorX[p][c] * CELL_SIZE,
                                      BOARD_Y + corridorY[p][c] * CELL_SIZE));
            cell.setFillColor(players[p].color);
            cell.setOutlineColor(Color(80, 80, 80));
            cell.setOutlineThickness(1.0f);
            window.draw(cell);
        }
    }

    // 6. Draw Center Finish Area (3x3 grid = cols 6..8, rows 6..8)
    float cx = BOARD_X + 7.5f * CELL_SIZE;
    float cy = BOARD_Y + 7.5f * CELL_SIZE;
    Vector2f centerPoint(cx, cy);

    Vector2f topLeft(BOARD_X + 6 * CELL_SIZE, BOARD_Y + 6 * CELL_SIZE);
    Vector2f topRight(BOARD_X + 9 * CELL_SIZE, BOARD_Y + 6 * CELL_SIZE);
    Vector2f bottomRight(BOARD_X + 9 * CELL_SIZE, BOARD_Y + 9 * CELL_SIZE);
    Vector2f bottomLeft(BOARD_X + 6 * CELL_SIZE, BOARD_Y + 9 * CELL_SIZE);

    // Left Triangle: Red
    ConvexShape redTri(3);
    redTri.setPoint(0, topLeft);
    redTri.setPoint(1, bottomLeft);
    redTri.setPoint(2, centerPoint);
    redTri.setFillColor(players[0].color);
    redTri.setOutlineColor(Color(30, 30, 30));
    redTri.setOutlineThickness(1.0f);
    window.draw(redTri);

    // Top Triangle: Green
    ConvexShape greenTri(3);
    greenTri.setPoint(0, topLeft);
    greenTri.setPoint(1, topRight);
    greenTri.setPoint(2, centerPoint);
    greenTri.setFillColor(players[1].color);
    greenTri.setOutlineColor(Color(30, 30, 30));
    greenTri.setOutlineThickness(1.0f);
    window.draw(greenTri);

    // Right Triangle: Yellow
    ConvexShape yellowTri(3);
    yellowTri.setPoint(0, topRight);
    yellowTri.setPoint(1, bottomRight);
    yellowTri.setPoint(2, centerPoint);
    yellowTri.setFillColor(players[2].color);
    yellowTri.setOutlineColor(Color(30, 30, 30));
    yellowTri.setOutlineThickness(1.0f);
    window.draw(yellowTri);

    // Bottom Triangle: Blue
    ConvexShape blueTri(3);
    blueTri.setPoint(0, bottomLeft);
    blueTri.setPoint(1, bottomRight);
    blueTri.setPoint(2, centerPoint);
    blueTri.setFillColor(players[3].color);
    blueTri.setOutlineColor(Color(30, 30, 30));
    blueTri.setOutlineThickness(1.0f);
    window.draw(blueTri);

    // Center Gold Circle
    CircleShape centerCircle(14.0f);
    centerCircle.setOrigin(Vector2f(14.0f, 14.0f));
    centerCircle.setPosition(centerPoint);
    centerCircle.setFillColor(Color(255, 215, 0));
    centerCircle.setOutlineColor(Color(30, 30, 30));
    centerCircle.setOutlineThickness(2.0f);
    window.draw(centerCircle);
}

// Draws all tokens with player color, token number, and highlights
void drawTokens(RenderWindow &window) {
    for (int p = 0; p < numPlayers; p++) {
        for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
            Vector2f pos = getTokenPixelPosition(p, t);

            // If on main path or safe cell, offset slightly if sharing cell with other active tokens
            if (players[p].tokens[t].step >= 1 && players[p].tokens[t].step <= 51) {
                int sharedCount = 0;
                int myIndex = 0;
                for (int op = 0; op < numPlayers; op++) {
                    for (int ot = 0; ot < TOKENS_PER_PLAYER; ot++) {
                        if (players[op].tokens[ot].status == 1 &&
                            players[op].tokens[ot].step >= 1 &&
                            players[op].tokens[ot].step <= 51) {
                            int myCell = (p * 13 + (players[p].tokens[t].step - 1)) % TOTAL_PATH_CELLS;
                            int otherCell = (op * 13 + (players[op].tokens[ot].step - 1)) % TOTAL_PATH_CELLS;
                            if (myCell == otherCell) {
                                if (op < p || (op == p && ot < t)) myIndex++;
                                sharedCount++;
                            }
                        }
                    }
                }
                if (sharedCount > 1) {
                    float offsetAngle = (myIndex * 2.0f * 3.14159f) / sharedCount;
                    pos.x += cos(offsetAngle) * 8.0f;
                    pos.y += sin(offsetAngle) * 8.0f;
                }
            }

            // Highlight ring if this token can be moved right now
            if (!isRolling && !gameOver && p == currentPlayerIndex && canMoveToken(p, t)) {
                CircleShape glow(TOKEN_RADIUS + 6.0f);
                glow.setOrigin(Vector2f(TOKEN_RADIUS + 6.0f, TOKEN_RADIUS + 6.0f));
                glow.setPosition(pos);
                glow.setFillColor(Color(255, 230, 80, 160));
                glow.setOutlineColor(Color(255, 215, 0));
                glow.setOutlineThickness(2.5f);
                window.draw(glow);
            }

            // Outer dark outline
            CircleShape tokenBorder(TOKEN_RADIUS + 1.5f);
            tokenBorder.setOrigin(Vector2f(TOKEN_RADIUS + 1.5f, TOKEN_RADIUS + 1.5f));
            tokenBorder.setPosition(pos);
            tokenBorder.setFillColor(Color(30, 30, 30));
            window.draw(tokenBorder);

            // Main token circle
            CircleShape token(TOKEN_RADIUS);
            token.setOrigin(Vector2f(TOKEN_RADIUS, TOKEN_RADIUS));
            token.setPosition(pos);
            token.setFillColor(players[p].color);
            window.draw(token);

            // Inner highlight ring
            CircleShape innerRing(TOKEN_RADIUS - 3.5f);
            innerRing.setOrigin(Vector2f(TOKEN_RADIUS - 3.5f, TOKEN_RADIUS - 3.5f));
            innerRing.setPosition(pos);
            innerRing.setFillColor(players[p].color);
            innerRing.setOutlineColor(Color(255, 255, 255, 120));
            innerRing.setOutlineThickness(1.0f);
            window.draw(innerRing);

            // Token Number (1 to 4)
            Text numText(gameFont, to_string(t + 1), 13);
            numText.setStyle(Text::Bold);
            numText.setFillColor(Color::White);
            numText.setOrigin(Vector2f(4.0f, 9.0f));
            numText.setPosition(pos);
            window.draw(numText);
        }
    }
}

// Draws the 3D-styled dice with accurate dots
void drawDice(RenderWindow &window) {
    const float diceX = 760.0f;
    const float diceY = 100.0f;
    const float diceSize = 60.0f;

    // Dice shadow
    RectangleShape shadow(Vector2f(diceSize + 4, diceSize + 4));
    shadow.setPosition(Vector2f(diceX - 2, diceY + 3));
    shadow.setFillColor(Color(200, 205, 215));
    window.draw(shadow);

    // Dice body
    RectangleShape dice(Vector2f(diceSize, diceSize));
    dice.setPosition(Vector2f(diceX, diceY));
    dice.setFillColor(Color::White);
    dice.setOutlineColor(Color(40, 40, 40));
    dice.setOutlineThickness(2.0f);
    window.draw(dice);

    // Helper to draw a single black dot
    auto drawDot = [&](float dx, float dy) {
        CircleShape dot(4.0f);
        dot.setOrigin(Vector2f(4.0f, 4.0f));
        dot.setPosition(Vector2f(diceX + dx, diceY + dy));
        dot.setFillColor(Color(30, 30, 30));
        window.draw(dot);
    };

    float mid = diceSize / 2.0f;
    float p1 = 15.0f;
    float p2 = diceSize - 15.0f;

    // Dot patterns for 1 to 6
    if (currentDiceValue == 1 || currentDiceValue == 3 || currentDiceValue == 5) {
        drawDot(mid, mid); // Center dot
    }
    if (currentDiceValue >= 2) {
        drawDot(p1, p1);
        drawDot(p2, p2);
    }
    if (currentDiceValue >= 4) {
        drawDot(p2, p1);
        drawDot(p1, p2);
    }
    if (currentDiceValue == 6) {
        drawDot(p1, mid);
        drawDot(p2, mid);
    }
}

// Draws the clean, modern sidebar UI displaying QUEUE, STACK, and LINKED LIST
void drawUI(RenderWindow &window) {
    const float panelX = 640.0f;
    const float panelY = 50.0f;
    const float panelW = 300.0f;
    const float panelH = 600.0f;

    // Sidebar Card Background
    RectangleShape panel(Vector2f(panelW, panelH));
    panel.setPosition(Vector2f(panelX, panelY));
    panel.setFillColor(Color::White);
    panel.setOutlineColor(Color(215, 220, 230));
    panel.setOutlineThickness(2.0f);
    window.draw(panel);

    // 1. Current Player Turn Header Card
    RectangleShape turnCard(Vector2f(panelW - 20, 38.0f));
    turnCard.setPosition(Vector2f(panelX + 10, panelY + 10));
    turnCard.setFillColor(players[currentPlayerIndex].color);
    window.draw(turnCard);

    Text turnText(gameFont, players[currentPlayerIndex].name + "'s Turn", 15);
    turnText.setStyle(Text::Bold);
    turnText.setFillColor(Color::White);
    turnText.setPosition(Vector2f(panelX + 20, panelY + 20));
    window.draw(turnText);

    // 2. Dice Roll Prompt / Badge
    if (isRolling) {
        RectangleShape rollBtn(Vector2f(panelW - 60, 26.0f));
        rollBtn.setPosition(Vector2f(panelX + 30, panelY + 118));
        rollBtn.setFillColor(Color(40, 165, 80));
        window.draw(rollBtn);

        Text rollText(gameFont, "ROLL DICE (SPACE / Click)", 12);
        rollText.setStyle(Text::Bold);
        rollText.setFillColor(Color::White);
        rollText.setPosition(Vector2f(panelX + 50, panelY + 123));
        window.draw(rollText);
    } else {
        Text rollResult(gameFont, "DICE: " + to_string(currentDiceValue), 15);
        rollResult.setStyle(Text::Bold);
        rollResult.setFillColor(Color(40, 40, 40));
        rollResult.setPosition(Vector2f(panelX + 115, panelY + 120));
        window.draw(rollResult);
    }

    // 3. DATA STRUCTURE #1: QUEUE (FIFO) Display
    // Shows recent dice rolls stored in CustomQueue diceHistory
    Text queueLabel(gameFont, "Dice History (Custom Queue - FIFO):", 11);
    queueLabel.setStyle(Text::Bold);
    queueLabel.setFillColor(Color(80, 90, 105));
    queueLabel.setPosition(Vector2f(panelX + 15, panelY + 150));
    window.draw(queueLabel);

    vector<int> qVals = diceHistory.toVector();
    for (size_t qIndex = 0; qIndex < qVals.size(); qIndex++) {
        int val = qVals[qIndex];

        RectangleShape qBox(Vector2f(22.0f, 20.0f));
        qBox.setPosition(Vector2f(panelX + 15 + qIndex * 28.0f, panelY + 168));
        qBox.setFillColor(Color(240, 244, 250));
        qBox.setOutlineColor(Color(180, 190, 205));
        qBox.setOutlineThickness(1.0f);
        window.draw(qBox);

        Text qText(gameFont, to_string(val), 11);
        qText.setStyle(Text::Bold);
        qText.setFillColor(Color(40, 50, 70));
        qText.setPosition(Vector2f(panelX + 21 + qIndex * 28.0f, panelY + 171));
        window.draw(qText);

        if (qIndex + 1 < qVals.size()) {
            Text arrow(gameFont, ">", 11);
            arrow.setFillColor(Color(150, 160, 175));
            arrow.setPosition(Vector2f(panelX + 39 + qIndex * 28.0f, panelY + 171));
            window.draw(arrow);
        }
    }

    // 4. Status / Instruction Message
    Text status(gameFont, statusMessage, 11);
    status.setFillColor(Color(40, 40, 50));
    status.setPosition(Vector2f(panelX + 15, panelY + 196));
    window.draw(status);

    // 5. Token Selection Buttons [1], [2], [3], [4]
    Text chooseTitle(gameFont, "Select Token to Move (1-4):", 11);
    chooseTitle.setStyle(Text::Bold);
    chooseTitle.setFillColor(Color(50, 50, 60));
    chooseTitle.setPosition(Vector2f(panelX + 15, panelY + 224));
    window.draw(chooseTitle);

    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        float btnY = panelY + 242.0f + t * 30.0f;
        RectangleShape btn(Vector2f(panelW - 30, 26.0f));
        btn.setPosition(Vector2f(panelX + 15, btnY));

        bool canMove = (!isRolling && !gameOver && canMoveToken(currentPlayerIndex, t));

        if (canMove) {
            btn.setFillColor(players[currentPlayerIndex].color);
            btn.setOutlineColor(Color(255, 215, 0));
            btn.setOutlineThickness(1.5f);
        } else {
            btn.setFillColor(Color(245, 247, 250));
            btn.setOutlineColor(Color(225, 230, 235));
            btn.setOutlineThickness(1.0f);
        }
        window.draw(btn);

        string tokenDesc = "Token " + to_string(t + 1) + ": ";
        const Token &tok = players[currentPlayerIndex].tokens[t];
        if (tok.status == 0) {
            tokenDesc += (currentDiceValue == 6 && !isRolling) ? "Exit Home (Ready!)" : "In Base";
        } else if (tok.status == 2) {
            tokenDesc += "Finished!";
        } else {
            tokenDesc += "Step " + to_string(tok.step) + "/57";
            if (canMove) tokenDesc += " (Can Move)";
        }

        Text btnText(gameFont, "[" + to_string(t + 1) + "] " + tokenDesc, 11);
        btnText.setStyle(Text::Bold);
        btnText.setFillColor(canMove ? Color::White : Color(110, 115, 125));
        btnText.setPosition(Vector2f(panelX + 22, btnY + 6));
        window.draw(btnText);
    }

    // 6. DATA STRUCTURE #2: STACK (LIFO) - UNDO BUTTON
    float undoY = panelY + 368.0f;
    RectangleShape undoBtn(Vector2f(panelW - 30, 26.0f));
    undoBtn.setPosition(Vector2f(panelX + 15, undoY));
    bool hasUndo = !moveHistory.empty();
    if (hasUndo) {
        undoBtn.setFillColor(Color(240, 130, 30));
        undoBtn.setOutlineColor(Color(200, 100, 20));
        undoBtn.setOutlineThickness(1.0f);
    } else {
        undoBtn.setFillColor(Color(240, 242, 245));
        undoBtn.setOutlineColor(Color(220, 225, 230));
        undoBtn.setOutlineThickness(1.0f);
    }
    window.draw(undoBtn);

    string undoLabel = "[U] Undo Move (Stack: " + to_string(moveHistory.size()) + " moves)";
    Text undoText(gameFont, undoLabel, 11);
    undoText.setStyle(Text::Bold);
    undoText.setFillColor(hasUndo ? Color::White : Color(140, 145, 155));
    undoText.setPosition(Vector2f(panelX + 25, undoY + 6));
    window.draw(undoText);

    // 7. DATA STRUCTURE #3: CUSTOM LINKED LIST - RECENT EVENT LOG
    float logY = panelY + 402.0f;
    Text logTitle(gameFont, "Game Events (Custom Linked List):", 11);
    logTitle.setStyle(Text::Bold);
    logTitle.setFillColor(Color(80, 90, 105));
    logTitle.setPosition(Vector2f(panelX + 15, logY));
    window.draw(logTitle);

    vector<string> logs = actionLog.toVector();
    int logIndex = 0;
    for (const string &msg : logs) {
        Text logText(gameFont, "* " + msg, 10);
        logText.setFillColor(Color(70, 75, 85));
        logText.setPosition(Vector2f(panelX + 18, logY + 16.0f + logIndex * 15.0f));
        window.draw(logText);
        logIndex++;
    }

    // 8. Finished Tokens Progress
    float scoreY = panelY + 478.0f;
    RectangleShape line(Vector2f(panelW - 30, 1.0f));
    line.setPosition(Vector2f(panelX + 15, scoreY));
    line.setFillColor(Color(225, 230, 235));
    window.draw(line);

    Text scoreTitle(gameFont, "Tokens Finished:", 11);
    scoreTitle.setStyle(Text::Bold);
    scoreTitle.setFillColor(Color(50, 50, 50));
    scoreTitle.setPosition(Vector2f(panelX + 15, scoreY + 5));
    window.draw(scoreTitle);

    for (int p = 0; p < numPlayers; p++) {
        float px = panelX + 15 + (p % 2) * 140.0f;
        float py = scoreY + 22.0f + (p / 2) * 16.0f;
        CircleShape pDot(3.5f);
        pDot.setPosition(Vector2f(px, py + 3));
        pDot.setFillColor(players[p].color);
        window.draw(pDot);

        string pScore = players[p].name.substr(0, 8) + ": " + to_string(players[p].tokensFinished) + "/4";
        Text pText(gameFont, pScore, 10);
        pText.setFillColor(Color(60, 60, 60));
        pText.setPosition(Vector2f(px + 10, py));
        window.draw(pText);
    }

    // 9. Keyboard Controls Help Footer
    float helpY = panelY + 540.0f;
    RectangleShape line2(Vector2f(panelW - 30, 1.0f));
    line2.setPosition(Vector2f(panelX + 15, helpY));
    line2.setFillColor(Color(225, 230, 235));
    window.draw(line2);

    Text helpText(gameFont, "SPACE = Roll | 1-4 = Move | U = Undo\nS = Save | L = Load | R = Restart | ESC = Quit", 10);
    helpText.setFillColor(Color(120, 125, 135));
    helpText.setPosition(Vector2f(panelX + 15, helpY + 8));
    window.draw(helpText);
}

// Draws the welcome start screen
void drawStartScreen(RenderWindow &window) {
    RectangleShape bg(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bg.setFillColor(Color(40, 50, 75));
    window.draw(bg);

    // Decorative Ludo board silhouette in center
    RectangleShape centerBox(Vector2f(340, 340));
    centerBox.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 170, 140));
    centerBox.setFillColor(Color(50, 65, 95));
    centerBox.setOutlineColor(Color(255, 215, 0));
    centerBox.setOutlineThickness(3.0f);
    window.draw(centerBox);

    Text title(gameFont, "LUDO GAME", 44);
    title.setStyle(Text::Bold);
    title.setFillColor(Color(255, 215, 0));
    title.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 145, 170));
    window.draw(title);

    Text subtitle(gameFont, "Classic 4-Player Board Game", 18);
    subtitle.setFillColor(Color(200, 220, 255));
    subtitle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 130, 235));
    window.draw(subtitle);

    // Start Button
    RectangleShape startBtn(Vector2f(220, 48));
    startBtn.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 110, 310));
    startBtn.setFillColor(Color(45, 180, 85));
    startBtn.setOutlineColor(Color::White);
    startBtn.setOutlineThickness(2.0f);
    window.draw(startBtn);

    Text startText(gameFont, "PRESS ENTER TO PLAY", 15);
    startText.setStyle(Text::Bold);
    startText.setFillColor(Color::White);
    startText.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 95, 324));
    window.draw(startText);

    // Rules summary
    Text rules(gameFont, "* Roll 6 to bring token out of base\n* Safe cells (*) protect tokens from capture\n* First player with all 4 tokens home wins!", 13);
    rules.setFillColor(Color(210, 225, 240));
    rules.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 145, 380));
    window.draw(rules);

    Text footer(gameFont, "Developed in C++ using SFML", 13);
    footer.setFillColor(Color(150, 170, 200));
    footer.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 95, 520));
    window.draw(footer);
}

// Draws the victory screen overlay
void drawWinScreen(RenderWindow &window) {
    RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(Color(15, 20, 30, 215));
    window.draw(overlay);

    RectangleShape dialog(Vector2f(440, 260));
    dialog.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 220, WINDOW_HEIGHT / 2.0f - 130));
    dialog.setFillColor(Color::White);
    dialog.setOutlineColor(players[winnerIndex].color);
    dialog.setOutlineThickness(4.0f);
    window.draw(dialog);

    Text winTitle(gameFont, "WINNER!", 36);
    winTitle.setStyle(Text::Bold);
    winTitle.setFillColor(Color(255, 195, 0));
    winTitle.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 85, WINDOW_HEIGHT / 2.0f - 105));
    window.draw(winTitle);

    Text winText(gameFont, players[winnerIndex].name + " WINS!", 22);
    winText.setStyle(Text::Bold);
    winText.setFillColor(players[winnerIndex].color);
    winText.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 120, WINDOW_HEIGHT / 2.0f - 50));
    window.draw(winText);

    Text subText(gameFont, "All 4 tokens reached home safely!", 14);
    subText.setFillColor(Color(70, 75, 85));
    subText.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 110, WINDOW_HEIGHT / 2.0f - 10));
    window.draw(subText);

    Text restart(gameFont, "Press R to Play Again   |   Press ESC to Quit", 14);
    restart.setStyle(Text::Bold);
    restart.setFillColor(Color(40, 140, 220));
    restart.setPosition(Vector2f(WINDOW_WIDTH / 2.0f - 160, WINDOW_HEIGHT / 2.0f + 45));
    window.draw(restart);
}
