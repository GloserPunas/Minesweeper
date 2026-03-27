#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

#include "Constants.h"
#include "Board.h"

// ============================================================
//  Game.h
//  State machine + event dispatch + render dispatch.
//  Không có SDL draw call nào ở đây — tất cả nằm trong
//  các helper method gọi SDL trực tiếp (chưa tách Renderer).
// ============================================================

// Button struct — đơn giản, dùng nội bộ trong Game
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

    // Khởi tạo SDL, window, renderer, font
    // Trả về false nếu bất kỳ bước nào thất bại, in lỗi ra stderr
    bool init();

    // Vòng lặp chính — chạy đến khi người dùng thoát
    void run();

private:
    // --- SDL handles ---
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font*     font     = nullptr;

    // --- App state ---
    GameState state         = GameState::MENU;
    bool      running       = false;
    int       selectedLevel = 0;

    // --- Board ---
    Board board;
    int   boardX = 0;
    int   boardY = BOARD_OFFSET_Y;

    // --- Buttons ---
    std::vector<Button> menuButtons;
    Button restartBtn;
    Button menuBtn;

    // -------------------------------------------------------
    // Init helpers
    // -------------------------------------------------------
    void buildMenuButtons();
    void buildGameButtons();

    // -------------------------------------------------------
    // Per-frame
    // -------------------------------------------------------
    void handleEvents();
    void render();

    // -------------------------------------------------------
    // Event handlers
    // -------------------------------------------------------
    void onMenuEvent    (const SDL_Event& e);
    void onGameEvent    (const SDL_Event& e);
    void onSettingEvent (const SDL_Event& e);

    // -------------------------------------------------------
    // Render helpers
    // -------------------------------------------------------
    void renderMenu    ();
    void renderGame    ();
    void renderSetting ();

    // -------------------------------------------------------
    // Utility
    // -------------------------------------------------------
    void startLevel(int levelIndex);

    void         drawButton       (const Button& btn);
    void         drawTextCentered (const std::string& text, SDL_Color color, SDL_Rect area);
    SDL_Texture* makeTextTexture  (const std::string& text, SDL_Color color) const;

    static bool  pointInRect  (int px, int py, SDL_Rect r);
    void         updateHover  (std::vector<Button>& buttons, int mx, int my);
};