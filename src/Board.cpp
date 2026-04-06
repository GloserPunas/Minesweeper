#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <stdexcept>

void Board::init(int r, int c, int b) {
    if (r <= 0 || c <= 0 || b <= 0 || b >= r * c)
        throw std::invalid_argument("Board: invalid dimensions or bomb count.");

    rows = r;
    cols = c;
    bombs = b;
    flagCount = 0;
    gameOver = false;
    win = false;
    firstClick = true;

    cells.assign(rows, std::vector<Cell>(cols));
}


void Board::reveal(int row, int col) {
    if (!isInBounds(row, col))   return;
    if (gameOver || win)          return;

    Cell& cell = cells[row][col];
    if (cell.revealed || cell.flagged) return;

    if (firstClick) {
        firstClick = false;
        placeBombs(row, col);
        calcNeighbors();
    }

    floodReveal(row, col);

    if (!gameOver) checkWin();
}

void Board::toggleFlag(int row, int col) {
    if (!isInBounds(row, col)) return;
    if (gameOver || win) return;

    Cell& cell = cells[row][col];
    if (cell.revealed) return;

    cell.flagged = !cell.flagged;
    flagCount += cell.flagged ? 1 : -1;
}

bool Board::checkWin() {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (!cells[i][j].hasBomb && !cells[i][j].revealed)
                return false;

    win = true;
    return true;
}

void Board::revealAllBombs() {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (cells[i][j].hasBomb)
                cells[i][j].revealed = true;
}

int Board::remainingMines() const {
    return bombs - flagCount;
}

bool Board::isInBounds(int row, int col) const {
    return row >= 0 && row < rows
        && col >= 0 && col < cols;
}

void Board::placeBombs(int safeRow, int safeCol) {
    srand(static_cast<unsigned>(time(nullptr)));

    int placed = 0;
    while (placed < bombs) {
        int r = rand() % rows;
        int c = rand() % cols;

        if (cells[r][c].hasBomb) continue;

        if (std::abs(r - safeRow) <= 1 && std::abs(c - safeCol) <= 1) continue;

        cells[r][c].hasBomb = true;
        ++placed;
    }
}

void Board::calcNeighbors() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (cells[i][j].hasBomb) continue;

            int count = 0;
            for (const auto& d : DIR) {
                int ni = i + d[0];
                int nj = j + d[1];
                if (isInBounds(ni, nj) && cells[ni][nj].hasBomb)
                    ++count;
            }
            cells[i][j].neighborBombs = count;
        }
    }
}

void Board::floodReveal(int row, int col) {
    if (!isInBounds(row, col))       return;

    Cell& cell = cells[row][col];
    if (cell.revealed || cell.flagged) return;

    cell.revealed = true;

    if (cell.hasBomb) {
        gameOver = true;
        revealAllBombs();
        return;
    }

    if (cell.neighborBombs == 0) {
        for (const auto& d : DIR)
            floodReveal(row + d[0], col + d[1]);
    }
}
