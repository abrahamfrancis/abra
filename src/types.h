#ifndef ABRA_TYPES_H
#define ABRA_TYPES_H

#include <bitset>
#include <cassert>
#include <cstdint>

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
};

// represent a move
struct move {
  square from, to;
  piece promotion;
  move(square = 0, square = 0, piece = {});
};

// represent castling rights
struct castle_rights {
  bool white_short, white_long, black_short, black_long;
  castle_rights();
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

piece::piece() : pcolor{color::none}, ptype{piece_type::empty} {};
piece::piece(color _color, piece_type _type) : pcolor{_color}, ptype{_type} {
  assert(_color != color::none);
  assert(_type != piece_type::empty);
};
inline bool piece::is_empty() const { return pcolor == color::none; }

move::move(square _from, square _to, piece _promotion)
    : from{_from}, to{_to}, promotion{_promotion} {};

castle_rights::castle_rights()
    : white_short{false},
      white_long{false},
      black_short{false},
      black_long{false} {};

board64::board64()
    : white{0},
      black{0},
      pawn{0},
      knight{0},
      bishop{0},
      rook{0},
      queen{0},
      king{0} {};

// bitboard operations
constexpr bitboard _one{1};
inline void set_bit(bitboard &b, square i) { b |= (_one << i); }
inline void reset_bit(bitboard &b, square i) { b &= ~(_one << i); }
inline void flip_bit(bitboard &b, square i) { b ^= (_one << i); }
inline bool test_bit(const bitboard &b, square i) {
  return static_cast<bool>(b & (_one << i));
}
inline void move_bit(bitboard &b, square f, square t) {
  if (test_bit(b, f) != test_bit(b, t)) flip_bit(b, t);
}
inline int popcount(const bitboard &b) {
  return static_cast<int>(std::bitset<64>{b}.count());
}

}  // namespace abra

#endif
