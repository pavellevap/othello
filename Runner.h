#pragma once

#include <memory>

#include "Game.h"
#include "Strategy.h"

class Runner {
public:
	Runner(Strategy* black, Strategy* white) : strategyForBlack(black), strategyForWhite(white) {}

	void makeMove() {
		if (game.getCurrentColor() == BLACK)
			game.makeMove(strategyForBlack->makeMove(game));
		else
			game.makeMove(strategyForWhite->makeMove(game));
	}

	void cancelMove() {
		game.cancelMove();
	}

	void run() {
		while (!game.isGameFinished())
			makeMove();
	}

	const Game& getGame() const {
		return game;
	}

	const std::unique_ptr<Strategy>& getStrategy(Color player) {
        if (player == BLACK)
            return strategyForBlack;
        else if (player == WHITE)
            return strategyForWhite;
	}

private:
	std::unique_ptr<Strategy> strategyForBlack;
	std::unique_ptr<Strategy> strategyForWhite;
	Game game;
};
