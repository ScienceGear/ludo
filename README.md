# LUDO GAME (C++ & SFML)

A simple, classic 4-player Ludo game built in **C++** using **SFML** for 2D graphics. Everything is drawn cleanly using geometric shapes (rectangles, circles, triangles) and text with no external images required.

---

## 1. What is Ludo? (Easy 3-Line Summary)
* Ludo is a traditional board game played by 2 to 4 players (Red, Green, Yellow, Blue).
* Each player has 4 tokens and takes turns rolling a 6-sided dice to race their tokens around the board.
* The first player to safely guide all 4 tokens into the central home finish wins the game!

---

## 2. Core Rules
- **Starting:** Roll a **6** to bring a token out of your home base onto the start cell.
- **Movement:** Active tokens advance clockwise around the 52-cell track by the dice roll number.
- **Bonus Rolls:** A player gets an extra roll if they:
  1. Roll a **6**.
  2. Capture an opponent's token.
  3. Reach the center finish.
- **Safe Cells:** Marked with a star (`*`) or Start (`S`). Tokens on safe cells cannot be captured.
- **Capturing:** If a token lands on an opponent's token on an unsafe cell, the opponent's token is captured and sent back to its home base!
- **Home Corridor:** After completing a full circuit around the board, tokens enter their private colored 5-cell corridor where opponents cannot follow.
- **Winning:** Tokens must land on the center finish with an exact dice roll. The first player with all 4 tokens finished wins!

---

## 3. How to Compile and Run

### Requirements
- **GCC / MinGW C++ Compiler** (supporting C++17)
- **SFML 3** (the required DLLs are already included in the project folder)

### Compile Command:
```bash
g++ -std=c++17 main.cpp game.cpp board.cpp -o ludo.exe -lsfml-graphics -lsfml-window -lsfml-system
```

### Run Command:
```bash
./ludo.exe
```

---

## 4. Game Controls

| Action | Control |
|---|---|
| **Roll Dice** | Press `SPACE` or Click the Dice box |
| **Move Token** | Press `1`, `2`, `3`, `4` or Click Token / Button |
| **Restart Game** | Press `R` |
| **Quit Game** | Press `ESC` |
| **Start from Title** | Press `ENTER` |

---

## 5. How the Board is Built (Simple Explanation for Mam)

The entire board is built on a standard **15 × 15 grid** (`CELL_SIZE = 40 pixels`):
1. **Four Corner Bases (6×6 cells each):**
   - Top-Left: **Red Base**
   - Top-Right: **Green Base**
   - Bottom-Right: **Yellow Base**
   - Bottom-Left: **Blue Base**
   - Each base has a large colored square with an inner white area holding 4 circular token slots.
2. **Four Arms (6×3 cells each):**
   - Contains the 52-cell white walking track.
   - Includes the 4 colored start cells (`S`) and 4 safe star cells (`*`).
   - Contains the 5-cell colored home corridor for each player.
3. **Center Finish (3×3 cells in the middle):**
   - 4 colored triangles (Red, Green, Yellow, Blue) meeting at a central golden circle where tokens finish.

---

## 6. How Token Movement Works (`step` Variable: 0 to 57)

To keep the project easy to understand, each token tracks its progress using a single integer: **`step`**:

```
[step = 0]       -> Inside Home Base (waiting for a 6)
[step = 1]       -> At Player's Start Cell (exited base)
[step = 1 to 51] -> Moving clockwise around the 52-cell board
                    Cell Index on Board = (PlayerStart + step - 1) % 52
[step = 52 to 56]-> Moving up the Player's Private Colored Corridor
[step = 57]      -> CENTER FINISH! Token is finished.
```

### Why this is so easy to explain:
- When a token is at home, rolling a 6 sets `step = 1`.
- When an active token moves, we simply do: `step += diceValue`.
- If `step + diceValue > 57`, the token cannot move (needs exact roll).
- When `step == 57`, the token is marked finished (`tokensFinished++`).
- When all 4 tokens reach 57, that player wins!

---

## 7. Data Structures Used (Detailed Explanation for Mam)

