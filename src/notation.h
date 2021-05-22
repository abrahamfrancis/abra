#ifndef ABRA_NOTATION_HPP
#define ABRA_NOTATION_HPP

#include <string>
#include <vector>

#include "types.h"

namespace abra::notation {

// convert square to alg notation (files in lowercase)
std::string to_AN(square);

// convert color to alg notation
std::string to_AN(color);

// convert piece to alg notation
std::string to_AN(piece);

// convert move to long alg notation
std::string to_AN(move);

// convert castling rights to alg notation
std::string to_AN(castle_rights);

// convert array of pieces to fen piece placement
std::string to_AN(const board64 &);

// convert alg notation to square
square to_square(const std::string &);

// convert alg notation to color
color to_color(const std::string &);

// convert alg notation to piece
piece to_piece(const std::string &);

// convert long alg notation to move
move to_move(const std::string &);

// convert alg notation to castle_rights
castle_rights to_castle_rights(const std::string &);

// convert fen piece placement to array of pieces
board64 to_chess_board(const std::vector<std::string> &);

}  // namespace abra::notation

#endif
