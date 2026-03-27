#pragma once

enum class CellVisual {
    HIDDEN,
    FLAGGED,
    REVEALED_EMPTY,
    REVEALED_NUMBER,
    REVEALED_BOMB,
    WRONG_FLAG,
};

class Cell {
public:
    Cell() = default;

    void reset();

    CellVisual visual(bool gameOver = false) const;

    bool isHidden() const {
        return !revealed && !flagged;
    }
    bool isFlagged() const {
        return !revealed &&  flagged;
    }
    bool isEmpty() const {
        return  revealed && !hasBomb && neighborBombs == 0;
    }
    bool hasNumber() const {
        return  revealed && !hasBomb && neighborBombs > 0;
    }
    bool isBombCell() const {
        return  revealed &&  hasBomb;
    }

    bool hasBomb = false;
    bool revealed = false;
    bool flagged = false;
    int  neighborBombs = 0;

    friend class Board;
};

inline void Cell::reset() {
    hasBomb       = false;
    revealed      = false;
    flagged       = false;
    neighborBombs = 0;
}

inline CellVisual Cell::visual(bool gameOver) const {
    if (gameOver && flagged && !hasBomb)
        return CellVisual::WRONG_FLAG;

    if (!revealed) {
        return flagged ? CellVisual::FLAGGED
        : CellVisual::HIDDEN;
    }

    if (hasBomb) return CellVisual::REVEALED_BOMB;
    if (neighborBombs > 0) return CellVisual::REVEALED_NUMBER;
    return CellVisual::REVEALED_EMPTY;
}
