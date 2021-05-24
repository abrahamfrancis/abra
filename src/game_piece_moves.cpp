#include <array>

#include "game.h"
#include "movement.h"

#include <iostream>

namespace abra {

// pawns

bitboard game::get_pawn_moves(square s, color c) const {
  auto moves = bitboard{0};
  auto pawn_dir = movement::get_pawn_direction(c);
  auto start_row = (c == color::white) ? 6 : 1;
  auto vacant = ~bitboard{board.black | board.white};
  auto row = get_row(s), col = get_col(s);

  square t = s + pawn_dir;

  // pawn push
  if (test_bit(vacant, t)) {
    set_bit(moves, t);
    if (row == start_row) {
      t += pawn_dir;  // double pawn push
      if (test_bit(vacant, t)) set_bit(moves, t);
    }
  }

  // pawn captures

  // add pawn capture along direction (including en passant)
  auto add_capture = [&](square dir) {
    auto edge = (dir == movement::left ? 0 : 7);
    if (col == edge) return;  // edge of board
    auto sq = s + pawn_dir + dir;
    // if square has piece or is an en passant target
    if (!test_bit(vacant, sq) || sq == en_passant) set_bit(moves, sq);
  };

  add_capture(movement::left);
  add_capture(movement::right);

  return moves;
}

// short range pieces

inline static bool is_valid_dimension(square i) { return 0 <= i && i < 8; }

bitboard game::get_knight_moves(square s) const {
  auto moves = bitboard{0};
  constexpr std::array<std::pair<int, int>, 8> offsets{
      {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}}};
  int r = get_row(s), c = get_col(s);
  for (auto [dr, dc] : offsets) {
    auto i = r + dr, j = c + dc;
    if (!is_valid_dimension(i) || !is_valid_dimension(j)) continue;
    set_bit(moves, square{8 * i + j});
  }
  return moves;
}

bitboard game::get_king_moves(square s) const {
  auto moves = bitboard{0};
  constexpr std::array<std::pair<int, int>, 8> offsets{
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
  int r = get_row(s), c = get_col(s);
  for (auto [dr, dc] : offsets) {
    auto i = r + dr, j = c + dc;
    if (!is_valid_dimension(i) || !is_valid_dimension(j)) continue;
    set_bit(moves, square{8 * i + j});
  }
  return moves;
}

bitboard game::get_bishop_moves(square s) const {
  auto moves = bitboard{0};
  constexpr std::array<std::pair<int, int>, 4> dirs{
      {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
  int r = get_row(s), c = get_col(s);
  auto occupied = bitboard{board.black | board.white};
  for (auto [dr, dc] : dirs) {
    auto i = r + dr, j = c + dc;
    while (is_valid_dimension(i) && is_valid_dimension(j)) {
      set_bit(moves, square{8 * i + j});
      if (test_bit(occupied, square{8 * i + j})) break;
      i += dr;
      j += dc;
    }
  }
  return moves;
}

bitboard game::get_rook_moves(square s) const {
  auto moves = bitboard{0};
  constexpr std::array<std::pair<int, int>, 4> dirs{
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
  int r = get_row(s), c = get_col(s);
  auto occupied = bitboard{board.black | board.white};
  for (auto [dr, dc] : dirs) {
    auto i = r + dr, j = c + dc;
    while (is_valid_dimension(i) && is_valid_dimension(j)) {
      auto sq = square{8 * i + j};
      set_bit(moves, sq);
      if (test_bit(occupied, sq)) break;
      i += dr;
      j += dc;
    }
  }
  return moves;
}

bitboard game::get_queen_moves(square s) const {
  return bitboard{get_bishop_moves(s) | get_rook_moves(s)};
}

}  // namespace abra
