#include "game.h"

namespace abra {

const std::string _initial_fen{
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

game::game() : game(_initial_fen) {}

// returns true iff game is over
bool game::is_terminal() const {}

// returns the result of the game (should be terminal state)
color game::get_result() const {}

// return true if color is in check
bool game::in_check(color c) const {
  assert(c != color::none);
  auto &colorb = get_colorb(c);
  return under_attack(bitboard{colorb & board.king}, c);
}

}  // namespace abra
