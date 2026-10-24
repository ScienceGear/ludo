/*
 * LUDO GAME - GAME LOGIC (game.cpp)
 * =================================
 * This file contains ALL the game rules and data structure operations.
 * 
 * KEY DATA STRUCTURE OPERATIONS (FOR VIVA/MAM):
 * 
 * 1. QUEUE (std::queue<int> diceHistory) - FIFO (First In First Out)
 *    - rollDice():  diceHistory.push(roll); if size > 5 -> diceHistory.pop();
 *    - Shows last 5 rolls on sidebar. Oldest at front, newest at back.
 * 
 * 2. STACK (std::stack<MoveRecord> moveHistory) - LIFO (Last In First Out)
 *    - moveToken(): Creates MoveRecord, moveHistory.push(record);
 *    - undoLastMove(): MoveRecord rec = moveHistory.top(); moveHistory.pop(); restore state
 *    - Press 'U' to undo last move.
 * 
 * 3. LINKED LIST (std::list<string> actionLog)
 *    - addActionLog(): actionLog.push_back(msg); if size > 4 -> actionLog.pop_front();
 *    - Shows recent game events on sidebar.
 */

#include "ludo.h"
#include <cmath>

// ========================== GLOBAL VARIABLE DEFINITIONS ==========================
// These variables are declared 'extern' in ludo.h, defined HERE

int currentPlayerIndex = 0;    // Start with Red (0)
int numPlayers = 4;            // 4 players
bool isRolling = true;         // Start waiting for dice roll
bool gameOver = false;         // Game not over yet
int winnerIndex = -1;          // No winner yet
int currentDiceValue = 1;      // Default dice value
bool canRollAgain = false;     // No extra roll initially
string statusMessage = "Press SPACE or Click Dice to Roll!";

Player players[MAX_PLAYERS];   // Array of 4 players

// THE 3 CUSTOM DATA STRUCTURES (BUILT FROM SCRATCH - NO STL!)
CustomQueue diceHistory;                   // CUSTOM QUEUE: Last 5 dice rolls (FIFO)
CustomStack moveHistory;                   // CUSTOM STACK: Move history for UNDO (LIFO)
CustomLinkedList actionLog;                // CUSTOM LINKED LIST: Game event log


// ========================== HELPER FUNCTIONS ==========================

// Add message to Linked List (keeps only last 4 messages)
void addActionLog(const string &msg) {
    actionLog.push_back(msg);              // Add to BACK of list
    if (actionLog.size() > 4) {            // If more than 4 messages
        actionLog.pop_front();             // Remove from FRONT (oldest)
    }
}


// ========================== GAME INITIALIZATION ==========================

void initializeGame(int playerCount) {
    numPlayers = playerCount;
    if (numPlayers < 2) numPlayers = 2;
    if (numPlayers > 4) numPlayers = 4;

    // CLEAR ALL DATA STRUCTURES FOR NEW GAME
    while (!diceHistory.empty()) diceHistory.pop();  // Clear QUEUE
    while (!moveHistory.empty()) moveHistory.pop();  // Clear STACK
    actionLog.clear();                               // Clear LINKED LIST

    // Player colors (Red, Green, Yellow, Blue)
    Color playerColors[MAX_PLAYERS] = {
        Color(225, 45, 45),    // Red
        Color(40, 175, 60),    // Green
        Color(245, 190, 20),   // Yellow
        Color(35, 115, 235)    // Blue
    };

    // Player names
    string playerNames[MAX_PLAYERS] = {
        "Player 1 (Red)",
        "Player 2 (Green)",
        "Player 3 (Yellow)",
        "Player 4 (Blue)"
    };

    // Initialize each player's 4 tokens
    for (int p = 0; p < numPlayers; p++) {
        players[p].name = playerNames[p];
        players[p].color = playerColors[p];
        players[p].tokensFinished = 0;

        for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
            players[p].tokens[t].id = t;
            players[p].tokens[t].step = 0;     // 0 = in home base
            players[p].tokens[t].status = 0;   // 0 = HOME
        }
    }

    // Reset game state
    currentPlayerIndex = 0;      // Red starts
    isRolling = true;            // Waiting for dice roll
    gameOver = false;
    winnerIndex = -1;
    currentDiceValue = 1;
    canRollAgain = false;
    statusMessage = players[0].name + "'s turn. Press SPACE to roll!";
    addActionLog("Game started with 4 players.");
}


