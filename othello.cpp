#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstdlib>
#include <fstream>

#include "Runner.h"
#include "Game.h"
#include "Strategy.h"
#include "MyStrategy.h"
#include "Board.h"

using namespace std;

void printBoard(const Board& board) {
	cout << ' ';
	for (size_t i = 0; i < Board::X_DIM; i++)
		cout << char(i + 'a');
	cout << endl;

	for (size_t i = 0; i < Board::X_DIM; i++) {
		cout << i + 1;
		for (size_t j = 0; j < Board::Y_DIM; j++)
			switch(board[Position(i, j)]) {
			case WHITE:
				cout << 'W';
				break;
			case BLACK:
				cout << 'B';
				break;
			case FREE:
				cout << '.';
				break;
			};
		cout << i + 1 << endl;
	}
	cout << ' ';
	for (size_t i = 0; i < Board::X_DIM; i++)
		cout << char(i + 'a');
	cout << endl;

	cout << endl;
}

MyConstants findConstants(double timeForMove) {
	srand(1);
	MyConstants bestConstants(10, -5, -2, timeForMove);

    int count = 0;
	for (size_t i = 0; i < 10000000; i++) {
		int cornerCost = rand() % 20 + 1;
		int XFieldCost = -(rand() % 20 + 1);
		int CFieldCost = -(rand() % 20 + 1);
		cout << i << endl;
		cout << "best " << bestConstants.CORNER_COST << ' ';
        cout << bestConstants.X_FIELD_COST << ' ';
        cout << bestConstants.C_FIELD_COST << ' ';
        cout << count << endl;

		MyConstants newConstants(cornerCost, XFieldCost, CFieldCost, timeForMove);

		int score = 0;

		Strategy* black = new MyStrategy(newConstants);
		Strategy* white = new MyStrategy(bestConstants);
		Runner run1(black, white);
		run1.run();
		score += run1.getGame().getScoreDifference(BLACK);
		cout << run1.getGame().getScoreDifference(BLACK) << ' ';

		black = new MyStrategy(bestConstants);
		white = new MyStrategy(newConstants);
		Runner run2(black, white);
		run2.run();
		score += run2.getGame().getScoreDifference(WHITE);
		cout << run2.getGame().getScoreDifference(WHITE) << ' ';

		cout << score << endl;
		if (score > 0) {
            count = 0;
			bestConstants = newConstants;
        } else {
            count++;
        }
	}

	cout << bestConstants.CORNER_COST << endl;
	cout << bestConstants.X_FIELD_COST << endl;
	cout << bestConstants.C_FIELD_COST << endl;
	return bestConstants;
}

void playWithServer() {
    string s;
    cin >> s >> s;
    Strategy* white;
    Strategy* black;
    if (s == "white") {
        white = new MyStrategy(MyConstants(10, -5, -2, 2.9));
        black = new ServerStrategy(cin, cout);
    }
    else {
        black = new MyStrategy(MyConstants(10, -5, -2, 2.9));
        white = new ServerStrategy(cin, cout);
    }

	Runner runner(black, white);
	runner.run();
}

int main(int argc, const char* argv[]) {
	srand(0);

	string color = "black";
	if (argc > 1)
	    color = argv[1];

	double time = 1.0;
	if (argc > 2)
	    time = stoi(argv[2]) / 1000.0;

    Strategy* black;
    Strategy* white;

    if (color == "black") {
        black = new StreamStrategy(cin, cout);
        white = new MyStrategy(MyConstants(10, -5, -2, time));
    } else {
        black = new MyStrategy(MyConstants(10, -5, -2, time));
        white = new StreamStrategy(cin, cout);
    }

	Runner runner(black, white);

    printBoard(runner.getGame().getBoard());
	while (!runner.getGame().isGameFinished()) {
		runner.makeMove();

		printBoard(runner.getGame().getBoard());
	}

	cout << "black: " << runner.getGame().getScore(BLACK) << endl;
	cout << "white: " << runner.getGame().getScore(WHITE) << endl;
	if (runner.getGame().getScoreDifference(BLACK) > 0)
	    cout << "Black won!\n";
    else if (runner.getGame().getScoreDifference(BLACK) < 0)
        cout << "White won!\n";
    else
        cout << "Draw!\n";

	return 0;
}
