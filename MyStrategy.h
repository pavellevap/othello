#pragma once

#include <queue>
#include <unordered_map>
#include <memory>
#include <time.h>

#include "Strategy.h"

class MyConstants {
public:
	MyConstants(int cornerCost, int XFieldCost, int CFieldCost, float timeForMove) :
		CORNER_COST(cornerCost), X_FIELD_COST(XFieldCost), C_FIELD_COST(CFieldCost), TIME_FOR_MOVE(timeForMove) {}
	int CORNER_COST;
	int X_FIELD_COST;
	int C_FIELD_COST;
	float TIME_FOR_MOVE; /// время на ход в секундах
};

class Estimator {
public:
    virtual int estimate(const Game& game, Color player) = 0;
};

class ScoreEstimator : public Estimator {
public:
    int estimate(const Game& game, Color player) {
        return game.getScoreDifference(player);
    }
};

class MobilityEstimator : public Estimator {
public:
    int estimate(const Game& game, Color player) {
        int score = 0;

		const Board& board = game.getBoard();
		for (size_t x = 0; x < Board::X_DIM; x++)
			for (size_t y = 0; y < Board::Y_DIM; y++)
				if (board[Position(x, y)] == FREE) {
					if (game.isMovePossible(Move(Position(x, y), false), player)) {
						score++;
                        if (Board::X_DIM == 8 && Board::Y_DIM == 8) {
                            if ((x == 0 && y == 0) || (x == 0 && y == 7) ||
                                (x == 7 && y == 0) || (x == 7 && y == 7))
                                score++;
                            if (x == 1 && y == 1 && board[Position(0, 0)] == FREE ||
                                x == 1 && y == 6 && board[Position(0, 7)] == FREE ||
                                x == 6 && y == 1 && board[Position(7, 0)] == FREE ||
                                x == 6 && y == 6 && board[Position(7, 7)] == FREE ||
                                x == 1 && y == 0 && board[Position(0, 0)] == FREE ||
                                x == 0 && y == 1 && board[Position(0, 0)] == FREE ||
                                x == 6 && y == 0 && board[Position(7, 0)] == FREE ||
                                x == 7 && y == 1 && board[Position(7, 0)] == FREE ||
                                x == 1 && y == 7 && board[Position(0, 7)] == FREE ||
                                x == 0 && y == 6 && board[Position(0, 7)] == FREE ||
                                x == 6 && y == 7 && board[Position(7, 7)] == FREE ||
                                x == 7 && y == 6 && board[Position(7, 7)] == FREE)
                                score--;
                        }
                    }

					if (game.isMovePossible(Move(Position(x, y), false), game.getOppositeColor(player))) {
						score--;
						if (Board::X_DIM == 8 && Board::Y_DIM == 8) {
                            if (x == 0 && y == 0 || x == 0 && y == 7 ||
                                x == 7 && y == 0 || x == 7 && y == 7)
                                score--;
                            if (x == 1 && y == 1 && board[Position(0, 0)] == FREE ||
                                x == 1 && y == 6 && board[Position(0, 7)] == FREE ||
                                x == 6 && y == 1 && board[Position(7, 0)] == FREE ||
                                x == 6 && y == 6 && board[Position(7, 7)] == FREE ||
                                x == 1 && y == 0 && board[Position(0, 0)] == FREE ||
                                x == 0 && y == 1 && board[Position(0, 0)] == FREE ||
                                x == 6 && y == 0 && board[Position(7, 0)] == FREE ||
                                x == 7 && y == 1 && board[Position(7, 0)] == FREE ||
                                x == 1 && y == 7 && board[Position(0, 7)] == FREE ||
                                x == 0 && y == 6 && board[Position(0, 7)] == FREE ||
                                x == 6 && y == 7 && board[Position(7, 7)] == FREE ||
                                x == 7 && y == 6 && board[Position(7, 7)] == FREE)
                                score++;
                        }
                    }
				}
        return score;
    }
};

