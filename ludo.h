/*
 * LUDO GAME - HEADER FILE (ludo.h)
 * ================================
 * This file contains ALL the shared things: constants, data structures (structs),
 * global variables, and function declarations.
 * 
 * Think of this as the "menu" that tells other files what's available.
 * 
 * CORE DATA STRUCTURES USED (FOR VIVA/MAM):
 * 1. QUEUE (FIFO)  -> std::queue<int> diceHistory
 *    Stores last 5 dice rolls. New roll added at back, oldest removed from front.
 * 
 * 2. STACK (LIFO)  -> std::stack<MoveRecord> moveHistory
 *    Stores every move for UNDO feature. Last move pushed on top, popped to undo.
 * 
 * 3. LINKED LIST   -> std::list<string> actionLog
 *    Stores game event messages. New events added at back, old ones removed from front.
 * 
 * 4. ARRAYS        -> 1D arrays (boardPathX[52], boardPathY[52], safeCells[52])
 *                    2D arrays (corridorX[4][5], corridorY[4][5])
 *    Store board coordinates for fast O(1) lookup.
 * 
 * 5. STRUCTS       -> Token, Player, MoveRecord
 *    Group related data together.
 */

#ifndef LUDO_H
#define LUDO_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <queue>    // For QUEUE (FIFO) - dice history
#include <stack>    // For STACK (LIFO) - move history for UNDO
#include <list>     // For LINKED LIST - game event log
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;
using namespace sf;

// ========================== CONSTANTS ==========================
// These are fixed values that never change during the game

const int MAX_PLAYERS = 4;           // 4 players: Red, Green, Yellow, Blue
const int TOKENS_PER_PLAYER = 4;     // Each player has 4 tokens
const int TOTAL_PATH_CELLS = 52;     // 52 cells in the main circular track
const int HOME_CORRIDOR_CELLS = 5;   // 5 cells in each player's private corridor
const int FINISH_STEP = 57;          // Step 57 = token reached center finish!

// Window and Board sizes
const float WINDOW_WIDTH = 960.0f;
const float WINDOW_HEIGHT = 700.0f;
const float BOARD_SIZE = 600.0f;     // 15 cells * 40 pixels = 600
const float CELL_SIZE = 40.0f;       // Each grid cell is 40x40 pixels
const float BOARD_X = 20.0f;         // Board starts at x=20
const float BOARD_Y = 50.0f;         // Board starts at y=50
const float TOKEN_RADIUS = 13.0f;    // Token circle radius


// ========================== DATA STRUCTURES (STRUCTS) ==========================

/*
 * Token struct - represents ONE token on the board
 * 
 * step: 0 = in home base, 1-51 = on main track, 52-56 = in corridor, 57 = finished
 * status: 0 = HOME, 1 = ACTIVE (on board), 2 = FINISHED
 */
struct Token {
    int id;         // Token ID: 0, 1, 2, 3
    int step;       // Current progress (0 to 57)
    int status;     // 0=HOME, 1=ACTIVE, 2=FINISHED
};

/*
 * Player struct - represents ONE player
 * Contains 4 tokens and tracks how many finished
 */
struct Player {
    string name;                     // e.g., "Player 1 (Red)"
    Color color;                     // Player's color
    Token tokens[TOKENS_PER_PLAYER]; // Array of 4 tokens
    int tokensFinished;              // How many tokens reached finish (0 to 4)
};

/*
 * MoveRecord struct - stores ONE move for the UNDO feature (STACK)
 * 
 * Every time a player moves a token, we save ALL the info needed to undo it.
 * This is pushed onto the stack (LIFO - Last In First Out).
 */
struct MoveRecord {
    int playerIndex;       // Which player moved (0 to 3)
    int tokenId;           // Which token (0 to 3)
    int prevStep;          // Token's step BEFORE the move
    int prevStatus;        // Token's status BEFORE the move
    int diceValue;         // What dice was rolled
    bool wasRolling;       // Was it rolling phase before?
    bool captured;         // Did this move capture someone?
    int capturedPlayer;    // If captured, which player?
    int capturedTokenId;   // If captured, which token?
    int capturedPrevStep;  // Captured token's step before capture
};


