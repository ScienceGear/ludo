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

CustomQueue diceHistory;
CustomStack moveHistory;
CustomLinkedList actionLog;

// Add event to action log (keeps last 4 events)
void addActionLog(const string &msg) {
    actionLog.push_back(msg);
    if (actionLog.size() > 4) {
        actionLog.pop_front();
    }
}

// Reset and setup new game
void initializeGame(int playerCount) {
    numPlayers = playerCount;
    if (numPlayers < 2) numPlayers = 2;
    if (numPlayers > 4) numPlayers = 4;

    while (!diceHistory.empty()) diceHistory.pop();
    while (!moveHistory.empty()) moveHistory.pop();
    actionLog.clear();

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
            players[p].tokens[t].step = 0;
            players[p].tokens[t].status = 0;
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

// Roll dice (1-6) and push to history queue
int rollDice() {
    int roll = (rand() % 6) + 1;
    currentDiceValue = roll;
    isRolling = false;
    canRollAgain = (roll == 6);

    // Push into queue, drop oldest if > 5
    diceHistory.push(roll);
    if (diceHistory.size() > 5) {
        diceHistory.pop();
    }

    addActionLog(players[currentPlayerIndex].name + " rolled a " + to_string(roll));

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

// Check if a token can be moved with current roll
bool canMoveToken(int playerIndex, int tokenId) {
    if (tokenId < 0 || tokenId >= TOKENS_PER_PLAYER) return false;

    const Token &tok = players[playerIndex].tokens[tokenId];

    if (tok.status == 2 || tok.step == FINISH_STEP) {
        return false;
    }

    if (tok.status == 0 || tok.step == 0) {
        return (currentDiceValue == 6);
    }

    if (tok.status == 1) {
        return (tok.step + currentDiceValue <= FINISH_STEP);
    }

    return false;
}

// Check if any of player's tokens have a valid move
bool hasAnyValidMove(int playerIndex) {
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        if (canMoveToken(playerIndex, t)) {
            return true;
        }
    }
    return false;
}

// Move selected token and record state to stack for undo
bool moveToken(int playerIndex, int tokenId) {
    if (!canMoveToken(playerIndex, tokenId)) return false;

    Token &tok = players[playerIndex].tokens[tokenId];
    bool earnedExtraRoll = false;

    // Save previous state for undo
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

    // Token exiting base
    if (tok.status == 0) {
        tok.status = 1;
        tok.step = 1;
        statusMessage = players[playerIndex].name + " brought Token " + to_string(tokenId + 1) + " out!";
        addActionLog(players[playerIndex].name + " brought Token " + to_string(tokenId + 1) + " out!");
        earnedExtraRoll = true;
    }
    // Moving active token
    else if (tok.status == 1) {
        tok.step += currentDiceValue;

        // Check if reached finish
        if (tok.step == FINISH_STEP) {
            tok.status = 2;
            players[playerIndex].tokensFinished++;
            statusMessage = players[playerIndex].name + "'s Token " + to_string(tokenId + 1) + " reached Finish!";
            addActionLog(players[playerIndex].name + " Token " + to_string(tokenId + 1) + " FINISHED!");
            earnedExtraRoll = true;

            moveHistory.push(rec);

            if (checkWin()) {
                return true;
            }

            isRolling = true;
            canRollAgain = false;
            statusMessage += " Roll again!";
            return true;
        }

        // On main 52-cell track: check capture
        if (tok.step >= 1 && tok.step <= 51) {
            int startCell = playerIndex * 13;
            int landingCell = (startCell + (tok.step - 1)) % TOTAL_PATH_CELLS;

            addActionLog(players[playerIndex].name + " moved Token " + to_string(tokenId + 1) + " to Step " + to_string(tok.step));

            // Check capture on non-safe cells
            if (!safeCells[landingCell]) {
                for (int op = 0; op < numPlayers; op++) {
                    if (op == playerIndex) continue;

                    for (int ot = 0; ot < TOKENS_PER_PLAYER; ot++) {
                        Token &opponentToken = players[op].tokens[ot];

                        if (opponentToken.status == 1 && opponentToken.step >= 1 && opponentToken.step <= 51) {
                            int opponentCell = (op * 13 + (opponentToken.step - 1)) % TOTAL_PATH_CELLS;

                            if (opponentCell == landingCell) {
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

    // Push record to stack
    moveHistory.push(rec);

    if (currentDiceValue == 6 || earnedExtraRoll) {
        isRolling = true;
        canRollAgain = false;
        statusMessage += " Roll again!";
    } else {
        nextTurn();
    }

    return true;
}

// Undo previous move using stack
bool undoLastMove() {
    if (moveHistory.empty()) {
        statusMessage = "No moves to undo!";
        return false;
    }

    MoveRecord rec = moveHistory.top();
    moveHistory.pop();

    if (players[rec.playerIndex].tokens[rec.tokenId].status == 2 && rec.prevStatus != 2) {
        players[rec.playerIndex].tokensFinished--;
    }

    players[rec.playerIndex].tokens[rec.tokenId].step = rec.prevStep;
    players[rec.playerIndex].tokens[rec.tokenId].status = rec.prevStatus;

    if (rec.captured) {
        players[rec.capturedPlayer].tokens[rec.capturedTokenId].status = 1;
        players[rec.capturedPlayer].tokens[rec.capturedTokenId].step = rec.capturedPrevStep;
    }

    currentPlayerIndex = rec.playerIndex;
    currentDiceValue = rec.diceValue;
    isRolling = false;
    gameOver = false;
    statusMessage = "Undid move for " + players[currentPlayerIndex].name + "! Press 1-4 to pick another.";
    addActionLog("UNDO: Restored " + players[currentPlayerIndex].name + " Token " + to_string(rec.tokenId + 1));

    return true;
}

// Advance turn to next player
void nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
    isRolling = true;
    canRollAgain = false;
    currentDiceValue = 1;
    statusMessage = players[currentPlayerIndex].name + "'s turn. Press SPACE to roll!";
}

// Check if any player has all 4 tokens finished
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

// Select token by number (1-4)
bool selectTokenById(int tokenId) {
    if (isRolling || gameOver) return false;
    return moveToken(currentPlayerIndex, tokenId);
}

// Handle clicks on dice, buttons, or board
bool selectTokenAtPosition(float mouseX, float mouseY) {
    if (gameOver) return false;

    // Click on dice or roll button
    if (isRolling) {
        if (mouseX >= 750.0f && mouseX <= 835.0f && mouseY >= 115.0f && mouseY <= 190.0f) {
            rollDice();
            return true;
        }
        if (mouseX >= 680.0f && mouseX <= 920.0f && mouseY >= 195.0f && mouseY <= 235.0f) {
            rollDice();
            return true;
        }
        return false;
    }

    // Click on token buttons on sidebar
    const float panelX = 645.0f;
    const float panelY = 50.0f;
    for (int t = 0; t < TOKENS_PER_PLAYER; t++) {
        float btnY = panelY + 285.0f + t * 35.0f;
        if (mouseX >= panelX + 15.0f && mouseX <= panelX + 285.0f &&
            mouseY >= btnY && mouseY <= btnY + 30.0f) {
            return selectTokenById(t);
        }
    }

    // Click on undo button
    if (!moveHistory.empty() && mouseX >= panelX + 15.0f && mouseX <= panelX + 285.0f &&
        mouseY >= panelY + 430.0f && mouseY <= panelY + 455.0f) {
        return undoLastMove();
    }

    // Click on token on board directly
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
