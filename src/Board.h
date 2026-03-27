#pragma once
#include "Cell.h"
#include <vector>

class Board {
public:
    // -------------------------------------------------------
    // State (read-only từ bên ngoài, Board tự cập nhật)
    // -------------------------------------------------------
    int  rows     = 0;
    int  cols     = 0;
    int  bombs    = 0;
    int  flagCount = 0;   // Số cờ đang cắm, tự cập nhật khi toggleFlag()
    bool gameOver = false;
    bool win      = false;
    bool firstClick = true; // Đảm bảo click đầu không trúng mìn

    std::vector<std::vector<Cell>> cells;

    // -------------------------------------------------------
    // Public API
    // -------------------------------------------------------

    // Khởi tạo lại bảng với kích thước và số mìn mới
    void init(int r, int c, int b);

    // Mở ô tại (row, col)
    // - Click đầu tiên: đặt mìn sau đó mới reveal (safe first click)
    // - Flood-fill nếu ô trống
    // - Set gameOver = true nếu trúng mìn
    void reveal(int row, int col);

    // Lật cờ tại ô (row, col), cập nhật flagCount
    // Không cho flag ô đã reveal
    void toggleFlag(int row, int col);

    // Kiểm tra thắng: tất cả ô không mìn đã được reveal
    // Set win = true nếu đúng, trả về kết quả
    bool checkWin();

    // Lộ toàn bộ mìn (gọi khi game over để hiển thị)
    void revealAllBombs();

    // Số mìn còn lại = bombs - flagCount
    int remainingMines() const;

    // Kiểm tra tọa độ hợp lệ
    bool isInBounds(int row, int col) const;

private:
    // Đặt mìn ngẫu nhiên, tránh ô (safeRow, safeCol) và các ô xung quanh
    void placeBombs(int safeRow, int safeCol);

    // Tính neighborBombs cho toàn bộ bảng
    void calcNeighbors();

    // Flood-fill reveal đệ quy (chỉ dùng nội bộ)
    void floodReveal(int row, int col);

    // 8 hướng di chuyển
    static constexpr int DIR[8][2] = {
        {-1,-1}, {-1, 0}, {-1, 1},
        { 0,-1},          { 0, 1},
        { 1,-1}, { 1, 0}, { 1, 1}
    };
};
