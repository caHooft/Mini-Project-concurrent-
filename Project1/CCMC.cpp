#include "ttt.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
array<int, 9> scores = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
State topboard;

const int nr_threads = 1000;
mutex ml;
mutex scorelock;
mutex fucklocks;

enum class PlayerType
{
	Human,
	Computer
};

void mcUpdateScores(array<int, 9> &subscores, State &trialboard, Player &winner)
{
	for (int i = 0; i < 9; i++)
	{
		if (winner == Player::X)
		{
			if (trialboard[i] == Player::X)
			{
				subscores[i]++;
			}

			if (trialboard[i] == Player::O)
			{
				subscores[i]--;
			}
		}

		if (winner == Player::O)
		{
			if (trialboard[i] == Player::X)
			{
				subscores[i]--;
			}

			if (trialboard[i] == Player::O)
			{
				subscores[i]++;
			}
		}
	}

	for (int i = 0; i < 9; i++)
	{
		scorelock.lock();
		scores[i] = scores[i] + subscores[i];
		scorelock.unlock();
	}
}

State mcTrial(const State &board)
{
	State trialboard = topboard;
	array<int, 9> subscores = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Player winner;

	vector<Move> moves = getMoves(trialboard);

	while (moves.size() != 0)
	{
		trialboard = doMove(trialboard, moves[(rand() % moves.size())]);
		moves = getMoves(trialboard);
	}

	winner = getWinner(trialboard);

	mcUpdateScores(subscores, trialboard, winner);

	fucklocks.lock();
	topboard = board;
	fucklocks.unlock();
	return board;
}

Move getBestMove(State &board)
{
	int highest = -9999;
	int index = -1;

	for (int i = 0; i < 9; i++)
	{
		if (scores[i] > highest && board[i] == Player::None)
		{
			highest = scores[i];
			index = i;
		}
	}

	return index;
}

Move mcMove(State &board, const Player &player)
{
	topboard = board;
	scores = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	/*
	for (int i = 0; i < 10000; i++)
	{
		board = mcTrial(board);
	}
	*/

	thread threads[nr_threads];

	for (int i = 0; i < nr_threads; ++i)
	{
		ml.lock();

		threads[i] = thread(mcTrial, topboard);

		ml.unlock();
	}

	// Join threads
	for (auto &th : threads)
	{
		th.join();
	}
	
	return getBestMove(topboard);
}

int main()
{
	std::srand(std::time(0));

	std::map<Player, PlayerType> playerType;
	playerType[Player::X] = PlayerType::Human;
	playerType[Player::O] = PlayerType::Computer;

	State board =
	{
			Player::None, Player::None, Player::None,
			Player::None, Player::None, Player::None,
			Player::None, Player::None, Player::None };
	std::cout << board << std::endl;

	std::vector<Move> moves = getMoves(board);
	while (moves.size() > 0)
	{
		if (playerType[getCurrentPlayer(board)] == PlayerType::Human)
		{
			std::cout << "+-+-+-+" << std::endl
				<< "|0|1|2|" << std::endl
				<< "+-+-+-+" << std::endl
				<< "|3|4|5|" << std::endl
				<< "+-+-+-+" << std::endl
				<< "|6|7|8|" << std::endl
				<< "+-+-+-+" << std::endl
				<< std::endl;
			std::cout << "Enter a move ( ";
			for (Move m : moves)
				std::cout << m << " ";
			std::cout << "): ";
			Move m;
			std::cin >> m;
			board = doMove(board, m);
		}

		else
		{
			board = doMove(board, mcMove(board, getCurrentPlayer(board)));
		}
		std::cout << board << std::endl;
		moves = getMoves(board);
	}

	return 0;
}