#include "Game.h"
#include <iostream>
#include <string>
#include <SDL_image.h>

Game::~Game() {
    if (font) TTF_CloseFont(font);

    if (logoTexture) {
        SDL_DestroyTexture(logoTexture);
        logoTexture = nullptr;
    }
    IMG_Quit();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "[SDL] Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    if (TTF_Init() != 0) {
        std::cerr << "[TTF] Init failed: " << TTF_GetError() << "\n";
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "[IMG] Init failed: " << IMG_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow(
        "Bomb Sweeper",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0
    );
    if (!window) {
        std::cerr << "[SDL] CreateWindow failed: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cerr << "[SDL] CreateRenderer failed: " << SDL_GetError() << "\n";
        return false;
    }

    font = TTF_OpenFont("D:/Minesweeper/assets/font/arial.ttf", FONT_SIZE_NORMAL);
    if (!font) {
        std::cerr << "[TTF] OpenFont failed: " << TTF_GetError() << "\n";
        return false;
    }

    SDL_Surface* logoSurf = IMG_Load("D:/Minesweeper/assets/image/logo.png");
    if (logoSurf) {
        logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurf);
        SDL_FreeSurface(logoSurf);
    } else {
        std::cerr << "[IMG] Failed to load logo.png: " << IMG_GetError() << "\n";
    }

    buildMenuButtons();
    buildGameButtons();
    return true;
}

void Game::run() {
    running = true;
    while (running) {
        //std::cout << flagMode << std::endl;
        handleEvents();
        render();
        SDL_Delay(FRAME_DELAY_MS);
    }
}

void Game::buildMenuButtons() {
    menuButtons = {
        { {50, 420, 120, 50}, "Beginner", Color::BTN_NORMAL, Color::BLACK },
        { {190, 420, 160, 50}, "Intermediate", Color::BTN_NORMAL, Color::BLACK },
        { {370, 420, 80, 50}, "Expert", Color::BTN_NORMAL, Color::BLACK },
        { {50, 480, 120, 40}, "Setting", Color::BTN_NORMAL, Color::BLACK },
        { {370, 480, 80, 40}, "Quit", Color::BTN_NORMAL, Color::BLACK },
    };
}

void Game::buildGameButtons() {
    restartBtn = { {WINDOW_WIDTH - 180, 10, 80, 40}, "Restart", Color::BTN_GAME, Color::BLACK };
    menuBtn = { {WINDOW_WIDTH - 90,  10, 80, 40}, "Menu", Color::BTN_GAME, Color::BLACK };
    flagBtn = { {WINDOW_WIDTH - 270, 10, 80, 40}, "Flag", Color::BTN_NORMAL, Color::BLACK };
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return;
        }

        if (e.type == SDL_MOUSEMOTION) {
            int mx = e.motion.x, my = e.motion.y;
            if (state == GameState::MENU) updateHover(menuButtons, mx, my);
        }

        switch (state) {
            case GameState::MENU:
                onMenuEvent(e);
                break;
            case GameState::GAME:
                onGameEvent(e);
                break;
            case GameState::SETTING:
                onSettingEvent(e);
                break;
            default: break;
        }
    }
}

void Game::onMenuEvent(const SDL_Event& e) {
    if (e.type != SDL_MOUSEBUTTONDOWN) return;
    if (e.button.button != SDL_BUTTON_LEFT) return;

    int mx = e.button.x, my = e.button.y;

    for (int i = 0; i < (int)menuButtons.size(); ++i) {
        if (!pointInRect(mx, my, menuButtons[i].rect)) continue;

        if (i < NUM_LEVELS) {
            startLevel(i);
        } else if (menuButtons[i].label == "Setting") {
            state = GameState::SETTING;
        } else if (menuButtons[i].label == "Quit") {
            running = false;
        }
        return;
    }
}

void Game::onGameEvent(const SDL_Event& e) {
    if (e.type != SDL_MOUSEBUTTONDOWN) return;

    int mx = e.button.x, my = e.button.y;

    if (pointInRect(mx, my, restartBtn.rect)) {
        startLevel(selectedLevel);
        return;
    }
    if (pointInRect(mx, my, menuBtn.rect)) {
        state = GameState::MENU;
        return;
    }
    if (pointInRect(mx, my, flagBtn.rect)) {
        flagMode = !flagMode;
        return;
    }

    if (board.gameOver || board.win) return;

    bool inBoard = mx >= boardX && mx < boardX + board.cols * CELL_SIZE
                && my >= boardY && my < boardY + board.rows * CELL_SIZE;
    if (!inBoard) return;

    int row = (my - boardY) / CELL_SIZE;
    int col = (mx - boardX) / CELL_SIZE;

    if (flagMode) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            board.toggleFlag(row, col);
        }
    }
    else {
        if (e.button.button == SDL_BUTTON_LEFT) {
            board.reveal(row, col);
        }
    }
}

void Game::onSettingEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN)
        state = GameState::MENU;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer,
        Color::BG.r, Color::BG.g, Color::BG.b, Color::BG.a);
    SDL_RenderClear(renderer);

    switch (state) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::GAME:
            renderGame();
            break;
        case GameState::SETTING:
            renderSetting();
            break;
        default: break;
    }

    SDL_RenderPresent(renderer);
}

