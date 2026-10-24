# Ludo Game (C++ & SFML)

A 4-player Ludo board game implemented in **C++** using **SFML** for graphical display. The project uses custom data structures built from scratch to manage game state, move history, and roll logs.

---

## 1. Overview
* Classic 4-player Ludo rules (Red, Green, Yellow, Blue).
* Each player has 4 tokens navigating a 52-cell track toward the center finish.
* Visual interface rendered with SFML 2D shapes and text.
* Features include capture mechanics, bonus turns, and move undo.

---

## 2. Rules of the Game
- **Base Exit:** A player must roll a **6** to move a token from home base to the start position.
- **Track Movement:** Tokens advance clockwise around the perimeter by the rolled dice value.
- **Safe Cells:** Marked with star (`*`) or Start (`S`). Tokens on safe cells cannot be captured.
- **Capturing:** Landing on an opponent's token on an unsafe cell sends that token back to its home base and awards a bonus roll.
- **Bonus Turns:** Awarded on rolling a 6, capturing an opponent token, or reaching the center finish.
- **Home Path:** After completing a full circuit, tokens enter their private colored corridor.
- **Winning:** Tokens must land on the center finish with an exact roll. The first player to bring all 4 tokens to the center wins.

---

## 3. Controls

| Action | Input |
|---|---|
| **Roll Dice** | `SPACE` or click Dice panel |
| **Move Token** | `1`, `2`, `3`, `4` or click token / button |
| **Undo Move** | `U` or click Undo button |
| **Restart** | `R` |
| **Quit** | `ESC` |
| **Start Game** | `ENTER` or `SPACE` |

---

## 4. Compilation & Execution

### Prerequisites
- GCC / MinGW C++ compiler (C++17 support)
- SFML 3 (required runtime DLLs are provided in the repository)

### Build Command:
```bash
g++ -std=c++17 main.cpp game.cpp board.cpp -o ludo.exe -lsfml-graphics -lsfml-window -lsfml-system
```

### Run Command:
```bash
./ludo.exe
```

---

## 5. Board Architecture & Coordinate System

The board is mapped to a **15 × 15 grid** (`CELL_SIZE = 40 pixels`):
1. **Four Corner Bases (6×6 cells each):**
   - Top-Left: Red Base
   - Top-Right: Green Base
   - Bottom-Right: Yellow Base
   - Bottom-Left: Blue Base
2. **Four Arms (6×3 cells each):**
   - 52-cell circular path shared by all players.
   - 4 colored start cells and 4 safe star cells.
   - 5-cell private corridor for each player.
3. **Center Finish (3×3 cells):**
   - 4 colored triangular zones meeting at the center finish circle.

---

## 6. Token Progress (`step` System)

Each token tracks its board position using an integer index `step` (0 to 57):
```
[step = 0]        Home base (waiting for roll of 6)
[step = 1]        Player start cell
[step = 1 to 51]  Active on outer 52-cell track
                  Board cell = (playerStartCell + step - 1) % 52
[step = 52 to 56] Private colored home corridor
[step = 57]       Finished (center goal reached)
```

---

## 7. Data Structures

The project implements custom data structure classes without relying on STL container adapters:

| Data Structure | Class / Type | Usage | Description |
|---|---|---|---|
| **Queue** | `CustomQueue` | Dice roll history | FIFO linked queue storing the last 5 dice rolls displayed on the UI sidebar. |
| **Stack** | `CustomStack` | Move history / Undo | LIFO linked stack storing `MoveRecord` states for rolling back turns on pressing `U`. |
| **Linked List** | `CustomLinkedList` | Event log | Singly linked list storing the latest game action messages. |
| **1D Arrays** | `boardPathX[52]`, `boardPathY[52]` | Board path lookup | Static coordinate map for the 52 perimeter cells. |
| **2D Arrays** | `corridorX[4][5]`, `corridorY[4][5]` | Home corridor lookup | Grid coordinates for each player's 5-cell corridor. |
| **Boolean Array**| `safeCells[52]` | Capture checks | Lookup table marking star and start cells as protected from captures. |
| **Structs** | `Token`, `Player`, `MoveRecord` | State management | Encapsulates token attributes, player data, and snapshot records for undo operations. |

---

## 8. Module Division

| File | Primary Responsibility |
|---|---|
| `ludo.h` | Constants, struct definitions, custom data structures, and function declarations. |
| `board.cpp` | Board layout math, coordinate tables, and SFML rendering functions. |
| `game.cpp` | Core rules, movement validation, capture logic, turn cycle, and undo handling. |
| `main.cpp` | Application entry point, window management, and input polling loop. |
