#include "ttt.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;
array<int, 9> scores = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int trials = 1000;

const int nr_threads = 10;
mutex ml;
mutex scorelock;
mutex locks;

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
		//lock and unlock score
		scorelock.lock();
		scores[i] = scores[i] + subscores[i];
		scorelock.unlock();
	}
}

void mcTrial(const State &board)
{
	State trialboard = board;
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

void muliplier(const State &board)
{
	for (int i = 0; i < trials; i++)
	{
		mcTrial(board);
	}
}

Move mcMove(State &board, const Player &player)
{
	scores = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	thread *tt = new thread[nr_threads - 1];
	
	//extra threads
	for (int i = 0; i < nr_threads - 1; ++i)
	{
		tt[i] = thread(muliplier, ref(board));

	}
	//main thread
	muliplier(board);

	//Join extra threads
	for (int i = 0; i < nr_threads - 1; ++i)
	{
		tt[i].join();
	}
	return getBestMove(board);
}

int main()
{	
	//srand(time(0));
	map<Player, PlayerType> playerType;
	playerType[Player::X] = PlayerType::Human;
	playerType[Player::O] = PlayerType::Computer;

	State board =
	{
			Player::None, Player::None, Player::None,
			Player::None, Player::None, Player::None,
			Player::None, Player::None, Player::None 
	};
	cout << board << endl;

	vector<Move> moves = getMoves(board);

	while (moves.size() > 0)
	{
		if (playerType[getCurrentPlayer(board)] == PlayerType::Human)
		{
			cout << "+-+-+-+" << endl
				<< "|0|1|2|" << endl
				<< "+-+-+-+" << endl
				<< "|3|4|5|" << endl
				<< "+-+-+-+" << endl
				<< "|6|7|8|" << endl
				<< "+-+-+-+" << endl
				<< endl;
			cout << "Enter a move ( ";
			for (Move m : moves)
				cout << m << " ";
			cout << "): ";
			Move m;
			cin >> m;
			board = doMove(board, m);
		}
		
		else
		{
			board = doMove(board, mcMove(board, getCurrentPlayer(board)));
		}

		cout << board << endl;
		moves = getMoves(board);
	}
	return 0;
}