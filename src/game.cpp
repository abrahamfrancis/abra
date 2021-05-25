#include "game.h"

#include <algorithm>

namespace abra {

const std::string _initial_fen{
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

game::game() : game(_initial_fen) {}

bool game::is_material_insufficient() const {
  if (board.pawn) return false;  // if pawns exist, no
  auto major_pieces = bitboard{board.queen | board.rook};
  if (major_pieces) return false;  // if major pieces exist, no
  auto minor_pieces = bitboard{board.knight | board.bishop};
  auto black_minor_pieces = popcount(bitboard{board.black & minor_pieces}),
       white_minor_pieces = popcount(bitboard{board.white & minor_pieces});
  return black_minor_pieces <= 1 &&
         white_minor_pieces <= 1;  // if both sides have <= 1 minor piece, no
}

// returns true iff game is over
bool game::is_terminal() const {
  if (halfmove_cnt >= 100 || is_material_insufficient()) return true;
  if (get_moves().empty()) return true;
  return false;
}

// returns the result of the game (should be terminal state)
color game::get_result() const {
  if (halfmove_cnt >= 100 || is_material_insufficient()) return color::none;
  // reaching here means get_moves() is empty
  if (in_check(color_to_move)) return get_opposite_color(color_to_move);
  return color::none;  // stalemate
}

// return true if color is in check
bool game::in_check(color c) const {
  assert(c != color::none);
  auto &colorb = get_colorb(c);
  return under_attack(bitboard{colorb & board.king}, get_opposite_color(c));
}

std::vector<move> game::get_moves(bool captures_only) const {
  auto all_moves = get_moves(color_to_move);
  auto legal_moves = std::vector<move>{};
  legal_moves.reserve(all_moves.size());
  auto vacant = ~bitboard{board.black | board.white};
  std::copy_if(all_moves.begin(), all_moves.end(),
               std::back_inserter(legal_moves), [&](move m) {
                 // check if legal
                 if (captures_only && test_bit(vacant, m.to)) return false;
                 auto test_game = game{*this};
                 test_game.make_move(m);
                 return !test_game.in_check(color_to_move);
               });
  return legal_moves;
}

}  // namespace abra
