#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 600
#define CELL_SIZE     32
#define MENU_HEIGHT   100

enum GameState { MENU, GAME, SETTING, QUIT };

struct Level {
    std::string name;
    int rows, cols, bombs;
};

const Level LEVELS[3] = {
    {"Beginner",     9,  9, 10},
    {"Intermediate",16, 16, 40},
    {"Expert",      16, 30, 99}
};

struct Cell {
    bool hasBomb = false;
    bool revealed = false;
    bool flagged = false;
    int  neighborBombs = 0;
};

struct Board {
    int rows, cols, bombs;
    std::vector<std::vector<Cell>> cells;
    bool gameOver = false;
    bool win = false;

    void init(int r, int c, int b) {
        rows = r; cols = c; bombs = b; gameOver = false; win = false;
        cells.assign(rows, std::vector<Cell>(cols));
        placeBombs();
        calcNeighbors();
    }
    void placeBombs() {
        srand((unsigned)time(0));
        int placed = 0;
        while (placed < bombs) {
            int x = rand() % rows;
            int y = rand() % cols;
            if (!cells[x][y].hasBomb) {
                cells[x][y].hasBomb = true;
                placed++;
            }
        }
    }
    void calcNeighbors() {
        int dx[] = {-1,-1,-1,0,0,1,1,1};
        int dy[] = {-1,0,1,-1,1,-1,0,1};
        for (int i=0; i<rows; ++i) {
            for (int j=0; j<cols; ++j) {
                int cnt = 0;
                for (int d=0; d<8; ++d) {
                    int ni = i + dx[d], nj = j + dy[d];
                    if (ni>=0 && ni<rows && nj>=0 && nj<cols)
                        cnt += cells[ni][nj].hasBomb;
                }
                cells[i][j].neighborBombs = cnt;
            }
        }
    }
    void reveal(int x, int y) {
        if (cells[x][y].revealed || cells[x][y].flagged) return;
        cells[x][y].revealed = true;
        if (cells[x][y].hasBomb) { gameOver = true; return; }
        if (cells[x][y].neighborBombs == 0) {
            int dx[] = {-1,-1,-1,0,0,1,1,1};
            int dy[] = {-1,0,1,-1,1,-1,0,1};
            for (int d=0; d<8; ++d) {
                int ni = x + dx[d], nj = y + dy[d];
                if (ni>=0 && ni<rows && nj>=0 && nj<cols)
                    reveal(ni, nj);
            }
        }
    }
    bool isWin() {
        for (int i=0; i<rows; ++i)
            for (int j=0; j<cols; ++j)
                if (!cells[i][j].revealed && !cells[i][j].hasBomb)
                    return false;
        win = true; return true;
    }
};

struct Button {
    SDL_Rect rect;
    std::string text;
    SDL_Color color;
    bool hover = false;
};

bool pointInRect(int x, int y, SDL_Rect r) {
    return x >= r.x && x <= r.x+r.w && y >= r.y && y <= r.y+r.h;
}

