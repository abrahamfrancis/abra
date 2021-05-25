#ifndef ABRA_TYPES_H
#define ABRA_TYPES_H

#include <bitset>
#include <cassert>
#include <cstdint>
#include <utility>

namespace abra {

// represent color flags for pieces
enum class color { none, white, black };

// represent piece type flags for pieces
enum class piece_type { empty, pawn, knight, bishop, rook, queen, king };

// to index 64 bit board
using square = int;

// represent a piece
struct piece {
  color pcolor;
  piece_type ptype;
  piece();
  piece(color, piece_type);
  bool is_empty() const;
  bool operator==(const piece &other) {
    return pcolor == other.pcolor && ptype == other.ptype;
  }
};

// represent a move
struct move {
  square from, to;
  piece promotion;
  move(square = 0, square = 0, piece = {});
  bool operator==(const move &other) {
    return from == other.from && to == other.to && promotion == other.promotion;
  }
};

// represent castling rights
struct castle_rights {
  bool white_short, white_long, black_short, black_long;
  castle_rights();
  std::pair<bool, bool> get_castle_rights(color) const;
};

// represent 64 bit board
using bitboard = uint64_t;

// interface for piece placement
struct board64 {
  bitboard white, black, pawn, knight, bishop, rook, queen, king;

  board64();

  // clear a piece from a square
  void clear_piece(square);
  // move piece from square to square (does not modify "initial square")
  void move_piece(square, square);

  // set piece at square (overwrites)
  void set_piece(square, piece);
  // return piece at index
  piece get_piece(square) const;
};

inline color get_opposite_color(color c) {
  assert(c != color::none);
  return (c == color::white ? color::black : color::white);
}

constexpr square null_square{-1};
inline bool is_valid_square(square s) { return 0 <= s && s < 64; }
inline int get_row(square s) { return s / 8; }
inline int get_col(square s) { return s % 8; }

inline bool piece::is_empty() const { return pcolor == color::none; }

// bitboard operations
inline constexpr bitboard to_bitboard(square i) { return bitboard{1} << i; }
inline constexpr void set_bit(bitboard &b, square i) { b |= to_bitboard(i); }
inline constexpr void reset_bit(bitboard &b, square i) { b &= ~to_bitboard(i); }
inline constexpr void flip_bit(bitboard &b, square i) { b ^= to_bitboard(i); }
inline constexpr bool test_bit(const bitboard &b, square i) {
  return static_cast<bool>(b & to_bitboard(i));
}
inline constexpr void move_bit(bitboard &b, square f, square t) {
  if (test_bit(b, f) != test_bit(b, t)) flip_bit(b, t);
}
inline int popcount(const bitboard &b) {
  return static_cast<int>(std::bitset<64>{b}.count());
}

}  // namespace abra

#endif
