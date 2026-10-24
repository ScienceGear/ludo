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

// Constants
const int MAX_PLAYERS = 4;
const int TOKENS_PER_PLAYER = 4;
const int TOTAL_PATH_CELLS = 52;
const int HOME_CORRIDOR_CELLS = 5;
const int FINISH_STEP = 57;

const float WINDOW_WIDTH = 960.0f;
const float WINDOW_HEIGHT = 700.0f;
const float BOARD_SIZE = 600.0f;
const float CELL_SIZE = 40.0f;
const float BOARD_X = 20.0f;
const float BOARD_Y = 50.0f;
const float TOKEN_RADIUS = 13.0f;

// Token representation
struct Token {
    int id;
    int step;       // 0: home, 1-51: path, 52-56: corridor, 57: finished
    int status;     // 0: home, 1: active, 2: finished
};

// Player representation
struct Player {
    string name;
    Color color;
    Token tokens[TOKENS_PER_PLAYER];
    int tokensFinished;
};

// Record for undo operation
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

// Queue node for linked queue
struct QueueNode {
    int data;
    QueueNode* next;
    QueueNode(int val) : data(val), next(nullptr) {}
};

// Custom queue implementation (FIFO)
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

// Stack node for linked stack
struct StackNode {
    MoveRecord data;
    StackNode* next;
    StackNode(const MoveRecord& record) : data(record), next(nullptr) {}
};

// Custom stack implementation (LIFO)
class CustomStack {
private:
    StackNode* topNode;
    int count;

public:
    CustomStack() : topNode(nullptr), count(0) {}

    ~CustomStack() {
        clear();
    }

    void push(const MoveRecord& record) {
        StackNode* newNode = new StackNode(record);
        newNode->next = topNode;
        topNode = newNode;
        count++;
    }

    void pop() {
        if (isEmpty()) return;
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
    }

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

// Node for singly linked list
struct ListNode {
    string message;
    ListNode* next;
    ListNode(const string& msg) : message(msg), next(nullptr) {}
};

// Custom linked list implementation
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

// Global variables
extern Font gameFont;

extern int currentPlayerIndex;
extern int numPlayers;
extern bool isRolling;
extern bool gameOver;
extern int winnerIndex;
extern int currentDiceValue;
extern bool canRollAgain;
extern string statusMessage;

extern Player players[MAX_PLAYERS];

// Instances of custom data structures
extern CustomQueue diceHistory;
extern CustomStack moveHistory;
extern CustomLinkedList actionLog;

// Coordinate lookup arrays
extern int boardPathX[TOTAL_PATH_CELLS];
extern int boardPathY[TOTAL_PATH_CELLS];
extern int corridorX[MAX_PLAYERS][HOME_CORRIDOR_CELLS];
extern int corridorY[MAX_PLAYERS][HOME_CORRIDOR_CELLS];
extern bool safeCells[TOTAL_PATH_CELLS];

// Functions
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