// Helper for rendering text
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& msg, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, msg.c_str(), color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return tex;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO); TTF_Init();
    SDL_Window* win = SDL_CreateWindow("Bomb Sweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);

    GameState state = MENU;
    int selectedLevel = 0, boardX = 0, boardY = MENU_HEIGHT;
    Board board;

    // --- Menu Buttons ---
    std::vector<Button> menuButtons = {
        {{50, 20, 120, 50}, "Beginner", {0,0,0,255}},
        {{190, 20, 160, 50}, "Intermediate", {0,0,0,255}},
        {{370, 20, 80, 50}, "Expert", {0,0,0,255}},
        {{50, 80, 120, 40}, "Setting", {0,0,0,255}},
        {{370, 80, 80, 40}, "Quit", {0,0,0,255}}
    };

    // --- Game Buttons ---
    Button restartBtn = {{WINDOW_WIDTH-180, 10, 80, 40}, "Restart", {0,0,0,255}};
    Button quitBtn    = {{WINDOW_WIDTH-90, 10, 80, 40}, "Quit", {0,0,0,255}};

    SDL_Event e;
    bool running = true;
    while (running) {
        // ---- Event Loop ----
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (state == MENU && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x, my = e.button.y;
                for (int i=0; i<menuButtons.size(); ++i) {
                    if (pointInRect(mx, my, menuButtons[i].rect)) {
                        if (i == 0 || i == 1 || i == 2) {
                            selectedLevel = i;
                            const Level& lvl = LEVELS[selectedLevel];
                            boardX = (WINDOW_WIDTH - lvl.cols*CELL_SIZE)/2;
                            boardY = MENU_HEIGHT+10;
                            board.init(lvl.rows, lvl.cols, lvl.bombs);
                            state = GAME;
                        } else if (i == 3) {
                            state = SETTING;
                        } else if (i == 4) {
                            running = false;
                        }
                    }
                }
            }
            if (state == GAME) {
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x, my = e.button.y;
                    if (pointInRect(mx, my, restartBtn.rect)) {
                        const Level& lvl = LEVELS[selectedLevel];
                        board.init(lvl.rows, lvl.cols, lvl.bombs);
                    } else if (pointInRect(mx, my, quitBtn.rect)) {
                        state = MENU;
                    } else if (!board.gameOver && !board.win && mx >= boardX && mx < boardX + board.cols*CELL_SIZE
                               && my >= boardY && my < boardY + board.rows*CELL_SIZE) {
                        int x = (my - boardY)/CELL_SIZE, y = (mx - boardX)/CELL_SIZE;
                        if (e.button.button == SDL_BUTTON_LEFT) {
                            board.reveal(x, y);
                            if (!board.gameOver) board.isWin();
                        } else if (e.button.button == SDL_BUTTON_RIGHT) {
                            board.cells[x][y].flagged = !board.cells[x][y].flagged;
                        }
                    }
                }
            }
            if (state == SETTING && e.type == SDL_KEYDOWN) {
                state = MENU; // Press any key to return to menu
            }
        }

        // ---- Rendering ----
        SDL_SetRenderDrawColor(ren, 220,220,220,255);
        SDL_RenderClear(ren);

        if (state == MENU) {
            for (auto& btn : menuButtons) {
                SDL_SetRenderDrawColor(ren, btn.hover?180:120, 180, 220, 255);
                SDL_RenderFillRect(ren, &btn.rect);
                SDL_Texture* t = renderText(ren, font, btn.text, btn.color);
                int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
                SDL_Rect tr = {btn.rect.x+(btn.rect.w-tw)/2, btn.rect.y+(btn.rect.h-th)/2, tw, th};
                SDL_RenderCopy(ren, t, NULL, &tr);
                SDL_DestroyTexture(t);
            }
            SDL_Texture* t = renderText(ren, font, "Bomb Sweeper", {40,40,90,255});
            int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
            SDL_Rect tr = {WINDOW_WIDTH/2-tw/2, MENU_HEIGHT-60, tw, th};
            SDL_RenderCopy(ren, t, NULL, &tr);
            SDL_DestroyTexture(t);
        }
        else if (state == GAME) {
            // Draw control buttons
            SDL_SetRenderDrawColor(ren, 200, 200, 250, 255);
            SDL_RenderFillRect(ren, &restartBtn.rect);
            SDL_RenderFillRect(ren, &quitBtn.rect);
            SDL_Texture* t1 = renderText(ren, font, restartBtn.text, restartBtn.color);
            SDL_Texture* t2 = renderText(ren, font, quitBtn.text, quitBtn.color);
            int tw, th;
            SDL_QueryTexture(t1, NULL, NULL, &tw, &th);
            SDL_Rect tr1 = {restartBtn.rect.x+(restartBtn.rect.w-tw)/2, restartBtn.rect.y+(restartBtn.rect.h-th)/2, tw, th};
            SDL_RenderCopy(ren, t1, NULL, &tr1);
            SDL_DestroyTexture(t1);
            SDL_QueryTexture(t2, NULL, NULL, &tw, &th);
            SDL_Rect tr2 = {quitBtn.rect.x+(quitBtn.rect.w-tw)/2, quitBtn.rect.y+(quitBtn.rect.h-th)/2, tw, th};
            SDL_RenderCopy(ren, t2, NULL, &tr2);
            SDL_DestroyTexture(t2);

            // Draw board
            for (int i=0; i<board.rows; ++i) {
                for (int j=0; j<board.cols; ++j) {
                    SDL_Rect r = {boardX + j*CELL_SIZE, boardY + i*CELL_SIZE, CELL_SIZE, CELL_SIZE};
                    SDL_SetRenderDrawColor(ren, 180,180,180,255);
                    if (board.cells[i][j].revealed) {
                        SDL_SetRenderDrawColor(ren, 240,240,240,255);
                        if (board.cells[i][j].hasBomb) SDL_SetRenderDrawColor(ren, 255,80,80,255);
                    }
                    SDL_RenderFillRect(ren, &r);
                    SDL_SetRenderDrawColor(ren, 120,120,120,255);
                    SDL_RenderDrawRect(ren, &r);

                    if (board.cells[i][j].flagged) {
                        SDL_Texture* t = renderText(ren, font, "F", {0,0,255,255});
                        int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
                        SDL_Rect tr = {r.x+(CELL_SIZE-tw)/2, r.y+(CELL_SIZE-th)/2, tw, th};
                        SDL_RenderCopy(ren, t, NULL, &tr);
                        SDL_DestroyTexture(t);
                    }
                    else if (board.cells[i][j].revealed && !board.cells[i][j].hasBomb && board.cells[i][j].neighborBombs > 0) {
                        SDL_Texture* t = renderText(ren, font, std::to_string(board.cells[i][j].neighborBombs), {0,150,0,255});
                        int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
                        SDL_Rect tr = {r.x+(CELL_SIZE-tw)/2, r.y+(CELL_SIZE-th)/2, tw, th};
                        SDL_RenderCopy(ren, t, NULL, &tr);
                        SDL_DestroyTexture(t);
                    }
                    else if (board.cells[i][j].revealed && board.cells[i][j].hasBomb) {
                        SDL_Texture* t = renderText(ren, font, "*", {255,0,0,255});
                        int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
                        SDL_Rect tr = {r.x+(CELL_SIZE-tw)/2, r.y+(CELL_SIZE-th)/2, tw, th};
                        SDL_RenderCopy(ren, t, NULL, &tr);
                        SDL_DestroyTexture(t);
                    }
                }
            }

            // Game over/win message
            if (board.gameOver || board.win) {
                std::string msg = board.win ? "You Win!" : "Game Over!";
                SDL_Texture* t = renderText(ren, font, msg, {255,0,0,255});
                int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
                SDL_Rect tr = {WINDOW_WIDTH/2-tw/2, MENU_HEIGHT+board.rows*CELL_SIZE+10, tw, th};
                SDL_RenderCopy(ren, t, NULL, &tr);
                SDL_DestroyTexture(t);
            }
        }
        else if (state == SETTING) {
            SDL_Texture* t = renderText(ren, font, "No settings yet. Press any key to return.", {0,0,0,255});
            int tw, th; SDL_QueryTexture(t, NULL, NULL, &tw, &th);
            SDL_Rect tr = {WINDOW_WIDTH/2-tw/2, WINDOW_HEIGHT/2-th/2, tw, th};
            SDL_RenderCopy(ren, t, NULL, &tr);
            SDL_DestroyTexture(t);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit(); SDL_Quit();
    return 0;
}