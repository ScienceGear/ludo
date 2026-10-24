/*
 * LUDO GAME - HEADER FILE (ludo.h)
 * ================================
 * This file contains ALL shared constants, custom data structures (built from scratch!),
 * global variables, and function declarations.
 * 
 * CORE CUSTOM DATA STRUCTURES (BUILT FROM SCRATCH - NO STL QUEUE/STACK/LIST!):
 * 1. CustomQueue      : Linked Queue (FIFO) for Dice Roll History
 * 2. CustomStack      : Linked Stack (LIFO) for Move History & UNDO (Press 'U')
 * 3. CustomLinkedList : Singly Linked List for Live Game Event Log
 * 4. Arrays & Matrices: 1D path array (52 cells) and 2D corridor matrix (4x5)
 * 5. Structs          : Token, Player, MoveRecord
 */

#ifndef LUDO_H
#define LUDO_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;
using namespace sf;

// ========================== CONSTANTS ==========================

const int MAX_PLAYERS = 4;           // 4 players: Red, Green, Yellow, Blue
const int TOKENS_PER_PLAYER = 4;     // Each player has 4 tokens
const int TOTAL_PATH_CELLS = 52;     // 52 cells in the main circular track
const int HOME_CORRIDOR_CELLS = 5;   // 5 cells in each player's private corridor
const int FINISH_STEP = 57;          // Step 57 = token reached center finish!

// Window and Board dimensions
const float WINDOW_WIDTH = 960.0f;
const float WINDOW_HEIGHT = 700.0f;
const float BOARD_SIZE = 600.0f;     // 15 cells * 40 pixels = 600 px
const float CELL_SIZE = 40.0f;       // Each cell is 40x40 pixels
const float BOARD_X = 20.0f;         // Board position X
const float BOARD_Y = 50.0f;         // Board position Y
const float TOKEN_RADIUS = 13.0f;    // Radius of token circle

// ========================== BASIC STRUCTS ==========================

/*
 * Token struct - represents ONE token on the board
 * step: 0 = in home base, 1-51 = on main track, 52-56 = in corridor, 57 = finished
 * status: 0 = HOME, 1 = ACTIVE (on board), 2 = FINISHED
 */
struct Token {
    int id;         // Token ID: 0, 1, 2, 3
    int step;       // Progress (0 to 57)
    int status;     // 0=HOME, 1=ACTIVE, 2=FINISHED
};

/*
 * Player struct - represents ONE player
 * Contains 4 tokens and count of finished tokens
 */
struct Player {
    string name;                     // e.g. "Player 1 (Red)"
    Color color;                     // Player's theme color
    Token tokens[TOKENS_PER_PLAYER]; // Array of 4 tokens
    int tokensFinished;              // Tokens reached finish (0 to 4)
};

/*
 * MoveRecord struct - stores snapshot of ONE move for the UNDO feature
 */
struct MoveRecord {
    int playerIndex;
    int tokenId;
    int prevStep;
    int prevStatus;
    int diceValue;
    bool wasRolling;
    bool captured;
    int capturedPlayer;
    int capturedTokenId;
    int capturedPrevStep;
};


// ============================================================================
//               CUSTOM DATA STRUCTURES (WRITTEN FROM SCRATCH)
// ============================================================================

// ----------------------------------------------------------------------------
// 1. CUSTOM QUEUE (FIFO: First-In, First-Out)
// Used for: Dice Roll History (keeps last 5 rolls)
// ----------------------------------------------------------------------------
struct QueueNode {
    int data;
    QueueNode* next;
    QueueNode(int val) : data(val), next(nullptr) {}
};

class CustomQueue {
private:
    QueueNode* frontNode;
    QueueNode* rearNode;
    int count;

public:
    CustomQueue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~CustomQueue() {
        clear();
    }

    // Enqueue: add element to the back (rear)
    void push(int val) {
        QueueNode* newNode = new QueueNode(val);
        if (rearNode == nullptr) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    // Dequeue: remove element from the front
    void pop() {
        if (isEmpty()) return;
        QueueNode* temp = frontNode;
        frontNode = frontNode->next;
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        count--;
    }

    // Inspect front element
    int front() const {
        if (isEmpty()) return -1;
        return frontNode->data;
    }

    bool isEmpty() const {
        return frontNode == nullptr;
    }

    bool empty() const {
        return isEmpty();
    }

    int size() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }

    // Exports queue elements in order for display on UI
    vector<int> toVector() const {
        vector<int> result;
        QueueNode* curr = frontNode;
        while (curr != nullptr) {
            result.push_back(curr->data);
            curr = curr->next;
        }
        return result;
    }
};


