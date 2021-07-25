#include "types.h"

#include <cassert>

namespace abra {

piece::piece() : pcolor{color::none}, ptype{piece_type::empty} {}
piece::piece(color _color, piece_type _type) : pcolor{_color}, ptype{_type} {
  assert(_color != color::none);
  assert(_type != piece_type::empty);
}

move::move(square _from, square _to, piece _promotion)
    : from{_from}, to{_to}, promotion{_promotion} {}

castle_rights::castle_rights()
    : white_short{false},
      white_long{false},
      black_short{false},
      black_long{false} {}

board64::board64()
    : white{0},
      black{0},
      pawn{0},
      knight{0},
      bishop{0},
      rook{0},
      queen{0},
      king{0} {}

std::pair<bool, bool> castle_rights::get_castle_rights(color c) const {
  using std::make_pair;
  return (c == color::white ? make_pair(white_short, white_long)
                            : make_pair(black_short, black_long));
}

void board64::clear_piece(square i) {
  assert(is_valid_square(i));
  reset_bit(white, i);
  reset_bit(black, i);
  reset_bit(pawn, i);
  reset_bit(knight, i);
  reset_bit(bishop, i);
  reset_bit(rook, i);
  reset_bit(queen, i);
  reset_bit(king, i);
}

// move bits from i to j
void board64::move_piece(square i, square j) {
  assert(is_valid_square(i) && is_valid_square(j));
  move_bit(white, i, j);
  move_bit(black, i, j);
  move_bit(pawn, i, j);
  move_bit(knight, i, j);
  move_bit(bishop, i, j);
  move_bit(rook, i, j);
  move_bit(queen, i, j);
  move_bit(king, i, j);
}

void board64::rotate() {
  reverse_bits(white);
  reverse_bits(black);
  reverse_bits(pawn);
  reverse_bits(knight);
  reverse_bits(bishop);
  reverse_bits(rook);
  reverse_bits(queen);
  reverse_bits(king);
}

void board64::set_piece(square i, piece x) {
  assert(is_valid_square(i));

  clear_piece(i);

  switch (x.pcolor) {
    case color::white:
      set_bit(white, i);
      break;
    case color::black:
      set_bit(black, i);
      break;
    default:
      assert(false && "color is none");
  }

  switch (x.ptype) {
    case piece_type::pawn:
      set_bit(pawn, i);
      break;
    case piece_type::knight:
      set_bit(knight, i);
      break;
    case piece_type::bishop:
      set_bit(bishop, i);
      break;
    case piece_type::rook:
      set_bit(rook, i);
      break;
    case piece_type::queen:
      set_bit(queen, i);
      break;
    case piece_type::king:
      set_bit(king, i);
      break;
    case piece_type::empty:
    default:
      assert(false && "piece type is empty");
  }
}

piece board64::get_piece(square i) const {
  auto pcolor = color{};
  if (test_bit(white, i))
    pcolor = color::white;
  else if (test_bit(black, i))
    pcolor = color::black;
  else
    return piece{};

  auto ptype = piece_type::empty;
  if (test_bit(pawn, i))
    ptype = piece_type::pawn;
  else if (test_bit(knight, i))
    ptype = piece_type::knight;
  else if (test_bit(bishop, i))
    ptype = piece_type::bishop;
  else if (test_bit(rook, i))
    ptype = piece_type::rook;
  else if (test_bit(queen, i))
    ptype = piece_type::queen;
  else if (test_bit(king, i))
    ptype = piece_type::king;
  else
    assert(false && "piece type is empty");  // shouldn't happen

  return piece{pcolor, ptype};
}

}  // namespace abra
