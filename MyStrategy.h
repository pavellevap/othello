#pragma once

#include <queue>
#include <unordered_map>
#include <memory>
#include <ctime>
#include "Strategy.h"


struct MyConstants {
	MyConstants(int cornerCost, int XFieldCost, int CFieldCost, double timeForMove) :
		CORNER_COST(cornerCost), X_FIELD_COST(XFieldCost), C_FIELD_COST(CFieldCost), TIME_FOR_MOVE(timeForMove) {}
	int CORNER_COST;
	int X_FIELD_COST; // X-field - position adjacent to a free corner diagonally
	int C_FIELD_COST; // C-field - position adjacent to a free corner vertically or horizontally
	double TIME_FOR_MOVE; // thinking time for one move in seconds
};


class Estimator {
public:
    virtual int estimate(const Game& game, Color player) = 0;
};


// Estimates current score difference
class ScoreEstimator : public Estimator {
public:
    int estimate(const Game& game, Color player) override {
        return game.getScoreDifference(player);
    }
};


// Estimates player's mobility.
// Mobility is a difference between number of moves available to player and number of moves available to opponent.
// Corners are counted twice. C and X fields are not counted at all.
// The higher mobility is, the better.
class MobilityEstimator : public Estimator {
public:
    int estimate(const Game& game, Color player) override {
        int score = 0;

		const Board& board = game.getBoard();
		for (size_t x = 0; x < Board::X_DIM; x++)
			for (size_t y = 0; y < Board::Y_DIM; y++) {
                if (board[Position(x, y)] != FREE)
                    continue;

                int pos_value = 1;
                if (Board::X_DIM == 8 && Board::Y_DIM == 8) {
                    // count corners twice
                    if ((x == 0 && y == 0) || (x == 0 && y == 7) ||
                        (x == 7 && y == 0) || (x == 7 && y == 7))
                        pos_value = 2;
                    // do not count C and X fields if corner is free
                    if ((x == 1 && y == 1 && board[Position(0, 0)] == FREE) ||
                        (x == 1 && y == 6 && board[Position(0, 7)] == FREE) ||
                        (x == 6 && y == 1 && board[Position(7, 0)] == FREE) ||
                        (x == 6 && y == 6 && board[Position(7, 7)] == FREE) ||
                        (x == 1 && y == 0 && board[Position(0, 0)] == FREE) ||
                        (x == 0 && y == 1 && board[Position(0, 0)] == FREE) ||
                        (x == 6 && y == 0 && board[Position(7, 0)] == FREE) ||
                        (x == 7 && y == 1 && board[Position(7, 0)] == FREE) ||
                        (x == 1 && y == 7 && board[Position(0, 7)] == FREE) ||
                        (x == 0 && y == 6 && board[Position(0, 7)] == FREE) ||
                        (x == 6 && y == 7 && board[Position(7, 7)] == FREE) ||
                        (x == 7 && y == 6 && board[Position(7, 7)] == FREE))
                        pos_value = 0;
                }

                if (game.isMovePossible(Move(Position(x, y), false), player))
                    score += pos_value;
                if (game.isMovePossible(Move(Position(x, y), false), Game::getOppositeColor(player)))
                    score -= pos_value;
            }
        return score;
    }
};


// Assigns values to corners, C and X fields and
// calculates difference between total value of player's stones and opponent's stones
class PositionEstimator : public Estimator {
public:
    PositionEstimator(int _cornerCost, int _XFieldCost, int _CFieldCost) : cornerCost(_cornerCost),
            XFieldCost(_XFieldCost), CFieldCost(_CFieldCost) {}

    static int countCorners(const Board& board, Color player) {
        return ((board[Position(0, 0)] == player) +
                (board[Position(0, 7)] == player) +
                (board[Position(7, 0)] == player) +
                (board[Position(7, 7)] == player));
    }