class PositionOfStonesEstimator : public Estimator {
public:
    PositionOfStonesEstimator(int _cornerCost, int _XFieldCost, int _CFieldCost) : cornerCost(_cornerCost),
            XFieldCost(_XFieldCost), CFieldCost(_CFieldCost) {}
    int estimate(const Game& game, Color player) {
        int score = 0;
        const Board& board = game.getBoard();

        switch (board[Position(0, 0)]) {
        case FREE:
            if (board[Position(1, 1)] == BLACK)
                score += XFieldCost;
            if (board[Position(1, 1)] == WHITE)
                score -= XFieldCost;
            if (board[Position(0, 1)] == BLACK)
                score += CFieldCost;
            if (board[Position(0, 1)] == WHITE)
                score -= CFieldCost;
            if (board[Position(1, 0)] == BLACK)
                score += CFieldCost;
            if (board[Position(1, 0)] == WHITE)
                score -= CFieldCost;
            break;
        case BLACK:
            score += cornerCost;
            break;
        case WHITE:
            score -= cornerCost;
            break;
        };

        switch (board[Position(0, 7)]) {
        case FREE:
            if (board[Position(1, 6)] == BLACK)
                score += XFieldCost;
            if (board[Position(1, 6)] == WHITE)
                score -= XFieldCost;
            if (board[Position(0, 6)] == BLACK)
                score += CFieldCost;
            if (board[Position(0, 6)] == WHITE)
                score -= CFieldCost;
            if (board[Position(1, 7)] == BLACK)
                score += CFieldCost;
            if (board[Position(1, 7)] == WHITE)
                score -= CFieldCost;
            break;
        case BLACK:
            score += cornerCost;
            break;
        case WHITE:
            score -= cornerCost;
            break;
        };

        switch (board[Position(7, 0)]) {
        case FREE:
            if (board[Position(6, 1)] == BLACK)
                score += XFieldCost;
            if (board[Position(6, 1)] == WHITE)
                score -= XFieldCost;
            if (board[Position(7, 1)] == BLACK)
                score += CFieldCost;
            if (board[Position(7, 1)] == WHITE)
                score -= CFieldCost;
            if (board[Position(6, 0)] == BLACK)
                score += CFieldCost;
            if (board[Position(6, 0)] == WHITE)
                score -= CFieldCost;
            break;
        case BLACK:
            score += cornerCost;
            break;
        case WHITE:
            score -= cornerCost;
            break;
        };

        switch (board[Position(7, 7)]) {
        case FREE:
            if (board[Position(6, 6)] == BLACK)
                score += XFieldCost;
            if (board[Position(6, 6)] == WHITE)
                score -= XFieldCost;
            if (board[Position(7, 6)] == BLACK)
                score += CFieldCost;
            if (board[Position(7, 6)] == WHITE)
                score -= CFieldCost;
            if (board[Position(6, 7)] == BLACK)
                score += CFieldCost;
            if (board[Position(6, 7)] == WHITE)
                score -= CFieldCost;
            break;
        case BLACK:
            score += cornerCost;
            break;
        case WHITE:
            score -= cornerCost;
            break;
        };

        if (player != BLACK)
            score = -score;
		return score;
    }
private:
    int cornerCost;
    int XFieldCost;
    int CFieldCost;
};

class FrontierStonesEstimator : public Estimator {
public:
    int estimate(const Game& game, Color player) {
        return 0;
        /*const Board& board = game.getBoard();
        int score = 0;

        for (size_t x = 0; x < Board::X_DIM; x++)
            for (size_t y = 0; y < Board::Y_DIM; y++) {
                if (board[Position(x, y)] == player)
                    for (size_t dx = -1; dx <= 1; dx++)
                        for (size_t dy = -1; dy <= 1; dy++) {
                            Position pos(x, y);
                            if (pos.add(dx, dy) && board[pos] == FREE) {
                                score--;
                                goto END;
                            }
                        }
                else if (board[Position(x, y)] == game.getOppositeColor(player))
                    for (size_t dx = -1; dx <= 1; dx++)
                        for (size_t dy = -1; dy <= 1; dy++) {
                            Position pos(x, y);
                            if (pos.add(dx, dy) && board[pos] == FREE) {
                                score++;
                                goto END;
                            }
                        }
END: break;
            }
        return score;*/
    }
};

