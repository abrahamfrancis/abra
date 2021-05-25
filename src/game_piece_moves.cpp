#include "game.h"
#include "movement.h"

namespace abra {

using namespace movement;

constexpr bitboard get_rowb(int r) {
  auto rowb = bitboard{0};
  for (square s = 0; s < 8; s++) set_bit(rowb, 8 * r + s);
  return rowb;
}
constexpr bitboard get_colb(int c) {
  auto colb = bitboard{0};
  for (square s = 0; s < 8; s++) set_bit(colb, 8 * s + c);
  return colb;
}
inline bitboard shift_board(bitboard b, square dir) {
  if (dir < 0)
    b >>= -dir;
  else
    b <<= dir;
  return b;
}

constexpr bitboard col0 = get_colb(0), col1 = get_colb(1), col6 = get_colb(6),
                   col7 = get_colb(7);
constexpr bitboard row0 = get_rowb(0), row1 = get_rowb(1), row7 = get_rowb(7),
                   row6 = get_rowb(6);

// pawns

bitboard game::get_pawn_attacks(bitboard b, color c) const {
  auto moves = bitboard{0};
  if (c == color::white)
    b = shift_board(b, up);
  else
    b = shift_board(b, down);
  moves |= shift_board(b & ~col0, left);
  moves |= shift_board(b & ~col7, right);
  return moves;
}

bitboard game::get_pawn_moves(bitboard b, color c) const {
  auto vacant = ~bitboard{board.black | board.white};
  auto moves = get_pawn_attacks(b, c) & (~vacant | to_bitboard(en_passant));
  auto pawn_dir = get_pawn_direction(c);
  auto &start_rowb = (c == color::white) ? row6 : row1;

  // one step push
  moves |= shift_board(b, pawn_dir) & vacant;

  // two step push
  auto ps1 = shift_board(b & start_rowb, pawn_dir) & vacant;
  moves |= shift_board(ps1, pawn_dir) & vacant;

  return moves;
}

// short range pieces

bitboard game::get_knight_moves(bitboard b) const {
  auto moves = bitboard{0};
  moves |= shift_board(b & ~(col0 | row0 | row1), 2 * up + left);
  moves |= shift_board(b & ~(col7 | row0 | row1), 2 * up + right);
  moves |= shift_board(b & ~(col0 | row6 | row7), 2 * down + left);
  moves |= shift_board(b & ~(col7 | row6 | row7), 2 * down + right);
  moves |= shift_board(b & ~(col0 | col1 | row0), 2 * left + up);
  moves |= shift_board(b & ~(col0 | col1 | row7), 2 * left + down);
  moves |= shift_board(b & ~(col6 | col7 | row0), 2 * right + up);
  moves |= shift_board(b & ~(col6 | col7 | row7), 2 * right + down);
  return moves;
}

bitboard game::get_king_moves(bitboard b) const {
  auto moves = bitboard{0};
  moves |= shift_board(b & ~col0, left);
  moves |= shift_board(b & ~col7, right);
  moves |= shift_board(b & ~row0, up);
  moves |= shift_board(b & ~row7, down);
  moves |= shift_board(b & ~(col0 | row0), up + left);
  moves |= shift_board(b & ~(col7 | row0), up + right);
  moves |= shift_board(b & ~(col0 | row7), down + left);
  moves |= shift_board(b & ~(col7 | row7), down + right);
  return moves;
}

// sliding pieces

inline auto get_slide_moves(const bitboard &pieces, bitboard vacant,
                            bitboard invalid, square dir) {
  auto moves = shift_board(pieces & ~invalid, dir);
  vacant &= ~invalid;
  for (int i = 0; i < 6; i++) moves |= shift_board(moves & vacant, dir);
  return moves;
}

bitboard game::get_bishop_moves(bitboard b) const {
  auto vacant = ~bitboard{board.black | board.white};
  auto moves = bitboard{0};
  moves |= get_slide_moves(b, vacant, row0 | col0, up + left);
  moves |= get_slide_moves(b, vacant, row0 | col7, up + right);
  moves |= get_slide_moves(b, vacant, row7 | col0, down + left);
  moves |= get_slide_moves(b, vacant, row7 | col7, down + right);
  return moves;
}

bitboard game::get_rook_moves(bitboard b) const {
  auto vacant = ~bitboard{board.black | board.white};
  auto moves = bitboard{0};
  moves |= get_slide_moves(b, vacant, row0, up);
  moves |= get_slide_moves(b, vacant, row7, down);
  moves |= get_slide_moves(b, vacant, col0, left);
  moves |= get_slide_moves(b, vacant, col7, right);
  return moves;
}

bitboard game::get_queen_moves(bitboard b) const {
  return bitboard{get_bishop_moves(b) | get_rook_moves(b)};
}

}  // namespace abra