| Data Structure | Implementation | Where Used | Why Chosen |
|---|---|---|---|
| **QUEUE (FIFO)** | `std::queue<int> diceHistory` | `rollDice()`, displayed on UI | **First-In, First-Out (FIFO):** Stores the last 5 dice rolls. Each new roll is pushed to the back (`push`), and the oldest roll is popped from the front (`pop`) when size > 5. Displayed on the right sidebar as a rolling history. |
| **STACK (LIFO)** | `std::stack<MoveRecord> moveHistory` | `moveToken()`, `undoLastMove()` | **Last-In, First-Out (LIFO):** Powers the **Undo feature (Press 'U')**! Every move is pushed onto the stack. Pressing 'U' pops the most recent move from the top (`pop`) and restores the previous token position and turn. |
| **LINKED LIST** | `std::list<string> actionLog` | `addActionLog()`, displayed on UI | **Doubly Linked List:** Efficiently appends event log messages (`push_back`) and removes old logs from the head (`pop_front`) in O(1) time without reallocating contiguous memory. |
| **1D ARRAYS** | `boardPathX[52], boardPathY[52]` | Board path lookup | Fixed-size array storing grid coordinates for the 52 main path cells. Provides instant **O(1)** indexing. |
| **2D ARRAYS (Matrices)** | `corridorX[4][5], corridorY[4][5]` | Home corridor lookup | 2D matrix storing the 5 colored corridor cells for each of the 4 players. |
| **BOOLEAN ARRAY** | `safeCells[52]` | Capture checks | Fast **O(1)** lookup to check if a landing cell is a protected safe star or start cell. |
| **STRUCTS** | `Token`, `Player`, `MoveRecord` | State encapsulation | Groups related data members (id, step, status, colors) into structured objects. |

---

## 8. Presentation / Viva Questions & Answers (Cheat Sheet for Mam!)

**Q1: What data structures did you use in this project?**  
> *"Mam, we used three core data structures from the C++ standard library:  
> 1. **Queue (`std::queue`)** for the rolling dice history (FIFO).  
> 2. **Stack (`std::stack`)** for the move history to implement the Undo feature (LIFO).  
> 3. **Linked List (`std::list`)** for the live game event log.  
> In addition, we used 1D and 2D arrays for board cell lookups and custom C++ structs for Tokens and Players."*

**Q2: How does the Stack work in your game?**  
> *"Mam, the Stack works on Last-In, First-Out (LIFO). Whenever a player moves a token, we push a `MoveRecord` containing the previous step and status onto the stack. If the player presses 'U', we pop the top of the stack and restore the previous state, undoing the move."*

**Q3: How does the Queue work in your game?**  
> *"Mam, the Queue works on First-In, First-Out (FIFO). Whenever the dice is rolled, we push the new roll to the back. When the queue size exceeds 5, we pop the oldest roll from the front. This maintains a rolling display of recent rolls on the sidebar."*

**Q4: How do you represent the Ludo board on the screen?**  
> *"Mam, we divided the board into a 15 by 15 mathematical grid of 40-pixel squares. The 4 corners are 6x6 squares for player bases, the 4 arms are 6x3 rectangles for the tracks, and the center is 3x3 with 4 triangles meeting at the finish."*

**Q2: How do tokens move around the board?**  
> *"Each token has a `step` variable from 0 to 57. Step 0 is the base. Rolling a 6 moves it to step 1. Steps 1 to 51 move along the main circular path. Steps 52 to 56 move up the player's colored corridor, and step 57 is the center finish."*

**Q3: How does capture detection work?**  
> *"When a token moves, we calculate which board cell it lands on using `(startCell + step - 1) % 52`. If that cell is not marked safe in our `safeCells` array, we check if any opponent token is on that exact cell. If so, the opponent's `step` and `status` are reset to 0 (sent back to base), and the player earns a bonus roll."*

**Q4: How do you prevent tokens from overshooting the finish?**  
> *"Before moving, we check `if (step + diceValue <= 57)`. If the roll would exceed 57, the move is invalid and the token cannot be moved."*

**Q5: How does the game detect a winner?**  
> *"Each player tracks `tokensFinished`. When a token reaches step 57, `tokensFinished` increments. When any player reaches 4 finished tokens, `checkWin()` returns true and displays the victory screen."*

---

## 9. Team Member Division

| Member | Module | Key Responsibility |
|---|---|---|
| **Member 1** | `ludo.h` | Data structures (`Token`, `Player`), constants, and function prototypes. |
| **Member 2** | `board.cpp` (Layout) | 15x15 grid layout, 52-cell coordinate mapping, and safe cells. |
| **Member 3** | `board.cpp` (Drawing) | Drawing base boxes, triangles, tokens, dice dots, and sidebar UI. |
| **Member 4** | `game.cpp` (Game Logic) | Dice rolling, `step` advancement, capture rules, and win detection. |
| **Member 5** | `main.cpp` + Documentation | SFML event loop, keyboard & mouse controls, and presentation guide. |

---

*Developed for college programming lab project demonstration.*
