#include "game.h"
#include "movement.h"

namespace abra {

// handles special pawn moves
void game::handle_pawn_move(move m, bool &capture, bool &reset_ep) {
  auto pawn_dir = movement::get_pawn_direction(color_to_move);
  if (m.to == en_passant) {  // en passant
    auto capture_on = m.to - pawn_dir;
    board.clear_piece(capture_on);
    capture = true;
  } else if (m.to == m.from + 2 * pawn_dir) {  // two step push
    en_passant = m.from + pawn_dir;
    reset_ep = false;
  } else if (!m.promotion.is_empty()) {  // promotion
    board.set_piece(m.from, m.promotion);
  }
}

// handle special king moves & update castle flags
void game::handle_king_move(move m) {
  if (std::abs(m.to - m.from) == 2) {
    // hack: only castling involves a two step king move
    auto dir = (m.to - m.from) / 2;
    square to = m.from, from = m.from;
    to += dir;
    if (dir > 0)  // king side
      from += 3;
    else  // queen side
      from -= 4;

    // move the rook (king will be moved by make_move)
    board.move_piece(from, to);
    board.clear_piece(from);
  }

  // cannot castle now
  if (color_to_move == color::white)
    castling.white_long = castling.white_short = false;
  else
    castling.black_long = castling.black_short = false;
}

// update castle flags if rook move
void game::handle_rook_move(move m) {
  auto first_row = color_to_move == color::white ? 7 : 0;
  int r = get_row(m.from), c = get_col(m.from);
  if (c == 0 && r == first_row) {
    auto &cflag = (color_to_move == color::white ? castling.white_long
                                                 : castling.black_long);
    cflag = false;
  } else if (c == 7 && r == first_row) {
    auto &cflag = (color_to_move == color::white ? castling.white_short
                                                 : castling.black_short);
    cflag = false;
  }
}

// make a move
void game::make_move(move m) {
  auto _piece = board.get_piece(m.from);

  // flags to update state
  auto reset_ep = true, pawn_move = false,
       capture = !board.get_piece(m.to).is_empty();

  // handle special moves
  if (_piece.ptype == piece_type::pawn) {
    pawn_move = true;
    handle_pawn_move(m, capture, reset_ep);
  } else if (_piece.ptype == piece_type::king) {
    handle_king_move(m);
  } else if (_piece.ptype == piece_type::rook) {
    handle_rook_move(m);
  }

  // move piece
  board.move_piece(m.from, m.to);
  board.clear_piece(m.from);

  // update other states
  color_to_move = get_opposite_color(color_to_move);
  if (reset_ep) en_passant = null_square;
  if (color_to_move == color::white) fullmove++;
  if (!pawn_move && !capture) halfmove_cnt++;
}

}  // namespace abra
