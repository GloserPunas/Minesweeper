#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

#include "Constants.h"
#include "Board.h"

struct Button {
    SDL_Rect    rect     = {0, 0, 0, 0};
    std::string label;
    SDL_Color   bgColor  = Color::BTN_NORMAL;
    SDL_Color   txtColor = Color::BLACK;
    bool        hover    = false;
};

class Game {
public:
    Game()  = default;
    ~Game();

    bool init();

    void run();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    GameState state = GameState::MENU;
    bool running = false;
    int selectedLevel = 0;
    bool flagMode = false;

    Board board;
    int boardX = 0;
    int boardY = BOARD_OFFSET_Y;

    std::vector<Button> menuButtons;
    Button restartBtn;
    Button menuBtn;
    Button flagBtn;

    void buildMenuButtons();
    void buildGameButtons();

    void handleEvents();
    void render();

    void onMenuEvent (const SDL_Event& e);
    void onGameEvent (const SDL_Event& e);
    void onSettingEvent (const SDL_Event& e);

    void renderMenu ();
    void renderGame ();
    void renderSetting ();

    void startLevel(int levelIndex);

    void drawButton (const Button& btn);
    void drawTextCentered (const std::string& text, SDL_Color color, SDL_Rect area);
    SDL_Texture* makeTextTexture (const std::string& text, SDL_Color color) const;

    static bool pointInRect (int px, int py, SDL_Rect r);
    void updateHover (std::vector<Button>& buttons, int mx, int my);
};