// ----------------------------------------------------------------------------
// 2. CUSTOM STACK (LIFO: Last-In, First-Out)
// Used for: Move History & Undo Feature (Press 'U')
// ----------------------------------------------------------------------------
struct StackNode {
    MoveRecord data;
    StackNode* next;
    StackNode(const MoveRecord& record) : data(record), next(nullptr) {}
};

class CustomStack {
private:
    StackNode* topNode;
    int count;

public:
    CustomStack() : topNode(nullptr), count(0) {}

    ~CustomStack() {
        clear();
    }

    // Push: place a new move on top of the stack
    void push(const MoveRecord& record) {
        StackNode* newNode = new StackNode(record);
        newNode->next = topNode;
        topNode = newNode;
        count++;
    }

    // Pop: remove the top move from the stack
    void pop() {
        if (isEmpty()) return;
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
    }

    // Top: peek at the most recent move
    MoveRecord top() const {
        if (isEmpty()) {
            MoveRecord dummy;
            dummy.playerIndex = -1;
            return dummy;
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    bool empty() const {
        return isEmpty();
    }

    int size() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }
};


// ----------------------------------------------------------------------------
// 3. CUSTOM LINKED LIST (Singly Linked List)
// Used for: Live Game Event Messages Log
// ----------------------------------------------------------------------------
struct ListNode {
    string message;
    ListNode* next;
    ListNode(const string& msg) : message(msg), next(nullptr) {}
};

class CustomLinkedList {
private:
    ListNode* head;
    ListNode* tail;
    int count;

public:
    CustomLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~CustomLinkedList() {
        clear();
    }

    // Insert at end: add new log message to tail
    void push_back(const string& msg) {
        ListNode* newNode = new ListNode(msg);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Delete from front: remove oldest log message
    void pop_front() {
        if (head == nullptr) return;
        ListNode* temp = head;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        }
        delete temp;
        count--;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    bool empty() const {
        return isEmpty();
    }

    int size() const {
        return count;
    }

    void clear() {
        while (head != nullptr) {
            pop_front();
        }
    }

    // Exports all messages for drawing in sidebar UI
    vector<string> toVector() const {
        vector<string> result;
        ListNode* curr = head;
        while (curr != nullptr) {
            result.push_back(curr->message);
            curr = curr->next;
        }
        return result;
    }
};


// ========================== GLOBAL STATE VARIABLES ==========================

extern Font gameFont;

extern int currentPlayerIndex;       // Whose turn: 0=Red, 1=Green, 2=Yellow, 3=Blue
extern int numPlayers;               // Number of players (default 4)
extern bool isRolling;               // true = waiting for dice roll, false = choose token
extern bool gameOver;                // true when someone wins
extern int winnerIndex;              // Who won (0 to 3)
extern int currentDiceValue;         // Current dice value (1 to 6)
extern bool canRollAgain;            // Extra roll granted?
extern string statusMessage;         // Message shown on sidebar

extern Player players[MAX_PLAYERS];

// CUSTOM DATA STRUCTURE INSTANCES:
extern CustomQueue diceHistory;         // Custom Queue (FIFO)
extern CustomStack moveHistory;         // Custom Stack (LIFO)
extern CustomLinkedList actionLog;      // Custom Linked List

// Board grid coordinate arrays:
extern int boardPathX[TOTAL_PATH_CELLS];
extern int boardPathY[TOTAL_PATH_CELLS];
extern int corridorX[MAX_PLAYERS][HOME_CORRIDOR_CELLS];
extern int corridorY[MAX_PLAYERS][HOME_CORRIDOR_CELLS];
extern bool safeCells[TOTAL_PATH_CELLS];


// ========================== FUNCTION DECLARATIONS ==========================

// board.cpp - Board setup and drawing
void initializeBoard();
Vector2f getGridPixel(int gx, int gy);
Vector2f getHomePosition(int playerIndex, int tokenId);
Vector2f getTokenPixelPosition(int playerIndex, int tokenId);
void drawBoard(RenderWindow &window);
void drawTokens(RenderWindow &window);
void drawDice(RenderWindow &window);
void drawUI(RenderWindow &window);
void drawStartScreen(RenderWindow &window);
void drawWinScreen(RenderWindow &window);

// game.cpp - Game logic and rules
void initializeGame(int playerCount = 4);
int rollDice();
bool canMoveToken(int playerIndex, int tokenId);
bool hasAnyValidMove(int playerIndex);
bool moveToken(int playerIndex, int tokenId);
bool undoLastMove();
void addActionLog(const string &msg);
void nextTurn();
bool checkWin();
bool selectTokenById(int tokenId);
bool selectTokenAtPosition(float mouseX, float mouseY);
int getPlayerStartCell(int playerIndex);

#endif // LUDO_H
