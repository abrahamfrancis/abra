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

  // return list of all moves except castling for piece at square
  // ignores ally capture or checks
  bitboard get_moves(square) const;
  // helpers
  bitboard get_pawn_moves(bitboard, color) const;
  bitboard get_pawn_attacks(bitboard, color) const;
  bitboard get_knight_moves(bitboard) const;
  bitboard get_bishop_moves(bitboard) const;
  bitboard get_rook_moves(bitboard) const;
  bitboard get_queen_moves(bitboard) const;
  bitboard get_king_moves(bitboard) const;  // doesnt include castling

  // return list of all pseudo legal moves for color
  std::vector<move> get_moves(color) const;

  // returns a bitboard containing all squares attacked by color
  // pieces attacked by same color are considered attacked
  bitboard get_attacks(color) const;

  // returns true if any of squares are under attack by color
  // helper for in_check & to check if you can castle
  // calls: get_attacks
  bool under_attack(const bitboard &, color) const;

  bool is_material_insufficient() const;

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
  std::vector<move> get_moves(bool = false) const;

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

  // return a copy of the board
  board64 get_board() const;

  castle_rights get_castle_rights() const;

  square get_en_passant_sq() const;

  bool operator==(const game &) const;
};

inline bool game::operator==(const game &other) const {
  if (board != other.board) return false;
  if (color_to_move != other.color_to_move) return false;
  if (castling != other.castling) return false;
  if (en_passant != other.en_passant) return false;
  return true;
}

inline square game::get_en_passant_sq() const { return en_passant; }

inline castle_rights game::get_castle_rights() const { return castling; }

inline const bitboard &game::get_colorb(color c) const {
  return (c == color::white ? board.white : board.black);
}
inline board64 game::get_board() const { return board; }
inline color game::get_color_to_move() const { return color_to_move; }
inline piece game::piece_at(square i) const { return board.get_piece(i); }

}  // namespace abra

#endif