// ========================== DICE ROLLING (QUEUE OPERATION) ==========================

int rollDice() {
    // Generate random number 1-6
    int roll = (rand() % 6) + 1;
    currentDiceValue = roll;
    isRolling = false;           // Now player must choose a token
    canRollAgain = (roll == 6);  // Rolling 6 gives extra turn

    // ========== QUEUE OPERATION (FIFO) ==========
    // Push new roll to BACK of queue
    diceHistory.push(roll);
    // If queue has more than 5 items, remove oldest from FRONT
    if (diceHistory.size() > 5) {
        diceHistory.pop();
    }
    // ============================================

    // Add to Linked List (event log)
    addActionLog(players[currentPlayerIndex].name + " rolled a " + to_string(roll));

    // Check if player has ANY valid move with this roll
    if (!hasAnyValidMove(currentPlayerIndex)) {
        if (canRollAgain) {
            // Rolled 6 but no moves possible - roll again
            statusMessage = "Rolled a 6! No moves available. Roll again!";
            isRolling = true;
            canRollAgain = false;
        } else {
            // No valid move - pass turn
            statusMessage = "Rolled " + to_string(roll) + ". No valid move! Passing turn.";
            nextTurn();
        }
    } else {
        // Has valid move(s)
        if (roll == 6) {
            statusMessage = "Rolled a 6! Select a token (1-4) to move.";
        } else {
            statusMessage = "Rolled " + to_string(roll) + ". Select a token (1-4) to move.";
        }
    }

    return roll;
}


// ========================== MOVE VALIDATION ==========================

// Check if a SPECIFIC token can be moved
bool canMoveToken(int playerIndex, int tokenId) {
    // Invalid token ID
    if (tokenId < 0 || tokenId >= TOKENS_PER_PLAYER) return false;

    const Token &tok = players[playerIndex].tokens[tokenId];

    // Already finished - can't move
    if (tok.status == 2 || tok.step == FINISH_STEP) {
        return false;
    }

    // In home base - needs 6 to come out
    if (tok.status == 0 || tok.step == 0) {
        return (currentDiceValue == 6);
    }

    // Active on board - must not overshoot finish (step 57)
    if (tok.status == 1) {
        return (tok.step + currentDiceValue <= FINISH_STEP);
    }

    return false;
}

// Check if player has AT LEAST ONE token that can move
bool hasAnyValidMove(int playerIndex) {
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        if (canMoveToken(playerIndex, t)) {
            return true;
        }
    }
    return false;
}


// ========================== MOVE TOKEN (STACK OPERATION) ==========================

