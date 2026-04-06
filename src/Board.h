#pragma once
#include "Cell.h"
#include <vector>

class Board {
public:
    int rows = 0;
    int cols = 0;
    int bombs = 0;
    int flagCount = 0;
    bool gameOver = false;
    bool win = false;
    bool firstClick = true;

    std::vector<std::vector<Cell>> cells;

    void init(int r, int c, int b);
    void reveal(int row, int col);
    void toggleFlag(int row, int col);

    bool checkWin();
    void revealAllBombs();
    int remainingMines() const;
    bool isInBounds(int row, int col) const;

private:
    void placeBombs(int safeRow, int safeCol);
    void calcNeighbors();
    void floodReveal(int row, int col);

    static constexpr int DIR[8][2] = {
        {-1,-1}, {-1, 0}, {-1, 1},
        { 0,-1},          { 0, 1},
        { 1,-1}, { 1, 0}, { 1, 1}
    };
};
