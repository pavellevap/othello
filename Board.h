#pragma once

const size_t BOARD_X_DIM = 8;
const size_t BOARD_Y_DIM = 8;

enum Color { BLACK, WHITE, FREE };


class Position {
public:
    Position(): _x(0), _y(0) {}

	Position(size_t x, size_t y) : _x(x), _y(y) {
		if (!isCorrect())
			_x = _y = 0;
	}

	size_t x() const {
		return _x;
	}

	size_t y() const {
		return _y;
	}

	bool add(int dx, int dy) {
		_x += dx;
		_y += dy;
		if (!isCorrect()) {
			_x -= dx;
			_y -= dy;
			return false;
		}
		return true;
	}

	bool operator == (Position pos) const {
		return _x == pos._x && _y == pos._y;
	}

	bool operator != (Position pos) const {
		return _x != pos._x || _y != pos._y;
	}

private:
	size_t _x, _y;

	bool isCorrect() const {
		return _x < BOARD_X_DIM && _y < BOARD_Y_DIM;
	}
};


// Simple representation of the state of the board. This class does not contain any game-related logic.
class Board {
public:
	Board() {
		for (size_t i = 0; i < X_DIM; i++)
			for (size_t j = 0; j < Y_DIM; j++)
				board[i][j] = FREE;
	}

	Color& operator [] (Position pos) {
		return board[pos.x()][pos.y()];
	}

	Color operator [] (Position pos) const {
		return board[pos.x()][pos.y()];
	}

    bool operator == (const Board& other) const {
        for (size_t i = 0; i < Board::X_DIM; i++)
            for (size_t j = 0; j < BOARD_Y_DIM; j++)
                if (board[i][j] != other.board[i][j])
                    return false;
        return true;
    }

	static const size_t X_DIM;
	static const size_t Y_DIM;

private:
	Color board[BOARD_X_DIM][BOARD_Y_DIM];
};
const size_t Board::X_DIM = BOARD_X_DIM;
const size_t Board::Y_DIM = BOARD_Y_DIM;


struct BoardHasher {
    size_t operator () (const Board& board) const {
        size_t hash = 0;
        for (size_t i = 0; i < Board::X_DIM; i++)
            for (size_t j = 0; j < Board::Y_DIM; j++)
                hash = hash * 3 + board[Position(i, j)];
        return hash;
    }
};
