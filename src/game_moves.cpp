#include "game.h"
#include "movement.h"

namespace abra {

bitboard game::get_attacks(color c) const {
  auto attacks = bitboard{0};
  auto &colorb = get_colorb(c);
  auto pawn_dir = movement::get_pawn_direction(c);
  for (square i = 0; i < 64; i++) {
    if (test_bit(colorb, i)) {
      if (!test_bit(board.pawn, i)) {
        attacks |= get_moves(i);
      } else {
        int col = get_col(i);
        if (col != 0) set_bit(attacks, i + pawn_dir + movement::left);
        if (col != 7) set_bit(attacks, i + pawn_dir + movement::right);
      }
    }
  }
  return attacks;
}

bool game::under_attack(const bitboard &squares, color c) const {
  return static_cast<bool>(squares & get_attacks(c));
}

bitboard game::get_moves(square i) const {
  auto x = board.get_piece(i);
  switch (x.ptype) {
    case piece_type::pawn:
      return get_pawn_moves(i);
    case piece_type::knight:
      return get_knight_moves(i);
    case piece_type::bishop:
      return get_bishop_moves(i);
    case piece_type::rook:
      return get_rook_moves(i);
    case piece_type::queen:
      return get_queen_moves(i);
    case piece_type::king:
      return get_king_moves(i);
  }
  assert(false);
}

// return list of available (legal) moves
std::vector<move> game::get_moves() const {}

}  // namespace abra
