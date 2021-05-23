#ifndef ABRA_GAME_H
#define ABRA_GAME_H

#include <string>
#include <vector>

#include "types.h"

namespace abra {

class game {
  board64 board;
  color color_to_move;
  castle_rights castling;
  square en_passant;
  int halfmove_cnt, fullmove;

  // return a reference to the bitboard corresponding
  // to specified color
  const bitboard &get_colorb(color) const;

  // helpers for make_move
  void handle_pawn_move(move, bool &, bool &);
  void handle_king_move(move);
  void handle_rook_move(move);

  // returns a bitboard with all squares the piece  can go to
  // "pseudo-legally" (piece should NOT be PAWN)
  bitboard get_moves(square) const;
  bitboard get_pawn_moves(square) const;
  bitboard get_knight_moves(square) const;
  bitboard get_bishop_moves(square) const;
  bitboard get_rook_moves(square) const;
  bitboard get_queen_moves(square) const;
  bitboard get_king_moves(square) const;

  // returns a bitboard containing all squares attacked by color
  bitboard get_attacks(color) const;

  // returns true if any of squares are under attack by color
  // helper for in_check & to check if you can castle
  // calls: get_attacks
  bool under_attack(const bitboard &, color) const;

 public:
  // create new game with default start position
  game();

  // create new game according to FEN
  game(const std::string &);

  // convert current state to FEN
  std::string to_fen() const;

  // return the color which has to move now if game is not over
  color get_color_to_move() const;

  // return list of available (legal) moves
  std::vector<move> get_moves() const;

  // make a move and update state
  void make_move(move);

  // returns true iff game is over
  bool is_terminal() const;

  // returns the result of the game (should be terminal state)
  color get_result() const;

  // return true if color is in check
  // calls under_attack
  bool in_check(color) const;

  // returns piece at square
  piece piece_at(square) const;
};

inline const bitboard &game::get_colorb(color c) const {
  return (c == color::white ? board.white : board.black);
}
inline color game::get_color_to_move() const { return color_to_move; }
inline piece game::piece_at(square i) const { return board.get_piece(i); }

}  // namespace abra

#endif
