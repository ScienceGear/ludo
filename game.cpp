/*
 * LUDO GAME - GAME LOGIC IMPLEMENTATION (game.cpp)
 * -----------------------------------------------
 * This file contains all rules, state transitions, and DATA STRUCTURE OPERATIONS:
 * 
 * 1. QUEUE (std::queue<int> diceHistory):
 *    - Pushes new dice rolls onto the back (FIFO).
 *    - Pops oldest roll when size > 5.
 * 
 * 2. STACK (std::stack<MoveRecord> moveHistory):
 *    - Pushes every move record onto the top (LIFO).
 *    - When player presses 'U', pops the top and restores previous state (UNDO)!
 * 
 * 3. LINKED LIST (std::list<string> actionLog):
 *    - Appends recent game event messages.
 *    - Pops from front when history exceeds 4 items.
 */

#include "ludo.h"
#include <cmath>

int currentPlayerIndex = 0;
int numPlayers = 4;
bool isRolling = true;
bool gameOver = false;
int winnerIndex = -1;
int currentDiceValue = 1;
bool canRollAgain = false;
string statusMessage = "Press SPACE or Click Dice to Roll!";

Player players[MAX_PLAYERS];

// Data structures definitions
queue<int> diceHistory;
stack<MoveRecord> moveHistory;
list<string> actionLog;

// Adds a message to the Linked List action log
void addActionLog(const string &msg) {
    actionLog.push_back(msg);
    if (actionLog.size() > 4) {
        actionLog.pop_front();
    }
}

void initializeGame(int playerCount) {
    numPlayers = playerCount;
    if (numPlayers < 2) numPlayers = 2;
    if (numPlayers > 4) numPlayers = 4;

    // Reset Data Structures
    while (!diceHistory.empty()) diceHistory.pop();
    while (!moveHistory.empty()) moveHistory.pop();
    actionLog.clear();

    // Define player colors and names
    Color playerColors[MAX_PLAYERS] = {
        Color(225, 45, 45),    // Red
        Color(40, 175, 60),    // Green
        Color(245, 190, 20),   // Yellow
        Color(35, 115, 235)    // Blue
    };

    string playerNames[MAX_PLAYERS] = {
        "Player 1 (Red)",
        "Player 2 (Green)",
        "Player 3 (Yellow)",
        "Player 4 (Blue)"
    };

    for (int p = 0; p < numPlayers; p++) {
        players[p].name = playerNames[p];
        players[p].color = playerColors[p];
        players[p].tokensFinished = 0;

        for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
            players[p].tokens[t].id = t;
            players[p].tokens[t].step = 0;   // 0 = inside home base
            players[p].tokens[t].status = 0; // 0 = HOME
        }
    }

    currentPlayerIndex = 0;
    isRolling = true;
    gameOver = false;
    winnerIndex = -1;
    currentDiceValue = 1;
    canRollAgain = false;
    statusMessage = players[0].name + "'s turn. Press SPACE to roll!";
    addActionLog("Game started with 4 players.");
}

// Rolls the dice and updates the QUEUE data structure
int rollDice() {
    int roll = (rand() % 6) + 1;
    currentDiceValue = roll;
    isRolling = false;
    canRollAgain = (roll == 6);

    // 1. QUEUE OPERATION (FIFO): Push roll, pop oldest if > 5
    diceHistory.push(roll);
    if (diceHistory.size() > 5) {
        diceHistory.pop();
    }

    // 3. LINKED LIST OPERATION: Add roll event
    addActionLog(players[currentPlayerIndex].name + " rolled a " + to_string(roll));

    // Check if player has any legal moves with this roll
    if (!hasAnyValidMove(currentPlayerIndex)) {
        if (canRollAgain) {
            statusMessage = "Rolled a 6! No moves available. Roll again!";
            isRolling = true;
            canRollAgain = false;
        } else {
            statusMessage = "Rolled " + to_string(roll) + ". No valid move! Passing turn.";
            nextTurn();
        }
    } else {
        if (roll == 6) {
            statusMessage = "Rolled a 6! Select a token (1-4) to move.";
        } else {
            statusMessage = "Rolled " + to_string(roll) + ". Select a token (1-4) to move.";
        }
    }

    return roll;
}

