#include "game.h"

namespace abra {

const std::string _initial_fen{
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

game::game() : game(_initial_fen) {}

// return list of available (legal) moves
std::vector<move> game::get_moves() const {}

// returns true iff game is over
bool game::is_terminal() const {}

// returns the result of the game (should be terminal state)
color game::get_result() const {}

// return true if color is in check
bool game::in_check(color) const {}

}  // namespace abra
