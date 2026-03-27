#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "Constants.h"
#include "Board.h"
#include "Button.h"

class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    bool init(const char* title, int w, int h, const char* fontPath);
    void clear();
    void present();

    void drawMenu(const std::vector<Button>& buttons);

    void drawGame(const Board& board,
    int boardX,
    int boardY,
    const Button& restartBtn,
    const Button& menuBtn);

    void drawSetting();
    void drawButton(const Button& btn);
    void drawBoard(const Board& board, int boardX, int boardY);

    void drawGameHeader(const Board&  board,
        const Button& restartBtn,
        const Button& menuBtn);

    void drawEndMessage(const Board& board, int boardY);

    void drawTextCentered(const std::string& text,
        SDL_Color color,
        SDL_Rect area);

    void drawText(const std::string& text,
        SDL_Color color,
        int x, int y);

    void fillRect(SDL_Rect rect, SDL_Color color);

    void drawRect(SDL_Rect rect, SDL_Color color);

    SDL_Renderer* getSDLRenderer() const { return renderer; }
    TTF_Font* getFont() const { return font;}

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    void drawCell(const Cell& cell, SDL_Rect rect, bool gameOver);

    static SDL_Color cellBgColor(CellVisual v);

    SDL_Texture* makeTexture(const std::string& text, SDL_Color color) const;
};