    static int countXFields(const Board& board, Color player) {
        return ((board[Position(0, 0)] == Color::FREE && board[Position(1, 1)] == player) +
                (board[Position(0, 7)] == Color::FREE && board[Position(1, 6)] == player) +
                (board[Position(7, 0)] == Color::FREE && board[Position(6, 1)] == player) +
                (board[Position(7, 7)] == Color::FREE && board[Position(6, 6)] == player));
    }

    static int countCFields(const Board& board, Color player) {
        return ((board[Position(0, 0)] == Color::FREE && board[Position(0, 1)] == player) +
                (board[Position(0, 0)] == Color::FREE && board[Position(1, 0)] == player) +
                (board[Position(0, 7)] == Color::FREE && board[Position(0, 6)] == player) +
                (board[Position(0, 7)] == Color::FREE && board[Position(1, 7)] == player) +
                (board[Position(7, 0)] == Color::FREE && board[Position(7, 1)] == player) +
                (board[Position(7, 0)] == Color::FREE && board[Position(6, 0)] == player) +
                (board[Position(7, 7)] == Color::FREE && board[Position(7, 6)] == player) +
                (board[Position(7, 7)] == Color::FREE && board[Position(6, 7)] == player));
    }

    int estimate(const Game& game, Color player) override {
        const Board& board = game.getBoard();
        Color opponent = Game::getOppositeColor(player);
        int score = (countCorners(board, player) - countCorners(board, opponent))  * cornerCost;
        score += (countXFields(board, player) - countXFields(board, opponent))  * XFieldCost;
        score += (countCFields(board, player) - countCFields(board, opponent))  * CFieldCost;
		return score;
    }
private:
    int cornerCost;
    int XFieldCost;
    int CFieldCost;
};


class OpeningEstimator : public Estimator {
public:
    OpeningEstimator(int cornerCost, int XFieldCost, int CFieldCost):
            positionEstimator(cornerCost, XFieldCost, CFieldCost) {}
    int estimate(const Game& game, Color player) override {
        return positionEstimator.estimate(game, player) + mobilityEstimator.estimate(game, player);
    }

private:
    PositionEstimator positionEstimator;
    MobilityEstimator mobilityEstimator;
};


class MiddlegameEstimator : public Estimator {
public:
    MiddlegameEstimator(int cornerCost, int XFieldCost, int CFieldCost):
            positionEstimator(cornerCost, XFieldCost, CFieldCost) {}
    int estimate(const Game& game, Color player) override {
        return positionEstimator.estimate(game, player) + mobilityEstimator.estimate(game, player);
    }

private:
    PositionEstimator positionEstimator;
    MobilityEstimator mobilityEstimator;
};


class EndgameEstimator : public Estimator {
public:
    EndgameEstimator(int cornerCost, int XFieldCost):
            positionEstimator(cornerCost, XFieldCost, 0) {}
    int estimate(const Game& game, Color player) override {
        return positionEstimator.estimate(game, player) + mobilityEstimator.estimate(game, player);
    }

private:
    PositionEstimator positionEstimator;
    MobilityEstimator mobilityEstimator;
};


class MyEstimator : public Estimator {
public:
    MyEstimator(int cornerCost, int XFieldCost, int CFieldCost) : openingEstimator(cornerCost, XFieldCost, CFieldCost),
        middlegameEstimator(cornerCost, XFieldCost, CFieldCost), endgameEstimator(cornerCost, XFieldCost) {}

    int estimate(const Game& game, Color player) override  {
        if (game.isGameFinished())
            return scoreEstimator.estimate(game, player);
        else if (game.getMoveNumber() < 20)
            return openingEstimator.estimate(game, player);
        else if (game.getMoveNumber() < 40)
            return middlegameEstimator.estimate(game, player);
        else
            return endgameEstimator.estimate(game, player);
	}
private:
    OpeningEstimator openingEstimator;
    MiddlegameEstimator middlegameEstimator;
    EndgameEstimator endgameEstimator;
    ScoreEstimator scoreEstimator;
};


