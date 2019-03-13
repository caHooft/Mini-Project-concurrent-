// ttt.cpp

#include "ttt.h"

std::ostream &operator<<(std::ostream &os, const Player &player)
{
	switch (player)
	{
	case Player::X: os << "X"; break;
	case Player::O: os << "O"; break;
	case Player::None: os << " "; break;
	default: os.setstate(std::ios_base::failbit);
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, const State &state)
{
	for (int r = 0; r < 3; r++) 
	{
		os << "+-+-+-+" << std::endl;
		for (int c = 0; c < 3; c++)
		{
			os << "|" << state[r * 3 + c];
		}
		os << "|" << std::endl;
	}
	os << "+-+-+-+" << std::endl;
	return os;
}

Player getCurrentPlayer(const State &state)
{
	int countX = 0;
	int countO = 0;
	for (int i = 0; i < 9; i++)
	{
		if (state[i] == Player::X)
		{
			countX++;
		}
		else if (state[i] == Player::O)
		{
			countO++;
		}
	}
	return (countX > countO ? Player::O : Player::X);
}

State doMove(const State &state, const Move &m)
{
	State result = state;
	int countX = 0;
	int countO = 0;
	for (int i = 0; i < 9; i++) 
	{
		if (result[i] == Player::X)
		{
			countX++;
		}
		else if (result[i] == Player::O)
		{
			countO++;
		}
	}
	result[m] = (countX > countO ? Player::O : Player::X);
	return result;
}

Player getWinner(const State &state)
{
	for (int r = 0; r < 3; r++)
		if (state[r * 3] == state[r * 3 + 1] && state[r * 3 + 1] == state[r * 3 + 2] && state[r * 3] != Player::None)
			return state[r * 3];
	for (int c = 0; c < 3; c++)
		if (state[0 + c] == state[3 + c] && state[3 + c] == state[6 + c] && state[0 + c] != Player::None)
			return state[0 + c];
	if (state[0] == state[4] && state[4] == state[8] && state[0] != Player::None)
		return state[0];
	if (state[2] == state[4] && state[4] == state[6] && state[2] != Player::None)
		return state[2];
	return Player::None;
}

std::vector<Move> getMoves(const State &state)
{
	std::vector<Move> moves;
	if (getWinner(state) == Player::None)
	{
		for (int i = 0; i < 9; i++)
		{
			if (state[i] == Player::None)
			{
				moves.push_back(i);
			}
		}
	}
	return moves;
}