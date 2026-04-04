#include "Renderer.h"
#include <stdexcept>

Renderer::~Renderer() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
}

bool Renderer::init(const char* title, int w, int h, const char* fontPath) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (TTF_Init() != 0) return false;

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h, 0
    );
    if (!window) return false;

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) return false;

    font = TTF_OpenFont(fontPath, FONT_SIZE_NORMAL);
    if (!font) return false;

    return true;
}

void Renderer::clear() {
    fillRect({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, Color::BG);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::drawMenu(const std::vector<Button>& buttons) {
    SDL_Rect titleArea = { 0, 5, WINDOW_WIDTH, HEADER_HEIGHT / 2 };
    drawTextCentered("Bomb Sweeper", Color::TITLE, titleArea);

    drawRect({ 10, HEADER_HEIGHT - 4, WINDOW_WIDTH - 20, 1 }, Color::BORDER);

    for (const auto& btn : buttons)
        drawButton(btn);
}

void Renderer::drawGame(const Board& board,
    int boardX,
    int boardY,
    const Button& restartBtn,
    const Button& menuBtn) {
    drawGameHeader(board, restartBtn, menuBtn);
    drawBoard(board, boardX, boardY);
    drawEndMessage(board, boardY);
}

void Renderer::drawSetting() {
    SDL_Rect area = { 0, WINDOW_HEIGHT / 2 - 20, WINDOW_WIDTH, 40 };
    drawTextCentered("No settings yet. Press any key to return.", Color::BLACK, area);
}

void Renderer::drawButton(const Button& btn) {
    btn.draw(renderer, font);
}

void Renderer::drawBoard(const Board& board, int boardX, int boardY) {
    for (int i = 0; i < board.rows; ++i) {
        for (int j = 0; j < board.cols; ++j) {
            SDL_Rect cellRect = {
                boardX + j * CELL_SIZE,
                boardY + i * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE
            };
            drawCell(board.cells[i][j], cellRect, board.gameOver);
        }
    }

    SDL_Rect boardBorder = {
        boardX - 1,
        boardY - 1,
        board.cols * CELL_SIZE + 2,
        board.rows * CELL_SIZE + 2
    };
    drawRect(boardBorder, Color::BTN_BORDER);
}

void Renderer::drawGameHeader(const Board&  board,
    const Button& restartBtn,
    const Button& menuBtn) {

    drawRect({ 0, HEADER_HEIGHT, WINDOW_WIDTH, 1 }, Color::BORDER);

    std::string mineText = "Mines: " + std::to_string(board.remainingMines());
    SDL_Rect mineArea = { 10, 0, 160, HEADER_HEIGHT };
    drawTextCentered(mineText, Color::TITLE, mineArea);

    drawButton(restartBtn);
    drawButton(menuBtn);
}

void Renderer::drawEndMessage(const Board& board, int boardY) {
    if (!board.gameOver && !board.win) return;

    std::string msg = board.win ? "You Win!" : "Game Over!";
    SDL_Color msgColor = board.win ? Color::WIN_MSG : Color::LOSE_MSG;

    int msgY = boardY + board.rows * CELL_SIZE + 10;
    SDL_Rect msgArea = { 0, msgY, WINDOW_WIDTH, 40 };
    drawTextCentered(msg, msgColor, msgArea);
}

void Renderer::drawTextCentered(const std::string& text,
    SDL_Color color,
    SDL_Rect area) {
    SDL_Texture* tex = makeTexture(text, color);
    if (!tex) return;

    int tw, th;
    SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);

    SDL_Rect dst = {
        area.x + (area.w - tw) / 2,
        area.y + (area.h - th) / 2,
        tw, th
    };
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void Renderer::drawText(const std::string& text, SDL_Color color, int x, int y) {
    SDL_Texture* tex = makeTexture(text, color);
    if (!tex) return;

    int tw, th;
    SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);

    SDL_Rect dst = { x, y, tw, th };
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void Renderer::fillRect(SDL_Rect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void Renderer::drawRect(SDL_Rect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

void Renderer::drawCell(const Cell& cell, SDL_Rect rect, bool gameOver) {
    CellVisual v = cell.visual(gameOver);

    fillRect(rect, cellBgColor(v));

    drawRect(rect, Color::BORDER);

    switch (v) {
        case CellVisual::HIDDEN:
            drawRect({ rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2 },
            { 200, 200, 200, 100 });
            break;

        case CellVisual::FLAGGED:
            fillRect(rect, Color::CELL_FLAG);
            drawRect(rect, Color::BORDER);
            drawTextCentered("F", Color::FLAG, rect);
            break;

        case CellVisual::WRONG_FLAG:
            fillRect(rect, Color::CELL_BOMB);
            drawRect(rect, Color::BORDER);
            drawTextCentered("X", Color::BLACK, rect);
            break;

        case CellVisual::REVEALED_NUMBER:
            drawTextCentered(
                std::to_string(cell.neighborBombs),
                Color::NUM[cell.neighborBombs],
                rect
            );
            break;

        case CellVisual::REVEALED_BOMB:
            drawTextCentered("*", Color::BOMB_ICON, rect);
            break;

        case CellVisual::REVEALED_EMPTY:
            break;
    }
}

SDL_Color Renderer::cellBgColor(CellVisual v) {
    switch (v) {
        case CellVisual::HIDDEN:
            return Color::CELL_HIDDEN;
        case CellVisual::FLAGGED:
            return Color::CELL_FLAG;
        case CellVisual::REVEALED_EMPTY:
            return Color::CELL_SHOWN;
        case CellVisual::REVEALED_NUMBER:
            return Color::CELL_SHOWN;
        case CellVisual::REVEALED_BOMB:
            return Color::CELL_BOMB;
        case CellVisual::WRONG_FLAG:
            return Color::CELL_BOMB;
        default:
            return Color::CELL_HIDDEN;
    }
}

SDL_Texture* Renderer::makeTexture(const std::string& text, SDL_Color color) const {
    if (!font || text.empty()) return nullptr;

    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surf) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}