// Checks if a specific token can be moved
bool canMoveToken(int playerIndex, int tokenId) {
    if (tokenId < 0 || tokenId >= TOKENS_PER_PLAYER) return false;
    const Token &tok = players[playerIndex].tokens[tokenId];

    // Already reached the center finish
    if (tok.status == 2 || tok.step == FINISH_STEP) {
        return false;
    }

    // Inside home base: needs a 6 to come out
    if (tok.status == 0 || tok.step == 0) {
        return (currentDiceValue == 6);
    }

    // Active token: must not overshoot finish (step 57)
    if (tok.status == 1) {
        return (tok.step + currentDiceValue <= FINISH_STEP);
    }

    return false;
}

// Checks if the player can move at least one of their 4 tokens
bool hasAnyValidMove(int playerIndex) {
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        if (canMoveToken(playerIndex, t)) {
            return true;
        }
    }
    return false;
}

// Moves the chosen token and records into STACK (for Undo)
bool moveToken(int playerIndex, int tokenId) {
    if (!canMoveToken(playerIndex, tokenId)) return false;

    Token &tok = players[playerIndex].tokens[tokenId];
    bool earnedExtraRoll = false;

    // 2. Prepare STACK Record (LIFO) for Undo
    MoveRecord rec;
    rec.playerIndex = playerIndex;
    rec.tokenId = tokenId;
    rec.prevStep = tok.step;
    rec.prevStatus = tok.status;
    rec.diceValue = currentDiceValue;
    rec.wasRolling = isRolling;
    rec.captured = false;
    rec.capturedPlayer = -1;
    rec.capturedTokenId = -1;
    rec.capturedPrevStep = 0;

    // Case 1: Token exiting home base on roll of 6
    if (tok.status == 0) {
        tok.status = 1;
        tok.step = 1; // Step 1 is the player's Start Cell
        statusMessage = players[playerIndex].name + " brought Token " + to_string(tokenId + 1) + " out!";
        addActionLog(players[playerIndex].name + " brought Token " + to_string(tokenId + 1) + " out!");
        earnedExtraRoll = true;
    }
    // Case 2: Moving an active token forward
    else if (tok.status == 1) {
        tok.step += currentDiceValue;

        // Subcase 2A: Reached the Center Finish!
        if (tok.step == FINISH_STEP) {
            tok.status = 2; // Finished
            players[playerIndex].tokensFinished++;
            statusMessage = players[playerIndex].name + "'s Token " + to_string(tokenId + 1) + " reached Finish!";
            addActionLog(players[playerIndex].name + " Token " + to_string(tokenId + 1) + " FINISHED!");
            earnedExtraRoll = true;

            // Push to stack before checking win
            moveHistory.push(rec);

            if (checkWin()) {
                return true;
            }
            isRolling = true;
            canRollAgain = false;
            statusMessage += " Roll again!";
            return true;
        }
        // Subcase 2B: On Main 52-cell track -> Check Capture
        else if (tok.step >= 1 && tok.step <= 51) {
            int startCell = playerIndex * 13;
            int landingCell = (startCell + (tok.step - 1)) % TOTAL_PATH_CELLS;

            addActionLog(players[playerIndex].name + " moved Token " + to_string(tokenId + 1) + " to Step " + to_string(tok.step));

            // If landing cell is NOT safe, check for opponent tokens to capture
            if (!safeCells[landingCell]) {
                for (int op = 0; op < numPlayers; op++) {
                    if (op == playerIndex) continue;

                    for (int ot = 0; ot < TOKENS_PER_PLAYER; ot++) {
                        Token &opponentToken = players[op].tokens[ot];
                        if (opponentToken.status == 1 && opponentToken.step >= 1 && opponentToken.step <= 51) {
                            int opponentCell = (op * 13 + (opponentToken.step - 1)) % TOTAL_PATH_CELLS;
                            if (opponentCell == landingCell) {
                                // CAPTURE! Record captured details for stack undo
                                rec.captured = true;
                                rec.capturedPlayer = op;
                                rec.capturedTokenId = ot;
                                rec.capturedPrevStep = opponentToken.step;

                                opponentToken.status = 0;
                                opponentToken.step = 0;
                                earnedExtraRoll = true;
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

    // 2. STACK OPERATION (LIFO): Push move record onto stack
    moveHistory.push(rec);

    // Determine next state
    if (currentDiceValue == 6 || earnedExtraRoll) {
        isRolling = true;
        canRollAgain = false;
        statusMessage += " Roll again!";
    } else {
        nextTurn();
    }

    return true;
}

// 2. STACK OPERATION (LIFO): Undo last move
bool undoLastMove() {
    if (moveHistory.empty()) {
        statusMessage = "No moves to undo!";
        return false;
    }

    // Pop the most recent move from the top of the stack (LIFO)
    MoveRecord rec = moveHistory.top();
    moveHistory.pop();

    // If token was finished, decrement finished count
    if (players[rec.playerIndex].tokens[rec.tokenId].status == 2 && rec.prevStatus != 2) {
        players[rec.playerIndex].tokensFinished--;
    }

    // Restore token position and status
    players[rec.playerIndex].tokens[rec.tokenId].step = rec.prevStep;
    players[rec.playerIndex].tokens[rec.tokenId].status = rec.prevStatus;

    // Restore captured opponent token if there was a capture
    if (rec.captured) {
        players[rec.capturedPlayer].tokens[rec.capturedTokenId].status = 1;
        players[rec.capturedPlayer].tokens[rec.capturedTokenId].step = rec.capturedPrevStep;
    }

    // Restore turn and game state
    currentPlayerIndex = rec.playerIndex;
    currentDiceValue = rec.diceValue;
    isRolling = false;
    gameOver = false;
    statusMessage = "Undid move for " + players[currentPlayerIndex].name + "! Press 1-4 to pick another.";
    addActionLog("UNDO: Restored " + players[currentPlayerIndex].name + " Token " + to_string(rec.tokenId + 1));

    return true;
}

// Switches turn to the next player
void nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
    isRolling = true;
    canRollAgain = false;
    currentDiceValue = 1;
    statusMessage = players[currentPlayerIndex].name + "'s turn. Press SPACE to roll!";
}

// Checks if any player has brought all 4 tokens to the finish
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

// Selects token by 0-indexed ID (from keyboard 1-4 or UI buttons)
bool selectTokenById(int tokenId) {
    if (isRolling || gameOver) return false;
    return moveToken(currentPlayerIndex, tokenId);
}

// Selects token or rolls dice by mouse click position
bool selectTokenAtPosition(float mouseX, float mouseY) {
    if (gameOver) return false;

    // 1. Check if Dice or Roll button was clicked while waiting to roll
    if (isRolling) {
        // Dice rect: X [755, 830], Y [115, 190]
        if (mouseX >= 750.0f && mouseX <= 835.0f && mouseY >= 115.0f && mouseY <= 190.0f) {
            rollDice();
            return true;
        }
        // "ROLL DICE" button: X [680, 920], Y [195, 230]
        if (mouseX >= 680.0f && mouseX <= 920.0f && mouseY >= 195.0f && mouseY <= 235.0f) {
            rollDice();
            return true;
        }
        return false;
    }

    // 2. Check if one of the 4 Token buttons on the sidebar was clicked
    const float panelX = 645.0f;
    const float panelY = 50.0f;
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        float btnY = panelY + 285.0f + t * 35.0f;
        if (mouseX >= panelX + 15.0f && mouseX <= panelX + 285.0f &&
            mouseY >= btnY && mouseY <= btnY + 30.0f) {
            return selectTokenById(t);
        }
    }

    // 3. Check if an Undo button was clicked
    if (!moveHistory.empty() && mouseX >= panelX + 15.0f && mouseX <= panelX + 285.0f &&
        mouseY >= panelY + 430.0f && mouseY <= panelY + 455.0f) {
        return undoLastMove();
    }

    // 4. Check if a token on the board was clicked directly
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
