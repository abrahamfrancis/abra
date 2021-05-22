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

  // helpers for make_move
  void handle_pawn_move(move, bool &, bool &);
  void handle_king_move(move);
  void handle_rook_move(move);

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

  // make a move and update castle flags (not reversible)
  void make_move(move);

  // returns true iff game is over
  bool is_terminal() const;

  // returns the result of the game (should be terminal state)
  color get_result() const;

  // return true if color is in check
  bool in_check(color) const;

  // returns piece at square
  piece piece_at(square) const;
};

inline color game::get_color_to_move() const { return color_to_move; }
inline piece game::piece_at(square i) const { return board.get_piece(i); }

}  // namespace abra

#endif
