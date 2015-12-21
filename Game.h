#pragma once

#include <vector>

#include "Board.h"

class Move {
public:
	Move(Position _pos = Position(0, 0), bool _isPass = false) : pos(_pos), isPass(_isPass) {}

	bool operator == (const Move& other) const {
        if (isPass != other.isPass)
            return false;
        if (isPass && other.isPass)
            return true;
        return pos == other.pos;
	}

	Position pos;
	bool isPass;
};

class Change {
public:
	Position position;
	Color oldValue;
	Color newValue;
};

class Game {
public:
	Game() {
        if (Board::X_DIM == 8 && Board::Y_DIM == 8) {
			board[Position(3, 3)] = WHITE;
			board[Position(3, 4)] = BLACK;
			board[Position(4, 3)] = BLACK;
			board[Position(4, 4)] = WHITE;

			score[WHITE] = score[BLACK] = 2;
			score[FREE] = 60;
		}
    }

	void makeMove(Move move) {
		if (isMovePossible(move, getCurrentColor())) {
			std::vector<Change> changesDuringMove;

			if (move.isPass) {
				moves.push_back(move);
				changes.push_back(changesDuringMove);
				return;
			}
			changesDuringMove.push_back(makeChange(move.pos, getCurrentColor()));

			for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue;
					Position pos = move.pos;
					while (pos.add(dx, dy) && board[pos] == getOppositeColor(getCurrentColor()));
					if (pos != move.pos && board[pos] == getCurrentColor())
						while (pos.add(-dx, -dy) && pos != move.pos)
							changesDuringMove.push_back(makeChange(pos, getCurrentColor()));
				}

			moves.push_back(move);
			changes.push_back(changesDuringMove);
		}
	}

	void cancelMove() {
		if (getMoveNumber() > 0) {
			for (size_t i = 0; i < changes.back().size(); i++)
				cancelChange(changes.back()[i]);
			moves.pop_back();
			changes.pop_back();
		}
	}

	const std::vector<Move>& getMoves() const {
		return moves;
	}

	const size_t getMoveNumber() const {
		return moves.size();
	}

	const Board& getBoard() const {
		return board;
	}

	Color getCurrentColor() const {
		if (moves.size() & 1)
			return WHITE;
		else
			return BLACK;
	}

	Color getOppositeColor(Color color) const {
		if (color == WHITE) return BLACK;
		if (color == BLACK) return WHITE;
		return color;
	}

	bool isMovePossible(Move move, Color playerColor) const {
		if (playerColor != WHITE && playerColor != BLACK)
			return false;
		if (move.isPass)
			return true; // это не правильно.
		if (board[move.pos] != FREE)
			return false;

		for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0) continue;
				Position pos = move.pos;
				if (pos.add(dx, dy) && board[pos] == getOppositeColor(playerColor)) {
					while (pos.add(dx, dy) && board[pos] == getOppositeColor(playerColor));

					if (pos != move.pos && board[pos] == playerColor)
						return true;
				}
			}
		return false;
	}

	std::vector<Move> getPossibleMoves(Color playerColor) const {
		std::vector<Move> moves;
		for (size_t i = 0; i < Board::X_DIM; i++)
			for (size_t j = 0; j < Board::Y_DIM; j++)
				if (isMovePossible(Move(Position(i, j), false), playerColor))
					moves.push_back(Position(i, j));
		if (moves.size() == 0)
			moves.push_back(Move(Position(), true));
		return moves;
	}

	bool isGameFinished() const {
		return moves.size() >= 2 && moves[moves.size() - 1].isPass && moves[moves.size() - 2].isPass;
	}

	size_t getScore(Color color) const {
		return score[color];
	}

    /// Возвращает разницу количества фишек цвета color и фишек противоположного цвета.
    /// При color == FREE поведение не определено
    int getScoreDifference(Color color) const {
        return score[color] - score[getOppositeColor(color)];
    }

	size_t getAmountOfFreePositions() const {
		return getScore(FREE);
	}

private:
	std::vector<Move> moves;
	std::vector< std::vector<Change> > changes;
	Board board;

	int score[3];

	Change makeChange(Position pos, Color newValue) {
		Change change;
		change.position = pos;
		change.oldValue = board[pos];
		change.newValue = newValue;
		board[pos] = newValue;

		score[change.newValue]++;
		score[change.oldValue]--;

		return change;
	}

	void cancelChange(Change change) {
		board[change.position] = change.oldValue;

		score[change.newValue]--;
		score[change.oldValue]++;
	}
};