bool moveToken(int playerIndex, int tokenId) {
    // Can't move this token
    if (!canMoveToken(playerIndex, tokenId)) return false;

    Token &tok = players[playerIndex].tokens[tokenId];
    bool earnedExtraRoll = false;

    // ========== PREPARE STACK RECORD (for UNDO) ==========
    // Save EVERYTHING needed to undo this move later
    MoveRecord rec;
    rec.playerIndex = playerIndex;
    rec.tokenId = tokenId;
    rec.prevStep = tok.step;          // Save old step
    rec.prevStatus = tok.status;      // Save old status
    rec.diceValue = currentDiceValue; // Save dice value
    rec.wasRolling = isRolling;       // Save game state
    rec.captured = false;             // No capture yet
    rec.capturedPlayer = -1;
    rec.capturedTokenId = -1;
    rec.capturedPrevStep = 0;
    // ====================================================

    // CASE 1: Token exiting home base (needs 6)
    if (tok.status == 0) {
        tok.status = 1;        // Now ACTIVE
        tok.step = 1;          // Step 1 = start cell
        statusMessage = players[playerIndex].name + " brought Token " + to_string(tokenId + 1) + " out!";
        addActionLog(players[playerIndex].name + " brought Token " + to_string(tokenId + 1) + " out!");
        earnedExtraRoll = true; // Rolling 6 gives extra roll
    }
    // CASE 2: Moving active token forward
    else if (tok.status == 1) {
        tok.step += currentDiceValue;  // Advance by dice value

        // SUBCASE 2A: Reached FINISH (step 57)!
        if (tok.step == FINISH_STEP) {
            tok.status = 2;  // FINISHED
            players[playerIndex].tokensFinished++;
            statusMessage = players[playerIndex].name + "'s Token " + to_string(tokenId + 1) + " reached Finish!";
            addActionLog(players[playerIndex].name + " Token " + to_string(tokenId + 1) + " FINISHED!");
            earnedExtraRoll = true;  // Reaching finish gives extra roll

            // Save to STACK before checking win
            moveHistory.push(rec);

            if (checkWin()) {
                return true;  // Game over!
            }

            isRolling = true;
            canRollAgain = false;
            statusMessage += " Roll again!";
            return true;
        }

        // SUBCASE 2B: On main track (steps 1-51) - CHECK CAPTURE
        if (tok.step >= 1 && tok.step <= 51) {
            // Calculate which board cell we landed on
            int startCell = playerIndex * 13;  // Each player starts 13 cells apart
            int landingCell = (startCell + (tok.step - 1)) % TOTAL_PATH_CELLS;

            addActionLog(players[playerIndex].name + " moved Token " + to_string(tokenId + 1) + " to Step " + to_string(tok.step));

            // If NOT on safe cell, check for opponent tokens to capture
            if (!safeCells[landingCell]) {
                // Check all opponent tokens
                for (int op = 0; op < numPlayers; op++) {
                    if (op == playerIndex) continue;  // Skip self

                    for (int ot = 0; ot < TOKENS_PER_PLAYER; ot++) {
                        Token &opponentToken = players[op].tokens[ot];

                        // Opponent token must be active on main track
                        if (opponentToken.status == 1 && opponentToken.step >= 1 && opponentToken.step <= 51) {
                            int opponentCell = (op * 13 + (opponentToken.step - 1)) % TOTAL_PATH_CELLS;

                            // SAME CELL = CAPTURE!
                            if (opponentCell == landingCell) {
                                // Save captured token info for UNDO
                                rec.captured = true;
                                rec.capturedPlayer = op;
                                rec.capturedTokenId = ot;
                                rec.capturedPrevStep = opponentToken.step;

                                // Send opponent back to base
                                opponentToken.status = 0;
                                opponentToken.step = 0;
                                earnedExtraRoll = true;  // Capture gives extra roll

                                statusMessage = players[playerIndex].name + " captured " +
                                                players[op].name + "'s Token! Extra roll awarded!";
                                addActionLog("CAPTURE! " + players[playerIndex].name + " hit " + players[op].name);
                            }
                        }
                    }
                }
            }
        }
    }

    // ========== STACK OPERATION (LIFO) ==========
    // Push move record onto TOP of stack
    moveHistory.push(rec);
    // ============================================

    // Determine what happens next
    if (currentDiceValue == 6 || earnedExtraRoll) {
        // Extra roll: roll 6, capture, or finish
        isRolling = true;
        canRollAgain = false;
        statusMessage += " Roll again!";
    } else {
        // Normal turn ends - next player
        nextTurn();
    }

    return true;
}


// ========================== UNDO LAST MOVE (STACK POP) ==========================

