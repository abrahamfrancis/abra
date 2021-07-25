#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "game.h"
#include "notation.h"
#include "types.h"

namespace abra {

game::game(const std::string& fen) : board{} {
  auto tokens = notation::split_string(fen, ' ');
  if (tokens.size() != 6)
    throw new std::invalid_argument("fen '" + fen +
                                    "' does not have exactly 6 fields");

  auto &piece_placement = tokens[0], &active_color = tokens[1],
       &castling_ability = tokens[2], &en_passant_target = tokens[3],
       &halfmove_clk = tokens[4], &fullmove_no = tokens[5];

  auto rows = notation::split_string(piece_placement, '/');
  if (rows.size() != 8)
    throw new std::invalid_argument(
        "fen '" + fen + "' should have exactly 8 rows in piece placement");
  board = notation::to_chess_board(rows);
  if (popcount(board.white & board.king) != 1)
    throw new std::invalid_argument(
        "fen '" + fen +
        "' should have exactly 1 white king in piece placement");
  if (popcount(board.black & board.king) != 1)
    throw new std::invalid_argument(
        "fen '" + fen +
        "' should have exactly 1 black king in piece placement");

  color_to_move = notation::to_color(active_color);

  castling = notation::to_castle_rights(castling_ability);
  if (!std::is_sorted(castling_ability.begin(), castling_ability.end()))
    throw new std::invalid_argument("fen '" + fen +
                                    "' has unsorted castling ability");

  // store invalid square, if no en passant (hack)
  en_passant = (en_passant_target == "-")
                   ? null_square
                   : notation::to_square(en_passant_target);

  halfmove_cnt = std::stoi(halfmove_clk);
  fullmove = std::stoi(fullmove_no);

  if (in_check(get_opposite_color(color_to_move)))
    throw new std::invalid_argument(
        "fen '" + fen + "' has a king that can be captured immediately");
}

std::string game::to_fen() const {
  auto piece_placement = notation::to_AN(board);
  auto active_color = notation::to_AN(color_to_move);
  auto castling_ability = notation::to_AN(castling);
  auto en_passant_target =
      (is_valid_square(en_passant) ? notation::to_AN(en_passant) : "-");
  auto halfmove_clk = halfmove_cnt;
  auto fullmove_no = fullmove;

  return piece_placement + " " + active_color + " " + castling_ability + " " +
         en_passant_target + " " + std::to_string(halfmove_clk) + " " +
         std::to_string(fullmove_no);
}

}  // namespace abra