// ========================== GLOBAL VARIABLES ==========================
// These are defined in game.cpp but declared here so all files can use them
// 'extern' means "this variable exists somewhere else, don't create a new one"

extern Font gameFont;

// Game state variables
extern int currentPlayerIndex;   // Whose turn: 0=Red, 1=Green, 2=Yellow, 3=Blue
extern int numPlayers;           // Number of players (usually 4)
extern bool isRolling;           // true = waiting for dice roll, false = choose token
extern bool gameOver;            // true when someone wins
extern int winnerIndex;          // Who won (0 to 3)
extern int currentDiceValue;     // Current dice value (1 to 6)
extern bool canRollAgain;        // true if player gets extra roll (6, capture, finish)
extern string statusMessage;     // Text shown on sidebar

extern Player players[MAX_PLAYERS];  // Array of 4 players

// ========================== CORE DATA STRUCTURES ==========================
// THESE ARE THE 3 MAIN DATA STRUCTURES FOR YOUR VIVA!

// 1. QUEUE (FIFO) - First In First Out
// Stores last 5 dice rolls. New rolls go to BACK, old rolls removed from FRONT.
extern queue<int> diceHistory;

// 2. STACK (LIFO) - Last In First Out
// Stores move history for UNDO. New moves pushed on TOP, popped from TOP to undo.
extern stack<MoveRecord> moveHistory;

// 3. LINKED LIST - Dynamic list, efficient add/remove at both ends
// Stores recent game event messages (max 4 shown on screen)
extern list<string> actionLog;


// ========================== BOARD COORDINATE ARRAYS ==========================
// These store the (x,y) grid positions for fast lookup

extern int boardPathX[TOTAL_PATH_CELLS];           // X grid position for each of 52 path cells
extern int boardPathY[TOTAL_PATH_CELLS];           // Y grid position for each of 52 path cells
extern int corridorX[MAX_PLAYERS][HOME_CORRIDOR_CELLS]; // X grid for each player's 5 corridor cells
extern int corridorY[MAX_PLAYERS][HOME_CORRIDOR_CELLS]; // Y grid for each player's 5 corridor cells
extern bool safeCells[TOTAL_PATH_CELLS];           // true if cell is safe (star or start)


// ========================== FUNCTION DECLARATIONS ==========================
// These functions are defined in board.cpp or game.cpp

// board.cpp - Board setup and drawing functions
void initializeBoard();                              // Sets up all board coordinates
Vector2f getGridPixel(int gx, int gy);               // Convert grid (0-14) to pixel position
Vector2f getHomePosition(int playerIndex, int tokenId); // Get token position in home base
Vector2f getTokenPixelPosition(int playerIndex, int tokenId); // Get token position by step
void drawBoard(RenderWindow &window);                // Draw entire board
void drawTokens(RenderWindow &window);               // Draw all tokens
void drawDice(RenderWindow &window);                 // Draw dice with dots
void drawUI(RenderWindow &window);                   // Draw sidebar UI (shows data structures!)
void drawStartScreen(RenderWindow &window);          // Draw welcome screen
void drawWinScreen(RenderWindow &window);            // Draw victory screen

// game.cpp - Game logic and rules
void initializeGame(int playerCount = 4);            // Reset and start new game
int rollDice();                                      // Roll dice, update QUEUE
bool canMoveToken(int playerIndex, int tokenId);     // Check if token can move
bool hasAnyValidMove(int playerIndex);               // Check if player has any valid move
bool moveToken(int playerIndex, int tokenId);        // Move token, update STACK
bool undoLastMove();                                 // UNDO: pop from STACK, restore state
void addActionLog(const string &msg);                // Add message to LINKED LIST
void nextTurn();                                     // Switch to next player
bool checkWin();                                     // Check if someone won
bool selectTokenById(int tokenId);                   // Select token by number (1-4)
bool selectTokenAtPosition(float mouseX, float mouseY); // Handle mouse clicks
int getPlayerStartCell(int playerIndex);             // Get start cell index for player

#endif // LUDO_H