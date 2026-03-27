#pragma once
#include <SDL.h>
#include <string>

constexpr int WINDOW_WIDTH = 500;
constexpr int WINDOW_HEIGHT = 600;
constexpr int CELL_SIZE = 32;
constexpr int HEADER_HEIGHT = 60;
constexpr int BOARD_OFFSET_Y = HEADER_HEIGHT + 10;

constexpr int TARGET_FPS = 60;
constexpr int FRAME_DELAY_MS = 1000 / TARGET_FPS;

constexpr int FONT_SIZE_NORMAL = 22;
constexpr int FONT_SIZE_TITLE = 28;
constexpr int FONT_SIZE_CELL = 18;

enum class GameState {
    MENU,
    GAME,
    SETTING,
    QUIT
};

struct LevelConfig {
    std::string name;
    int rows, cols, bombs;
};

constexpr int NUM_LEVELS = 3;

inline const LevelConfig LEVELS[NUM_LEVELS] = {
    { "Beginner",     9,  9,  10 },
    { "Intermediate", 16, 16, 40 },
    { "Expert",       16, 30, 99 },
};

namespace Color {
    constexpr SDL_Color BG          = { 220, 220, 220, 255 };
    constexpr SDL_Color BLACK       = {   0,   0,   0, 255 };
    constexpr SDL_Color WHITE       = { 255, 255, 255, 255 };
    constexpr SDL_Color TITLE       = {  40,  40,  90, 255 };

    constexpr SDL_Color BTN_NORMAL  = { 120, 180, 220, 255 };
    constexpr SDL_Color BTN_HOVER   = { 160, 200, 240, 255 };
    constexpr SDL_Color BTN_GAME    = { 200, 200, 250, 255 };
    constexpr SDL_Color BTN_BORDER  = {  80, 120, 160, 255 };

    constexpr SDL_Color CELL_HIDDEN = { 180, 180, 180, 255 };
    constexpr SDL_Color CELL_SHOWN  = { 240, 240, 240, 255 };
    constexpr SDL_Color CELL_BOMB   = { 255,  80,  80, 255 };
    constexpr SDL_Color CELL_FLAG   = { 255, 220, 100, 255 };
    constexpr SDL_Color BORDER      = { 120, 120, 120, 255 };

    constexpr SDL_Color NUMBER = {0, 152,251,152};
    constexpr SDL_Color FLAG        = {   0,   0, 220, 255 };
    constexpr SDL_Color BOMB_ICON   = { 200,   0,   0, 255 };

    constexpr SDL_Color NUM[9] = {
        {   0,   0,   0, 255 },
        {   0,   0, 255, 255 },
        {   0, 130,   0, 255 },
        { 220,   0,   0, 255 },
        {   0,   0, 130, 255 },
        { 130,   0,   0, 255 },
        {   0, 130, 130, 255 },
        {   0,   0,   0, 255 },
        { 128, 128, 128, 255 },
    };

    constexpr SDL_Color WIN_MSG  = {   0, 160,   0, 255 };
    constexpr SDL_Color LOSE_MSG = { 220,   0,   0, 255 };

}
