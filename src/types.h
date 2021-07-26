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
  bool operator==(const piece &) const;
};

inline bool piece::operator==(const piece &other) const {
  return pcolor == other.pcolor && ptype == other.ptype;
}

// represent a move
struct move {
  square from, to;
  piece promotion;
  move(square = 0, square = 0, piece = {});
  bool operator==(const move &) const;
};

inline bool move::operator==(const move &other) const {
  return from == other.from && to == other.to && promotion == other.promotion;
}

// represent castling rights
struct castle_rights {
  bool white_short, white_long, black_short, black_long;
  castle_rights();
  std::pair<bool, bool> get_castle_rights(color) const;

  bool operator==(const castle_rights &) const;
  bool operator!=(const castle_rights &) const;
};

inline bool castle_rights::operator==(const castle_rights &other) const {
  if (white_short != other.white_short) return false;
  if (white_long != other.white_long) return false;
  if (black_short != other.black_short) return false;
  if (black_long != other.black_long) return false;
  return true;
}

inline bool castle_rights::operator!=(const castle_rights &other) const {
  return !(*this == other);
}

// represent 64 bit board
using bitboard = uint64_t;

// interface for piece placement
struct board64 {
  bitboard white, black, pawn, knight, bishop, rook, queen, king;

  board64();

  // rotate board individually
  void rotate();

  // clear a piece from a square
  void clear_piece(square);
  // move piece from square to square (does not modify "initial square")
  void move_piece(square, square);

  // set piece at square (overwrites)
  void set_piece(square, piece);
  // return piece at index
  piece get_piece(square) const;

  bool operator==(const board64 &) const;
  bool operator!=(const board64 &) const;
};

inline bool board64::operator==(const board64 &other) const {
  if (white != other.white) return false;
  if (black != other.black) return false;
  if (pawn != other.pawn) return false;
  if (knight != other.knight) return false;
  if (bishop != other.bishop) return false;
  if (rook != other.rook) return false;
  if (queen != other.queen) return false;
  if (king != other.king) return false;
  return true;
}

inline bool board64::operator!=(const board64 &other) const {
  return !(*this == other);
}

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
// bit twiddling hack
inline void reverse_bits32(uint32_t &n) {
  n = (n >> 16) | (n << 16);
  n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);
  n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
  n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
  n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
}
inline void reverse_bits(bitboard &b) {
  uint32_t left = (b >> 32), right = b;
  reverse_bits32(left);
  reverse_bits32(right);
  b = right;
  b = (b << 32) | left;
}

}  // namespace abra

#endif