bool undoLastMove() {
    // Stack empty - nothing to undo
    if (moveHistory.empty()) {
        statusMessage = "No moves to undo!";
        return false;
    }

    // ========== STACK OPERATION (LIFO) ==========
    // Get TOP record (most recent move) and REMOVE it
    MoveRecord rec = moveHistory.top();
    moveHistory.pop();
    // ============================================

    // If token was finished, decrement finished count
    if (players[rec.playerIndex].tokens[rec.tokenId].status == 2 && rec.prevStatus != 2) {
        players[rec.playerIndex].tokensFinished--;
    }

    // Restore token's previous position and status
    players[rec.playerIndex].tokens[rec.tokenId].step = rec.prevStep;
    players[rec.playerIndex].tokens[rec.tokenId].status = rec.prevStatus;

    // If there was a capture, restore the captured token too
    if (rec.captured) {
        players[rec.capturedPlayer].tokens[rec.capturedTokenId].status = 1;
        players[rec.capturedPlayer].tokens[rec.capturedTokenId].step = rec.capturedPrevStep;
    }

    // Restore game state
    currentPlayerIndex = rec.playerIndex;
    currentDiceValue = rec.diceValue;
    isRolling = false;      // Now player can choose a different token
    gameOver = false;       // Game continues
    statusMessage = "Undid move for " + players[currentPlayerIndex].name + "! Press 1-4 to pick another.";
    addActionLog("UNDO: Restored " + players[currentPlayerIndex].name + " Token " + to_string(rec.tokenId + 1));

    return true;
}


// ========================== TURN MANAGEMENT ==========================

void nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;  // 0->1->2->3->0
    isRolling = true;
    canRollAgain = false;
    currentDiceValue = 1;
    statusMessage = players[currentPlayerIndex].name + "'s turn. Press SPACE to roll!";
}


// ========================== WIN CHECK ==========================

bool checkWin() {
    for (int p = 0; p < numPlayers; p++) {
        if (players[p].tokensFinished == TOKENS_PER_PLAYER) {
            gameOver = true;
            winnerIndex = p;
            statusMessage = players[p].name + " WINS THE GAME!";
            addActionLog("GAME OVER: " + players[p].name + " WINS!");
            return true;
        }
    }
    return false;
}


// ========================== INPUT HANDLING ==========================

// Select token by keyboard (1,2,3,4) or UI button
bool selectTokenById(int tokenId) {
    if (isRolling || gameOver) return false;  // Can't move while rolling or game over
    return moveToken(currentPlayerIndex, tokenId);
}

// Handle mouse clicks on dice, tokens, buttons
bool selectTokenAtPosition(float mouseX, float mouseY) {
    if (gameOver) return false;

    // 1. Click on DICE or ROLL button while waiting to roll
    if (isRolling) {
        // Dice area
        if (mouseX >= 750.0f && mouseX <= 835.0f && mouseY >= 115.0f && mouseY <= 190.0f) {
            rollDice();
            return true;
        }
        // "ROLL DICE" button
        if (mouseX >= 680.0f && mouseX <= 920.0f && mouseY >= 195.0f && mouseY <= 235.0f) {
            rollDice();
            return true;
        }
        return false;
    }

    // 2. Click on TOKEN BUTTONS on sidebar (1,2,3,4)
    const float panelX = 645.0f;
    const float panelY = 50.0f;
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        float btnY = panelY + 285.0f + t * 35.0f;
        if (mouseX >= panelX + 15.0f && mouseX <= panelX + 285.0f &&
            mouseY >= btnY && mouseY <= btnY + 30.0f) {
            return selectTokenById(t);
        }
    }

    // 3. Click on UNDO button
    if (!moveHistory.empty() && mouseX >= panelX + 15.0f && mouseX <= panelX + 285.0f &&
        mouseY >= panelY + 430.0f && mouseY <= panelY + 455.0f) {
        return undoLastMove();
    }

    // 4. Click directly on a token on the board
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        Vector2f pos = getTokenPixelPosition(currentPlayerIndex, t);
        float dx = mouseX - pos.x;
        float dy = mouseY - pos.y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist <= TOKEN_RADIUS + 8.0f) {
            return selectTokenById(t);
        }
    }

    return false;
}