struct SearchResult {
    SearchResult() {}

    SearchResult(int bestScore, bool isGameFinished, Move bestMove=Move()) :
        score(bestScore), isFinished(isGameFinished), move(bestMove), isValid(true) {}

    SearchResult(bool _isValid) : isValid(_isValid) {}

    int score;
    bool isFinished;
    Move move;
    bool isValid;

    SearchResult operator - () {
        SearchResult result = (*this);
        result.score = -result.score;
        return result;
    }

    SearchResult operator - (int x) {
        SearchResult result = *this;
        result.score -= x;
        return result;
    }

    SearchResult operator + (int x) {
        SearchResult result = *this;
        result.score += x;
        return result;
    }

    bool operator < (const SearchResult& sr) const {
        int result1 = 0;
        if (isFinished) {
            if (score > 0)
                result1 = 1;
            else if (score < 0)
                result1 = -1;
        }

        int result2 = 0;
        if (sr.isFinished) {
            if (sr.score > 0)
                result2 = 1;
            else if (sr.score < 0)
                result2 = -1;
        }

        return (result1 < result2) || (result1 == result2 && score < sr.score);
    }

    bool operator > (const SearchResult& sr) const {
        return sr < (*this);
    }

    bool operator <= (const SearchResult& sr) const {
        return !(sr < (*this));
    }

    bool operator >= (const SearchResult& sr) const {
        return !((*this) < sr);
    }
};

// Stores best move for board state
class TranspositionTable {
public:
    bool isPositionStored(const Board& board) const {
        return transpositionTable.count(board) != 0;
    }

    void store(const Board& board, Move move) {
        transpositionTable[board] = move;
    }

    Move retrieve(const Board& board) {
        return transpositionTable[board];
    }

private:
    std::unordered_map< Board, Move, BoardHasher > transpositionTable;
};


class MyStrategy : public Strategy {
public:
	explicit MyStrategy(MyConstants myConstants) : constants(myConstants),
        myEstimator(myConstants.CORNER_COST, myConstants.X_FIELD_COST, myConstants.C_FIELD_COST) {}

	Move makeMove(const Game& game) override {
		Game gameCopy(game);

		if (game.getMoveNumber() == 0 || constants.TIME_FOR_MOVE < 0.001)
            return game.getPossibleMoves(game.getCurrentColor())[0];

        // iterative deepening
        SearchResult result;
        double remainingTime = constants.TIME_FOR_MOVE - 0.001;
        int depth = 1;
        do {
            clock_t startClock = clock();
            SearchResult newResult = PVS(gameCopy,
                                         SearchResult(-1000000, true),
                                         SearchResult(1000000, true),
                                         depth, remainingTime);
            remainingTime -= double (clock() - startClock + 0.0L) / CLOCKS_PER_SEC;

            if (!newResult.isValid) // happens when thinking time is over
                break;
            result = newResult;

            depth++;
        } while(!result.isFinished);

        return result.move;
	}

private:
	MyConstants constants;
	MyEstimator myEstimator;
	TranspositionTable transpositionTable;

	static bool isCornerField(const Board& board, Position pos) {
        return (pos == Position(0, 0) ||
                pos == Position(0, 7) ||
                pos == Position(7, 0) ||
                pos == Position(7, 7));
	}

	static bool isXField(const Board& board, Position pos) {
        return (pos == Position(1, 1) && board[Position(0, 0)] == FREE) ||
               (pos == Position(6, 1) && board[Position(7, 0)] == FREE) ||
               (pos == Position(1, 6) && board[Position(0, 7)] == FREE) ||
               (pos == Position(6, 6) && board[Position(7, 7)] == FREE);
	}

    static bool isEdgeField(const Board& board, Position pos) {
        return pos.x() == 0 || pos.x() == 7 || pos.y() == 0 || pos.y() == 7;
    }

