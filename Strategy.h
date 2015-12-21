#pragma once

#include <iostream>
#include <fstream>

#include "Game.h"

class Strategy {
public:
	virtual Move makeMove(const Game& game) = 0;
	virtual ~Strategy() {}
};

class StreamStrategy : public Strategy {
public:
	StreamStrategy (std::istream& _in, std::ostream& _out) : in(_in), out(_out) {}
	Move makeMove(const Game& game) {
		if (game.getMoveNumber() != 0) {
            if (game.getMoves().size() == 1 && game.getMoves()[0].isPass)
                out << "Pass\n";
            else
                out << char(game.getMoves().back().pos.y() + 'a') << ' ' <<
                            game.getMoves().back().pos.x() + 1 << std::endl;
            out.flush();
		}

        std::vector<Move> moves = game.getPossibleMoves(game.getCurrentColor());
        if (moves.size() == 1 && moves[0].isPass) {
            std::cout << "Pass\n";
            return moves[0];
        }

        int x;
        char y;
        in >> y >> x;
        return Move(Position(x - 1, y - 'a'), false);
	}

	~StreamStrategy() {}

private:
	std::istream& in;
	std::ostream& out;
};

class ServerStrategy : public Strategy {
public:
	ServerStrategy (std::istream& _in, std::ostream& _out) : in(_in), out(_out) {}
	Move makeMove(const Game& game) {
		if (game.getMoveNumber() != 0) {
		    std::string s;
		    in >> s;

		    if (s == "turn") {
                out << "move " <<
                       char(game.getMoves().back().pos.y() + 'a') << ' ' <<
                            game.getMoves().back().pos.x() + 1 << std::endl;
                out.flush();
            }
		}

        std::vector<Move> moves = game.getPossibleMoves(game.getCurrentColor());
        if (moves.size() == 1 && moves[0].isPass)
            return moves[0];

        std::string s;
        in >> s;
        if (s == "move") {
            int x;
            char y;
            in >> y >> x;

            return Move(Position(x - 1, y - 'a'), false);
        }
        else
            return moves[0];
	}

	~ServerStrategy() {}

private:
	std::istream& in;
	std::ostream& out;
};

class RandomStrategy : public Strategy {
	Move makeMove(const Game& game) {
		std::vector<Move> moves = game.getPossibleMoves(game.getCurrentColor());
		return moves[rand() % moves.size()];
	}
};