void Game::renderMenu() {

    if (logoTexture) {
        int logoW, logoH;
        SDL_QueryTexture(logoTexture, nullptr, nullptr, &logoW, &logoH);

        SDL_Rect logoRect = {
            (WINDOW_WIDTH - logoW) / 2,
            20,
            logoW,
            logoH
        };
        SDL_RenderCopy(renderer, logoTexture, nullptr, &logoRect);
    }
    else {
        SDL_Rect titleArea = { 0, 40, WINDOW_WIDTH, 80 };
        drawTextCentered("Bomb Sweeper", Color::TITLE, titleArea);
    }

    for (const auto& btn : menuButtons)
        drawButton(btn);
}

void Game::renderGame() {
    
    std::string mineInfo = "Mines: " + std::to_string(board.remainingMines());
    SDL_Rect infoArea = { 10, 0, 180, HEADER_HEIGHT };
    drawTextCentered(mineInfo, Color::TITLE, infoArea);

    drawButton(restartBtn);
    drawButton(menuBtn);

    flagBtn.bgColor = flagMode ? Color::BTN_HOVER : Color::BTN_GAME;
    drawButton(flagBtn);

    SDL_SetRenderDrawColor(renderer,
        Color::BORDER.r, Color::BORDER.g, Color::BORDER.b, Color::BORDER.a);
    SDL_RenderDrawLine(renderer, 0, HEADER_HEIGHT, WINDOW_WIDTH, HEADER_HEIGHT);

    for (int i = 0; i < board.rows; ++i) {
        for (int j = 0; j < board.cols; ++j) {
            const Cell& cell = board.cells[i][j];
            SDL_Rect r = {
                boardX + j * CELL_SIZE,
                boardY + i * CELL_SIZE,
                CELL_SIZE, CELL_SIZE
            };

            CellVisual v = cell.visual(board.gameOver);

            SDL_Color bg;
            switch (v) {
                case CellVisual::REVEALED_EMPTY:
                case CellVisual::REVEALED_NUMBER:
                    bg = Color::CELL_SHOWN;
                    break;
                case CellVisual::REVEALED_BOMB:
                case CellVisual::WRONG_FLAG:
                    bg = Color::CELL_BOMB;
                    break;
                case CellVisual::FLAGGED:
                    bg = Color::CELL_FLAG;
                    break;
                default:
                    bg = Color::CELL_HIDDEN;
                    break;
            }
            SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
            SDL_RenderFillRect(renderer, &r);

            SDL_SetRenderDrawColor(renderer,
                Color::BORDER.r, Color::BORDER.g, Color::BORDER.b, Color::BORDER.a);
            SDL_RenderDrawRect(renderer, &r);

            switch (v) {
                case CellVisual::FLAGGED:
                    drawTextCentered("F", Color::FLAG, r);
                    break;
                case CellVisual::WRONG_FLAG:
                    drawTextCentered("X", Color::LOSE_MSG, r);
                    break;
                case CellVisual::REVEALED_NUMBER:
                    drawTextCentered(
                        std::to_string(cell.neighborBombs),
                        Color::NUM[cell.neighborBombs],
                        r
                    );
                    break;
                case CellVisual::REVEALED_BOMB:
                    drawTextCentered("*", Color::BOMB_ICON, r);
                    break;
                default:
                    break;
            }
        }
    }

    if (board.gameOver || board.win) {
        std::string msg = board.win ? "You Win!" : "Game Over!";
        SDL_Color msgColor = board.win ? Color::WIN_MSG : Color::LOSE_MSG;
        int msgY = boardY + board.rows * CELL_SIZE + 10;
        SDL_Rect msgArea = { 0, msgY, WINDOW_WIDTH, 40 };
        drawTextCentered(msg, msgColor, msgArea);
    }
}

void Game::renderSetting() {
    SDL_Rect area = { 0, WINDOW_HEIGHT / 2 - 20, WINDOW_WIDTH, 40 };
    drawTextCentered("No settings yet. Press any key to return.", Color::BLACK, area);
}

void Game::startLevel(int levelIndex) {
    selectedLevel = levelIndex;
    const LevelConfig& lvl = LEVELS[selectedLevel];
    boardX = (WINDOW_WIDTH - lvl.cols * CELL_SIZE) / 2;
    boardY = BOARD_OFFSET_Y;
    board.init(lvl.rows, lvl.cols, lvl.bombs);
    state = GameState::GAME;
}

void Game::drawButton(const Button& btn) {
    SDL_Color bg = btn.hover ? Color::BTN_HOVER : btn.bgColor;
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &btn.rect);

    SDL_SetRenderDrawColor(renderer,
        Color::BTN_BORDER.r, Color::BTN_BORDER.g, Color::BTN_BORDER.b, Color::BTN_BORDER.a);
    SDL_RenderDrawRect(renderer, &btn.rect);

    drawTextCentered(btn.label, btn.txtColor, btn.rect);
}

void Game::drawTextCentered(const std::string& text, SDL_Color color, SDL_Rect area) {
    SDL_Texture* tex = makeTextTexture(text, color);
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

SDL_Texture* Game::makeTextTexture(const std::string& text, SDL_Color color) const {
    if (!font || text.empty()) return nullptr;
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surf) return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

bool Game::pointInRect(int px, int py, SDL_Rect r) {
    return px >= r.x && px <= r.x + r.w
        && py >= r.y && py <= r.y + r.h;
}

void Game::updateHover(std::vector<Button>& buttons, int mx, int my) {
    for (auto& btn : buttons)
        btn.hover = pointInRect(mx, my, btn.rect);
}
