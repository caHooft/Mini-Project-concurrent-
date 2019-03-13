#ifndef h4
#define h4

#include <tuple>
#include <array>
#include <vector>
#include <ctime>
#include <random>
#include <iterator>
#include <iostream>

enum class Player { X, O, None };
using Move = int;
using State = std::array<Player, 9>;

// used to get a random element from a container
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g)
{
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return select_randomly(start, end, gen);
}

std::ostream &operator<<(std::ostream &os, const State &state);
std::ostream &operator<<(std::ostream &os, const Player &player);

Player getCurrentPlayer(const State &state);
State doMove(const State &state, const Move &m);
Player getWinner(const State &state);
std::vector<Move> getMoves(const State &state);

#endif