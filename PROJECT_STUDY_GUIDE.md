# 📘 Ludo Game Project - Complete Learning & Viva Guide

Welcome! This guide is written in **simple, easy-to-understand language** so you can learn exactly how this Ludo game is built, understand every line of code, and confidently explain the whole project to your teacher ("mam") to score top marks!

---

## 📑 Table of Contents
1. [Project Overview](#1-project-overview)
2. [File Organization (How the Code is Split)](#2-file-organization)
3. [The 15x15 Board Math (Explained Visually)](#3-the-15x15-board-math)
4. [The Movement System (The Step 0-57 Magic)](#4-the-movement-system)
5. [The 4 Data Structures Used (Crucial for Viva!)](#5-the-4-data-structures-used)
6. [Step-by-Step Code Walkthrough](#6-step-by-step-code-walkthrough)
7. [Word-for-Word Presentation Script for Mam](#7-word-for-word-presentation-script-for-mam)
8. [Top 10 Viva Questions & Perfect Answers](#8-top-10-viva-questions--perfect-answers)

---

## 1. Project Overview

* **Game:** Classic 4-Player Ludo (Red, Green, Yellow, Blue).
* **Language:** C++ (C++17 standard).
* **Graphics Library:** SFML (Simple and Fast Multimedia Library).
* **Key Feature:** Everything is drawn using **pure mathematical shapes** (Rectangles, Circles, Triangles, Text). No external images or textures are needed!
* **Controls:** 
  - `SPACE` or Click Dice = Roll.
  - Number keys `1`, `2`, `3`, `4` or Click Token/Button = Move.
  - `U` = **Undo Move** (using Stack).
  - `R` = Restart, `ESC` = Quit.

---

## 2. File Organization

The project is cleanly split into 4 files so each team member has a specific role:

| File Name | Purpose | What's Inside? |
|---|---|---|
| **`ludo.h`** | Header File | Declares all constants, structs (`Token`, `Player`, `MoveRecord`), and function prototypes. |
| **`board.cpp`** | Board & Graphics | Contains board layout, coordinate tables, and all drawing code (board, tokens, dice, sidebar). |
| **`game.cpp`** | Game Logic & Rules | Handles dice rolling, movement rules, capturing, Undo (Stack), Queue, and win checks. |
| **`main.cpp`** | Entry Point & Events | Creates window, loads fonts, runs the 60 FPS game loop, and handles keyboard/mouse clicks. |

---

## 3. The 15x15 Board Math

Every classic Ludo board in the world is a **15 × 15 grid**:
* Each cell has a width and height of **40 pixels** (`CELL_SIZE = 40.0f`).
* 15 cells × 40 px = **600 × 600 pixels** total board size.

```
       0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
     +-----------------------+---+---+---+-----------------------+
   0 |                       |   |   |   |                       |
   1 |      RED BASE         |   | G |   |      GREEN BASE       |
   2 |     (6x6 cells)       |   | R |   |     (6x6 cells)       |
   3 |                       |   | E |   |                       |
   4 |                       |   | E |   |                       |
   5 |                       |   | N |   |                       |
     +-----------------------+---+---+---+-----------------------+
   6 |   | S |   |   |   |   |   |   |   |   |   | * |   |   |   |
   7 |   | R | E | D | H | O | M | E | ! | Y | E | L | L | O | W |
   8 |   |   | * |   |   |   |   |   |   |   |   | S |   |   |   |
     +-----------------------+---+---+---+-----------------------+
   9 |                       |   | B |   |                       |
  10 |      BLUE BASE        |   | L |   |     YELLOW BASE       |
  11 |     (6x6 cells)       |   | U |   |     (6x6 cells)       |
  12 |                       |   | E |   |                       |
  13 |                       |   |   |   |                       |
  14 |                       |   |   |   |                       |
     +-----------------------+---+---+---+-----------------------+
```

### The 3 Zones:
1. **4 Corner Bases (6×6 cells each):**
   - Top-Left: **Red** (cols 0-5, rows 0-5)
   - Top-Right: **Green** (cols 9-14, rows 0-5)
   - Bottom-Right: **Yellow** (cols 9-14, rows 9-14)
   - Bottom-Left: **Blue** (cols 0-5, rows 9-14)
2. **4 Arms (6×3 cells each):**
   - Contains the 52 white cells of the outer circular track.
   - Contains the 4 colored start cells (`S`) and 4 star-marked safe cells (`*`).
   - Contains the 5 colored corridor cells for each player leading towards center.
3. **Center Finish (3×3 cells = cols 6-8, rows 6-8):**
   - 4 colored triangles meeting at the center point `(7.5, 7.5)` with a golden circle.

### How Grid Coordinates Become Pixels:
```cpp
Vector2f getGridPixel(int gx, int gy) {
    float x = BOARD_X + gx * CELL_SIZE + CELL_SIZE / 2.0f;
    float y = BOARD_Y + gy * CELL_SIZE + CELL_SIZE / 2.0f;
    return Vector2f(x, y);
}
```
*If gx = 1 and gy = 6, it instantly gives the center pixel coordinate!*

---

## 4. The Movement System

To make the code clean and impossible to break, we track each token using a single integer: **`step`**:

```
[step = 0]        Inside Home Base (Waiting for a 6)
      ↓ (roll a 6)
[step = 1]        Player's Start Cell on the board
      ↓ (advance by dice value)
[step = 1 to 51]  Moving clockwise around the 52-cell outer track
      ↓ 
[step = 52 to 56] Inside player's Private Colored Corridor (safe from all opponents)
      ↓ (exact roll needed)
[step = 57]       CENTER FINISH! (Token is safely home)
```

### Formula for Board Cell Index:
When `1 <= step <= 51`, which cell on the 52-cell board is the token standing on?
$$\text{boardCell} = (\text{playerStartCell} + (\text{step} - 1)) \pmod{52}$$

* Red starts at index `0`.
* Green starts at index `13` ($0 + 13$).
* Yellow starts at index `26` ($13 + 13$).
* Blue starts at index `39` ($26 + 13$).

Every player's start is spaced **exactly 13 cells apart**, making the loop mathematically symmetrical!

---

## 5. The 4 Data Structures Used (Crucial for Viva!)

Teachers love when you can clearly identify each data structure and explain **why** it was chosen:

### 1. QUEUE (`std::queue<int> diceHistory`) — FIFO (First-In, First-Out)
* **What it does:** Stores a rolling history of the last 5 dice rolls.
* **Why Queue?** Just like people standing in a ticket line, the first dice roll that enters is the first one to be removed when the line is full.
* **Code in `game.cpp`:**
  ```cpp
  diceHistory.push(roll);            // Add newest roll to back
  if (diceHistory.size() > 5) {
      diceHistory.pop();             // Remove oldest roll from front
  }
  ```

### 2. STACK (`std::stack<MoveRecord> moveHistory`) — LIFO (Last-In, First-Out)
* **What it does:** Powers the **Undo** feature (Press `U` to undo a move).
* **Why Stack?** The last move you made is the first move you want to undo (LIFO - like a stack of plates).
* **Code in `game.cpp`:**
  ```cpp
  // 1. When moving, push the record onto the stack:
  moveHistory.push(record);

  // 2. When pressing 'U', pop from top and reverse:
  MoveRecord last = moveHistory.top();
  moveHistory.pop();
  players[last.playerIndex].tokens[last.tokenId].step = last.prevStep;
  ```

### 3. LINKED LIST (`std::list<string> actionLog`) — Dynamic Chain of Nodes
* **What it does:** Displays the live game events log on the sidebar (e.g. *"Red rolled 6"*, *"Red captured Blue"*).
* **Why Linked List?** Adding items to the tail (`push_back`) and removing old items from the head (`pop_front`) is **$O(1)$ constant time**, without resizing or shifting array memory.
* **Code in `game.cpp`:**
  ```cpp
  actionLog.push_back(msg);
  if (actionLog.size() > 4) {
      actionLog.pop_front();
  }
  ```

### 4. 1D & 2D ARRAYS (Matrices)
* **1D Arrays (`boardPathX[52]`, `boardPathY[52]`):** Fixed tables of grid coordinates for fast **$O(1)$** lookup.
* **2D Array (`corridorX[4][5]`, `corridorY[4][5]`):** Matrix of 4 rows (players) and 5 columns (corridor cells).
* **Boolean Array (`safeCells[52]`):** Marks which cells are safe from capture.

---

## 6. Step-by-Step Code Walkthrough

### How the Game Loop Works in `main.cpp`:
```
[Start Program]
      ↓
[Initialize Window (960x700) & Load Windows Fonts]
      ↓
[initializeBoard() -> build coordinates & safe cells]
[initializeGame(4) -> set 4 players & 16 tokens at step 0]
      ↓
┌───> [Poll Events: Keyboard & Mouse]
│       - If SPACE: call rollDice()
│       - If 1, 2, 3, 4: call selectTokenById()
│       - If U: call undoLastMove() (Stack Pop!)
│       - If R: reset game
│     [Check Win Condition: if checkWin() -> STATE_WIN]
│     [Render Frame: drawBoard(), drawTokens(), drawDice(), drawUI()]
└───  [Display Window] (Runs at 60 FPS)
```

### Capture Detection in `game.cpp`:
```cpp
// 1. Calculate which cell the moving token lands on
int landingCell = (playerStart + (tok.step - 1)) % 52;

// 2. Safe cells protect tokens from being captured!
if (!safeCells[landingCell]) {
    // 3. Check all opponent tokens
    for (int op = 0; op < numPlayers; op++) {
        if (op == currentPlayerIndex) continue;
        for (int ot = 0; ot < 4; ot++) {
            if (opponentToken lands on landingCell) {
                // CAPTURE! Send opponent back to base
                opponentToken.step = 0;
                opponentToken.status = 0;
                earnedExtraRoll = true; // Reward!
            }
        }
    }
}
```

---

## 7. Word-for-Word Presentation Script for Mam

*(You can memorize or read this exact script during your project presentation!)*

> *"Good morning / afternoon, Mam.*  
>  
> *Today, our team presents our classic **4-Player Ludo Game**, developed in **C++** using **SFML** for 2D graphics.*  
>  
> *First, regarding the board: we modeled the standard Ludo board on a **15 by 15 mathematical grid**. The 4 corners are 6x6 squares for player bases, the 4 arms are 6x3 rectangles containing the 52-cell track, and the center is a 3x3 finish area with 4 colored triangles meeting at the center finish.*  
>  
> *Second, for token movement, we implemented an elegant **Step System from 0 to 57**:*  
> - *Step 0 means the token is inside its home base.*  
> - *Rolling a 6 moves it out to Step 1 (its start cell).*  
> - *Steps 1 to 51 navigate the circular board using modulo 52 arithmetic.*  
> - *Steps 52 to 56 lead up the player's private colored home corridor.*  
> - *And Step 57 is the Center Finish!*  
>  
> *Third, we incorporated all the core Data Structures taught in our syllabus:*  
> 1. *We used a **Queue (`std::queue`)** to maintain a FIFO sliding window of the last 5 dice rolls shown on our UI.*  
> 2. *We used a **Stack (`std::stack`)** to implement the **Undo feature** (pressing 'U'), which operates on LIFO to pop and reverse the most recent move.*  
> 3. *We used a **Linked List (`std::list`)** to log live game events dynamically.*  
> 4. *We used **1D and 2D arrays** for constant-time $O(1)$ lookups of board coordinates and safe cells.*  
>  
> *The game supports both keyboard shortcuts and full mouse interaction, features safe cell protection, capture rewards, and automated win detection.*  
>  
> *Thank you, Mam! We are happy to demonstrate the gameplay and answer any questions."*

---

## 8. Top 10 Viva Questions & Perfect Answers

#### Q1: What data structures did you use in this project?
**Answer:**  
> *"Mam, we used 4 primary data structures:*  
> *1. **Queue (`std::queue`)** for the rolling dice history (FIFO).*  
> *2. **Stack (`std::stack`)** for the move history to implement Undo (LIFO).*  
> *3. **Linked List (`std::list`)** for the dynamic game events log.*  
> *4. **1D & 2D Arrays** for the 52-cell track coordinates and home corridors."*

#### Q2: Can you demonstrate how your Stack works?
**Answer:**  
> *"Yes, Mam! When any player moves a token, a `MoveRecord` struct containing previous positions is pushed onto `moveHistory` using `push()`. When we press the 'U' key, `moveHistory.top()` retrieves the last action, we reverse the token positions, and call `moveHistory.pop()`. This is a classic Last-In, First-Out (LIFO) stack application."*

#### Q3: Why did you use a Queue for dice history instead of an Array?
**Answer:**  
> *"Because dice history represents a First-In, First-Out (FIFO) stream. When a new roll arrives, it enters from the back (`push`), and when the count exceeds 5, the oldest roll drops off the front (`pop`). A queue naturally enforces this without manually shifting array elements."*

#### Q4: Why did you use a Linked List for the event log?
**Answer:**  
> *"A Linked List (`std::list`) allows $O(1)$ insertions at the end and $O(1)$ deletions from the front without needing to reallocate contiguous blocks of memory as vectors or arrays would."*

#### Q5: How do you mathematically map the 52-cell circular track?
**Answer:**  
> *"We use modulo arithmetic: `cellIndex = (playerStart + (step - 1)) % 52`. Since the track has 52 cells, using `% 52` guarantees that after cell 51, the token wraps smoothly around to cell 0."*

#### Q6: How do you prevent a token from overshooting the finish?
**Answer:**  
> *"Before moving, we check: `if (tok.step + currentDiceValue <= 57)`. If the sum exceeds 57, the move is invalid because Ludo requires an exact dice roll to enter home."*

#### Q7: How does capture detection work?
**Answer:**  
> *"When a token lands on a cell, we check our boolean `safeCells` array. If the cell is safe (a start cell or star cell), no capture is permitted. If it is unsafe, we loop through all opponent tokens on the board. If an opponent is on the exact same cell, we reset their `step = 0` (sending them back to base) and grant the current player a bonus roll."*

#### Q8: How is the board drawn without any external image files?
**Answer:**  
> *"All visual components are drawn procedurally using SFML primitive shapes: `sf::RectangleShape` for square cells and base boxes, `sf::CircleShape` for tokens and dice dots, and `sf::ConvexShape` for the 4 center finish triangles."*

#### Q9: How do you manage circular player turns?
**Answer:**  
> *"In `nextTurn()`, we simply do `currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers`. For 4 players, it cycles cleanly: 0 → 1 → 2 → 3 → 0."*

#### Q10: How does the game decide who won?
**Answer:**  
> *"Each player has a counter `tokensFinished`. When a token reaches step 57, `tokensFinished` increments. When any player's count reaches 4, `checkWin()` returns true and triggers the victory screen."*

---

*Good luck with your project and presentation! You are now 100% prepared!*
