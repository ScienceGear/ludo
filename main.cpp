/*
 * LUDO GAME - MAIN ENTRY POINT (main.cpp)
 * =======================================
 * This file is the "driver" or entry point of the entire application.
 * 
 * WHAT THIS FILE DOES:
 * 1. Creates the SFML game window (960 x 700 pixels).
 * 2. Loads a system font for rendering text.
 * 3. Initializes the board math and the 4-player game state.
 * 4. Runs the 60 FPS Game Loop:
 *    - Process Inputs (Keyboard SPACE/1-4/U/R/ESC and Mouse Clicks)
 *    - Update Game State
 *    - Render / Draw Everything on Screen
 * 
 * HOW TO EXPLAIN THIS FILE TO MAM:
 * "Mam, main.cpp starts the game window, listens for player input (keyboard & mouse),
 * and continuously calls the drawing functions 60 times per second to update the screen."
 */

#include "ludo.h"
#include <SFML/Graphics.hpp>

using namespace sf;

// 3 Game States: Start Screen -> Playing Game -> Victory Screen
enum GameState {
    STATE_START,   // Showing welcome screen
    STATE_GAME,    // Playing active game
    STATE_WIN      // Someone won, showing victory popup
};

// Global font used across the entire game
Font gameFont;

int main() {
    // 1. Seed random number generator with current time for fair dice rolls
    srand((unsigned)time(NULL));

    // 2. Load standard Windows font (Segoe UI or Arial)
    const char* fontPaths[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    bool fontLoaded = false;
    for (const char* path : fontPaths) {
        if (gameFont.openFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    if (!fontLoaded) {
        cerr << "Warning: Could not load default Windows font." << endl;
    }

    // 3. Create game window (960 x 700) and limit frame rate to 60 FPS
    RenderWindow window(VideoMode({(unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT}), "Ludo Game - Classic 4-Player");
    window.setFramerateLimit(60);

    // 4. Initialize Board layout (board.cpp) and Game State (game.cpp)
    initializeBoard();
    initializeGame(4);

    GameState state = STATE_START;

    // ==================== MAIN GAME LOOP (Runs 60 times/sec) ====================
    while (window.isOpen()) {

        // --- STEP 1: EVENT HANDLING (Keyboard & Mouse) ---
        while (const std::optional event = window.pollEvent()) {
            // Close button clicked (X)
            if (event->is<Event::Closed>()) {
                window.close();
            }

            // Keyboard Press Events
            else if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {

                // State: Start Screen -> Press Enter or Space to play
                if (state == STATE_START) {
                    if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Space) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                }

                // State: Playing Game
                else if (state == STATE_GAME) {
                    // SPACE = Roll Dice
                    if (keyPressed->code == Keyboard::Key::Space) {
                        if (isRolling && !gameOver) {
                            rollDice();
                        }
                    }
                    // Keys 1, 2, 3, 4 = Move Token 1, 2, 3, or 4
                    else if (keyPressed->code == Keyboard::Key::Num1 || keyPressed->code == Keyboard::Key::Numpad1) {
                        selectTokenById(0);
                    }
                    else if (keyPressed->code == Keyboard::Key::Num2 || keyPressed->code == Keyboard::Key::Numpad2) {
                        selectTokenById(1);
                    }
                    else if (keyPressed->code == Keyboard::Key::Num3 || keyPressed->code == Keyboard::Key::Numpad3) {
                        selectTokenById(2);
                    }
                    else if (keyPressed->code == Keyboard::Key::Num4 || keyPressed->code == Keyboard::Key::Numpad4) {
                        selectTokenById(3);
                    }
                    // Key U = UNDO Last Move (STACK LIFO Operation)
                    else if (keyPressed->code == Keyboard::Key::U) {
                        undoLastMove();
                    }
                    // Key R = Restart Game
                    else if (keyPressed->code == Keyboard::Key::R) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                    // Key ESC = Exit Game
                    else if (keyPressed->code == Keyboard::Key::Escape) {
                        window.close();
                    }
                }

                // State: Win Screen -> R to replay or ESC to quit
                else if (state == STATE_WIN) {
                    if (keyPressed->code == Keyboard::Key::R) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                    else if (keyPressed->code == Keyboard::Key::Escape) {
                        window.close();
                    }
                }
            }

            // Mouse Click Events
            else if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseButton->button == Mouse::Button::Left) {
                    float mx = (float)mouseButton->position.x;
                    float my = (float)mouseButton->position.y;

                    if (state == STATE_START) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                    else if (state == STATE_GAME) {
                        // Click on dice, buttons, or board tokens
                        selectTokenAtPosition(mx, my);
                    }
                    else if (state == STATE_WIN) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                }
            }
        }

        // --- STEP 2: CHECK WIN CONDITION ---
        if (state == STATE_GAME && (gameOver || checkWin())) {
            state = STATE_WIN;
        }

        // --- STEP 3: DRAWING / RENDERING ---
        window.clear();

        if (state == STATE_START) {
            drawStartScreen(window);
        }
        else if (state == STATE_GAME) {
            drawBoard(window);   // 1. Draw 15x15 board grid & base boxes
            drawTokens(window);  // 2. Draw 16 tokens with step positions
            drawDice(window);    // 3. Draw 3D dice with dots
            drawUI(window);      // 4. Draw sidebar with Queue, Stack, and Event log
        }
        else if (state == STATE_WIN) {
            drawBoard(window);
            drawTokens(window);
            drawDice(window);
            drawUI(window);
            drawWinScreen(window); // 5. Draw Victory popup on top
        }

        window.display();
    }

    return 0;
}