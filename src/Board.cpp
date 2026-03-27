#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <stdexcept>

// ============================================================
// init() — reset toàn bộ bảng về trạng thái ban đầu
// ============================================================

void Board::init(int r, int c, int b) {
    if (r <= 0 || c <= 0 || b <= 0 || b >= r * c)
        throw std::invalid_argument("Board: invalid dimensions or bomb count.");

    rows       = r;
    cols       = c;
    bombs      = b;
    flagCount  = 0;
    gameOver   = false;
    win        = false;
    firstClick = true;

    // Reset toàn bộ ô về giá trị mặc định
    cells.assign(rows, std::vector<Cell>(cols));
}

// ============================================================
// reveal() — mở ô tại (row, col)
// ============================================================

void Board::reveal(int row, int col) {
    if (!isInBounds(row, col))   return;
    if (gameOver || win)          return;

    Cell& cell = cells[row][col];
    if (cell.revealed || cell.flagged) return;

    // --- Safe first click: đặt mìn sau khi biết ô đầu tiên ---
    if (firstClick) {
        firstClick = false;
        placeBombs(row, col);
        calcNeighbors();
    }

    floodReveal(row, col);

    if (!gameOver) checkWin();
}

// ============================================================
// toggleFlag() — cắm / bỏ cờ tại ô (row, col)
// ============================================================

void Board::toggleFlag(int row, int col) {
    if (!isInBounds(row, col)) return;
    if (gameOver || win)        return;

    Cell& cell = cells[row][col];
    if (cell.revealed) return; // Không flag ô đã lộ

    cell.flagged = !cell.flagged;
    flagCount += cell.flagged ? 1 : -1;
}

// ============================================================
// checkWin() — trả về true nếu tất cả ô an toàn đã lộ
// ============================================================

bool Board::checkWin() {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (!cells[i][j].hasBomb && !cells[i][j].revealed)
                return false;

    win = true;
    return true;
}

// ============================================================
// revealAllBombs() — lộ toàn bộ mìn (dùng khi game over)
// ============================================================

void Board::revealAllBombs() {
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (cells[i][j].hasBomb)
                cells[i][j].revealed = true;
}

// ============================================================
// remainingMines()
// ============================================================

int Board::remainingMines() const {
    return bombs - flagCount;
}

// ============================================================
// isInBounds()
// ============================================================

bool Board::isInBounds(int row, int col) const {
    return row >= 0 && row < rows
        && col >= 0 && col < cols;
}

// ============================================================
// placeBombs() — đặt mìn ngẫu nhiên, tránh vùng safe 3x3
//               xung quanh ô đầu tiên người chơi click
// ============================================================

void Board::placeBombs(int safeRow, int safeCol) {
    srand(static_cast<unsigned>(time(nullptr)));

    int placed = 0;
    while (placed < bombs) {
        int r = rand() % rows;
        int c = rand() % cols;

        // Bỏ qua nếu đã có mìn
        if (cells[r][c].hasBomb) continue;

        // Bỏ qua vùng 3x3 xung quanh ô safe
        if (std::abs(r - safeRow) <= 1 && std::abs(c - safeCol) <= 1) continue;

        cells[r][c].hasBomb = true;
        ++placed;
    }
}

// ============================================================
// calcNeighbors() — tính số mìn lân cận cho mỗi ô
// ============================================================

void Board::calcNeighbors() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (cells[i][j].hasBomb) continue; // Ô mìn không cần tính

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

// ============================================================
// floodReveal() — mở ô và lan ra nếu neighborBombs == 0
// ============================================================

void Board::floodReveal(int row, int col) {
    if (!isInBounds(row, col))       return;

    Cell& cell = cells[row][col];
    if (cell.revealed || cell.flagged) return;

    cell.revealed = true;

    // Trúng mìn → game over, lộ toàn bộ mìn
    if (cell.hasBomb) {
        gameOver = true;
        revealAllBombs();
        return;
    }

    // Ô trống → flood-fill ra 8 hướng
    if (cell.neighborBombs == 0) {
        for (const auto& d : DIR)
            floodReveal(row + d[0], col + d[1]);
    }
}