class StableStonesEstimator : public Estimator {
public:
    int estimate(const Game& game, Color player) {
        for (size_t k = 0; k < 2; k++)
            for (size_t i = 0; i < 8; i++)
                for (size_t j = 0; j < 8; j++)
                    isStable[k][i][j] = false;

        Board board = game.getBoard();
        cornersAreStable(board);
        sidesCanBeStable(board);
        insideCanBeStable(board);

        int amountOfStable[2] = {0, 0};
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++) {
                if (isStable[BLACK][i][j])
                    amountOfStable[BLACK]++;
                if (isStable[WHITE][i][j])
                    amountOfStable[WHITE]++;
            }

        int score = amountOfStable[player] - amountOfStable[game.getOppositeColor(player)];

        return score;
    }

private:
    bool isStable[2][Board::X_DIM][Board::Y_DIM];

    bool isPositionStable(const Board& board, Position pos) {
        Color player = board[pos];
        if (player == FREE)
            return false;
        int a, b, c, d;
        a = b = c = d = 0;

        Position curPos = pos;
        if (!curPos.add(1, 0) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            a++;
        curPos = pos;
        if (!curPos.add(-1, 0) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            a++;

        curPos = pos;
        if (!curPos.add(0, 1) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            b++;
        curPos = pos;
        if (!curPos.add(0, -1) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            b++;

        curPos = pos;
        if (!curPos.add(1, 1) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            c++;
        curPos = pos;
        if (!curPos.add(-1, -1) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            c++;

        curPos = pos;
        if (!curPos.add(-1, 1) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            d++;
        curPos = pos;
        if (!curPos.add(1, -1) || board[curPos] == player && isStable[player][curPos.x()][curPos.y()])
            d++;

        return a && b && c && d;
    }

    void cornersAreStable(const Board& board) {
        if (board[Position(0, 0)] != FREE)
            isStable[board[Position(0, 0)]][0][0] = true;
        if (board[Position(0, 7)] != FREE)
            isStable[board[Position(0, 7)]][0][7] = true;
        if (board[Position(7, 0)] != FREE)
            isStable[board[Position(7, 0)]][7][0] = true;
        if (board[Position(7, 7)] != FREE)
            isStable[board[Position(7, 7)]][7][7] = true;
    }

    void sidesCanBeStable(const Board& board) {
        bool isLayerStable = true;
        for (size_t x = 0; x < 8; x++)
            if (board[Position(x, 0)] == FREE)
                isLayerStable = false;
        if (isLayerStable)
            for (size_t x = 0; x < 8; x++)
                isStable[board[Position(x, 0)]][x][0] = true;

        isLayerStable = true;
        for (int x = 0; x < 8; x++)
            if (board[Position(x, 7)] == FREE)
                isLayerStable = false;
        if (isLayerStable)
            for (int x = 0; x < 8; x++)
                isStable[board[Position(x, 7)]][x][7] = true;

        isLayerStable = true;
        for (int y = 0; y < 8; y++)
            if (board[Position(0, y)] == FREE)
                isLayerStable = false;
        if (isLayerStable)
            for (int y = 0; y < 8; y++)
                isStable[board[Position(0, y)]][0][y] = true;

        isLayerStable = true;
        for (int y = 0; y < 8; y++)
            if (board[Position(7, y)] == FREE)
                isLayerStable = false;
        if (isLayerStable)
            for (int y = 0; y < 8; y++)
                isStable[board[Position(7, y)]][7][y] = true;
    }

    void insideCanBeStable(const Board& board) {
        for (int x = 0; x < 8; x++)
            for (int y = 0; y < 8; y++)
                if (board[Position(x, y)] != FREE)
                    isStable[board[Position(x, y)]][x][y] = isPositionStable(board, Position(x, y));
        for (int x = 7; x >= 0; x--)
            for (int y = 0; y < 8; y++)
                if (board[Position(x, y)] != FREE)
                    isStable[board[Position(x, y)]][x][y] = isPositionStable(board, Position(x, y));
        for (int x = 0; x < 8; x++)
            for (int y = 7; y >= 0; y--)
                if (board[Position(x, y)] != FREE)
                    isStable[board[Position(x, y)]][x][y] = isPositionStable(board, Position(x, y));
        for (int x = 7; x >= 0; x--)
            for (int y = 7; y >= 0; y--)
                if (board[Position(x, y)] != FREE)
                    isStable[board[Position(x, y)]][x][y] = isPositionStable(board, Position(x, y));
    }
};

class OpeningEstimator : public Estimator {
public:
    OpeningEstimator(int cornerCost, int XFieldCost, int CFieldCost) : positionOfStonesEstimator(cornerCost, XFieldCost, CFieldCost) {}
    int estimate(const Game& game, Color player) {
        return positionOfStonesEstimator.estimate(game, player) + mobilityEstimator.estimate(game, player);
            //+ frontierStonesEstimator.estimate(game, player);
	}

private:
    PositionOfStonesEstimator positionOfStonesEstimator;
    MobilityEstimator mobilityEstimator;
    FrontierStonesEstimator frontierStonesEstimator;
};

class MiddlegameEstimator : public Estimator {
public:
    MiddlegameEstimator(int cornerCost, int XFieldCost, int CFieldCost) : positionOfStonesEstimator(cornerCost, XFieldCost, CFieldCost) {}
    int estimate(const Game& game, Color player) {
        return positionOfStonesEstimator.estimate(game, player) + mobilityEstimator.estimate(game, player);
            //+ frontierStonesEstimator.estimate(game, player);
	}

private:
    PositionOfStonesEstimator positionOfStonesEstimator;
    MobilityEstimator mobilityEstimator;
    FrontierStonesEstimator frontierStonesEstimator;
};

class EndgameEstimator : public Estimator {
public:
    EndgameEstimator(int cornerCost, int XFieldCost) : positionOfStonesEstimator(cornerCost, XFieldCost, 0) {}
    int estimate(const Game& game, Color player) {
        return positionOfStonesEstimator.estimate(game, player) + mobilityEstimator.estimate(game, player);
	}

private:
    PositionOfStonesEstimator positionOfStonesEstimator;
    MobilityEstimator mobilityEstimator;
};

class MyEstimator : public Estimator {
public:
    MyEstimator(int cornerCost, int XFieldCost, int CFieldCost) : openingEstimator(cornerCost, XFieldCost, CFieldCost),
        middlegameEstimator(cornerCost, XFieldCost, CFieldCost), endgameEstimator(cornerCost, XFieldCost) {}

    int estimate(const Game& game, Color player) {
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
    SearchResult(int bestScore = 0, bool isGameFinished = false, Move bestMove = Move()) :
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

/// Сохраняет информацию о лучшем ходе для позиции на доске.
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
	MyStrategy(MyConstants myConstants) : constants(myConstants),
        myEstimator(myConstants.CORNER_COST, myConstants.X_FIELD_COST, myConstants.C_FIELD_COST) {}

	Move makeMove(const Game& game) {
		Game gameCopy(game);

		if (game.getMoveNumber() == 0 || constants.TIME_FOR_MOVE < 0.001)
            return game.getPossibleMoves(game.getCurrentColor())[0];

        /// iterative deepening
        SearchResult result;
        double remainingTime = constants.TIME_FOR_MOVE - 0.001;
        int deep = 1;
        do {
            clock_t startClock = clock();
            SearchResult newResult = PVS(gameCopy, &myEstimator, SearchResult(-1000000, 1), SearchResult(1000000, 1), deep, remainingTime);
            remainingTime -= (clock() - startClock + 0.0L) / CLOCKS_PER_SEC;

            if (!newResult.isValid)
                break;
            result = newResult;

            deep++;
        } while(!result.isFinished);

        //std::cout << deep << std::endl;
        return result.move;
	}

private:
	MyConstants constants;
	MyEstimator myEstimator;
	TranspositionTable transpositionTable;

	bool isCornerField(const Board& board, Position pos) {
        return pos == Position(0, 0) || pos == Position(0, 7) || pos == Position(7, 0) || pos == Position(7, 7);
	}

	bool isBadXField(const Board& board, Position pos) {
        return pos == Position(1, 1) && board[Position(0, 0)] == FREE ||
               pos == Position(6, 1) && board[Position(7, 0)] == FREE ||
               pos == Position(1, 6) && board[Position(0, 7)] == FREE ||
               pos == Position(6, 6) && board[Position(7, 7)] == FREE;
	}

    bool isEdgeField(const Board& board, Position pos) {
        return pos.x() == 0 || pos.x() == 7 || pos.y() == 0 || pos.y() == 7;
    }

	std::vector<Move> getPossibleMovesInGoodOrder(const Game& game, Color player) {
        std::vector<Move> moves = game.getPossibleMoves(player);

        if (moves.size() == 1 && moves[0].isPass)
            return moves;

        if (Board::X_DIM == 8 && Board::Y_DIM == 8) {
            std::vector<Move> reorderedMoves[7];

            bool isRetrievedMovePossible = false;
            if (transpositionTable.isPositionStored(game.getBoard())) {
                Move retrivedMove = transpositionTable.retrieve(game.getBoard());
                for (size_t i = 0; i < moves.size(); i++)
                    if (retrivedMove == moves[i]) {
                        std::swap(moves[i], moves[0]);
                        isRetrievedMovePossible = true;
                        reorderedMoves[0].push_back(retrivedMove);
                        break;
                    }
            }

            for (size_t i = isRetrievedMovePossible; i < moves.size(); i++) {
                if (game.isMovePossible(moves[i], game.getOppositeColor(player))) {
                    if (isCornerField(game.getBoard(), moves[i].pos))
                        reorderedMoves[1].push_back(moves[i]);
                    else if (isBadXField(game.getBoard(), moves[i].pos))
                        reorderedMoves[6].push_back(moves[i]);
                    else
                        reorderedMoves[3].push_back(moves[i]);
                }
                else {
                    if (isCornerField(game.getBoard(), moves[i].pos))
                        reorderedMoves[2].push_back(moves[i]);
                    else if (isBadXField(game.getBoard(), moves[i].pos))
                        reorderedMoves[5].push_back(moves[i]);
                    else
                        reorderedMoves[4].push_back(moves[i]);
                }
            }

            size_t i = 0;
            for (size_t k = 0; k < 7; k++)
                for (size_t j = 0; j < reorderedMoves[k].size(); j++, i++)
                    moves[i] = reorderedMoves[k][j];
        }
        return moves;
    }

    /// Principal Variation Search
    SearchResult PVS(Game& game, Estimator* estimator, SearchResult alpha, SearchResult beta, int subtreeDeep, double remainingTime) {
        if (remainingTime < 0)
            return SearchResult(false);
        clock_t startClock = clock();

		if (subtreeDeep <= 0 || game.isGameFinished())
			return SearchResult(estimator->estimate(game, game.getCurrentColor()), game.isGameFinished());

        bool zeroWindowMode = false;
        std::vector<Move> moves = getPossibleMovesInGoodOrder(game, game.getCurrentColor());
        alpha.move = moves[0];

		for (Move move : moves) {
            SearchResult result;

			game.makeMove(move);
			if (zeroWindowMode) {
                result = -PVS(game, estimator, -alpha - 1, -alpha, subtreeDeep - 1, remainingTime - (clock() - startClock + 0.0L) / CLOCKS_PER_SEC);
                if (result > alpha)
                    result = -PVS(game, estimator, -beta, -alpha, subtreeDeep - 1, remainingTime - (clock() - startClock + 0.0L) / CLOCKS_PER_SEC);
            }
            else
                result = -PVS(game, estimator, -beta, -alpha, subtreeDeep - 1, remainingTime - (clock() - startClock + 0.0L) / CLOCKS_PER_SEC);
			game.cancelMove();

			if (!result.isValid)
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