	std::vector<Move> getPossibleMovesInGoodOrder(const Game& game, Color player) {
        std::vector<Move> moves = game.getPossibleMoves(player);

        if (moves.size() == 1 && moves[0].isPass)
            return moves;

        if (Board::X_DIM != 8 || Board::Y_DIM != 8)
            return moves;

        std::vector<Move> reorderedMoves[7];

        // First we check the move which was stored in transposition table
        bool isRetrievedMovePossible = false;
        if (transpositionTable.isPositionStored(game.getBoard())) {
            Move retrievedMove = transpositionTable.retrieve(game.getBoard());
            for (size_t i = 0; i < moves.size(); i++)
                if (retrievedMove == moves[i]) {
                    std::swap(moves[i], moves[0]);
                    isRetrievedMovePossible = true;
                    reorderedMoves[0].push_back(retrievedMove);
                    break;
                }
        }

        for (size_t i = isRetrievedMovePossible; i < moves.size(); i++) {
            if (game.isMovePossible(moves[i], Game::getOppositeColor(player))) {
                if (isCornerField(game.getBoard(), moves[i].pos)) // If opponent can take corner, the move is important
                    reorderedMoves[1].push_back(moves[i]);
                else if (isXField(game.getBoard(), moves[i].pos)) // No one wants to take X-field
                    reorderedMoves[6].push_back(moves[i]);
                else
                    reorderedMoves[3].push_back(moves[i]);
            } else {
                if (isCornerField(game.getBoard(), moves[i].pos)) // If we can take corner, but opponent can not
                    reorderedMoves[2].push_back(moves[i]);
                else if (isXField(game.getBoard(), moves[i].pos)) // Don't want to take X field
                    reorderedMoves[5].push_back(moves[i]);
                else
                    reorderedMoves[4].push_back(moves[i]);
            }
        }

        size_t i = 0;
        for (size_t k = 0; k < 7; k++)
            for (size_t j = 0; j < reorderedMoves[k].size(); j++, i++)
                moves[i] = reorderedMoves[k][j];
        return moves;
    }

    // Principal Variation Search
    SearchResult PVS(Game& game, SearchResult alpha, SearchResult beta, int subtreeDepth, double remainingTime) {
        if (remainingTime < 0)
            return SearchResult(false);
        clock_t startClock = clock();

		if (subtreeDepth <= 0 || game.isGameFinished())
			return SearchResult(myEstimator.estimate(game, game.getCurrentColor()), game.isGameFinished());

        bool zeroWindowMode = false;
        // order is very important for alpha-beta pruning
        std::vector<Move> moves = getPossibleMovesInGoodOrder(game, game.getCurrentColor());
        alpha.move = moves[0];

		for (Move move : moves) {
            SearchResult result;

			game.makeMove(move);
			if (zeroWindowMode) {
                result = -PVS(game, -alpha - 1, -alpha, subtreeDepth - 1,
                              remainingTime - double(clock() - startClock) / CLOCKS_PER_SEC);
                if (result > alpha)
                    result = -PVS(game, -beta, -alpha, subtreeDepth - 1,
                                  remainingTime - double(clock() - startClock) / CLOCKS_PER_SEC);
            } else {
                result = -PVS(game, -beta, -alpha, subtreeDepth - 1,
                              remainingTime - double(clock() - startClock) / CLOCKS_PER_SEC);
            }
			game.cancelMove();

			if (!result.isValid) // that means thinking time is over
                return result;

            if (result >= beta) {
                transpositionTable.store(game.getBoard(), move);
                beta.move = move;
                return beta;
            }
            if (result > alpha) {
                alpha = result;
                alpha.move = move;
                zeroWindowMode = true;
            }
		}

		transpositionTable.store(game.getBoard(), alpha.move);
        return alpha;
    }
};
