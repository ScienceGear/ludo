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
    srand((unsigned)time(NULL));

    // Load available system font
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
        cerr << "Warning: Could not load default font." << endl;
    }

    RenderWindow window(VideoMode({(unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT}), "Ludo Game - Classic 4-Player");
    window.setFramerateLimit(60);

    initializeBoard();
    initializeGame(4);

    GameState state = STATE_START;

    while (window.isOpen()) {
        // Poll input events
        while (const std::optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
            // Keyboard controls
            else if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                if (state == STATE_START) {
                    if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Space) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
                }
                else if (state == STATE_GAME) {
                    if (keyPressed->code == Keyboard::Key::Space) {
                        if (isRolling && !gameOver) {
                            rollDice();
                        }
                    }
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
                    else if (keyPressed->code == Keyboard::Key::U) {
                        undoLastMove();
                    }
                    else if (keyPressed->code == Keyboard::Key::R) {
                        initializeGame(4);
                        state = STATE_GAME;
                    }
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
            // Mouse controls
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

        if (state == STATE_GAME && (gameOver || checkWin())) {
            state = STATE_WIN;
        }

        // Render current state
        window.clear();

        if (state == STATE_START) {
            drawStartScreen(window);
        }
        else if (state == STATE_GAME) {
            drawBoard(window);
            drawTokens(window);
            drawDice(window);
            drawUI(window);
        }
        else if (state == STATE_WIN) {
            drawBoard(window);
            drawTokens(window);
            drawDice(window);
            drawUI(window);
            drawWinScreen(window);
        }

        window.display();
    }

    return 0;
}
