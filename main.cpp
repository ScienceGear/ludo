/*
 * LUDO GAME - MAIN ENTRY POINT (main.cpp)
 * ---------------------------------------
 * This file contains:
 * 1. Window setup (940x700).
 * 2. Font loading from Windows system fonts.
 * 3. Event loop (Keyboard and Mouse inputs).
 * 4. Game state management (Start Screen, Game Screen, Win Screen).
 */

#include "ludo.h"
#include <SFML/Graphics.hpp>

using namespace sf;

enum GameState {
    STATE_START,
    STATE_GAME,
    STATE_WIN
};

Font gameFont;

int main() {
    // Seed the random number generator for fair dice rolls
    srand((unsigned)time(NULL));

    // Try loading clean system fonts available on Windows
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

    // Create the SFML game window
    RenderWindow window(VideoMode({(unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT}), "Ludo Game - Classic 4-Player");
    window.setFramerateLimit(60);

    // Initialize board geometry and 4-player game state
    initializeBoard();
    initializeGame(4);

    GameState state = STATE_START;

    // Main Game Loop
    while (window.isOpen()) {
        // Event Polling (SFML 3 style)
        while (const std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
            // Keyboard Controls
            else if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                if (state == STATE_START) {
                    if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Space) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                }
                else if (state == STATE_GAME) {
                    // Roll Dice with SPACE
                    if (keyPressed->code == Keyboard::Key::Space) {
                        if (isRolling && !gameOver) {
                            rollDice();
                        }
                    }
                    // Select Token with 1, 2, 3, 4
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
                    // Undo last move with U (STACK data structure)
                    else if (keyPressed->code == Keyboard::Key::U) {
                        undoLastMove();
                    }
                    // Restart with R
                    else if (keyPressed->code == Keyboard::Key::R) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                    // Quit with ESC
                    else if (keyPressed->code == Keyboard::Key::Escape) {
                        window.close();
                    }
                }
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
            // Mouse Controls
            else if (const auto* mouseButton = event->getIf<Event::MouseButtonPressed>()) {
                if (mouseButton->button == Mouse::Button::Left) {
                    float mx = (float)mouseButton->position.x;
                    float my = (float)mouseButton->position.y;

                    if (state == STATE_START) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                    else if (state == STATE_GAME) {
                        selectTokenAtPosition(mx, my);
                    }
                    else if (state == STATE_WIN) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                }
            }
        }

        // Check if game was won
        if (state == STATE_GAME && (gameOver || checkWin())) {
            state = STATE_WIN;
        }

        // Render current frame
        window.clear();

        switch (state) {
            case STATE_START:
                drawStartScreen(window);
                break;
            case STATE_GAME:
                drawBoard(window);
                drawTokens(window);
                drawDice(window);
                drawUI(window);
                break;
            case STATE_WIN:
                drawBoard(window);
                drawTokens(window);
                drawDice(window);
                drawUI(window);
                drawWinScreen(window);
                break;
        }

        window.display();
    }

    return 0;
}
