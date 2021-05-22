#include "notation.h"

#include <cassert>
#include <cctype>
#include <stdexcept>

namespace abra::notation {

std::string to_AN(square sq) {
  assert(is_valid_square(sq));
  int r = get_row(sq), c = get_col(sq);
  int rank = 8 - r;
  char file = 'a' + c % 8;
  return file + std::to_string(rank);
}

std::string to_AN(color c) {
  assert(c != color::none);
  return (c == color::white ? "w" : "b");
}

std::string to_AN(piece p) {
  assert(!p.is_empty());
  char symbol;
  switch (p.ptype) {
    case piece_type::pawn:
      symbol = 'P';
      break;
    case piece_type::knight:
      symbol = 'N';
      break;
    case piece_type::bishop:
      symbol = 'B';
      break;
    case piece_type::rook:
      symbol = 'R';
      break;
    case piece_type::queen:
      symbol = 'Q';
      break;
    case piece_type::king:
      symbol = 'K';
      break;
    default:
      assert(false);  // shouldn't reach here
  }
  if (p.pcolor == color::black) symbol = std::tolower(symbol);
  return std::string{symbol};
}

std::string to_AN(move m) {
  auto an = to_AN(m.from) + to_AN(m.to);
  if (!m.promotion.is_empty()) an += to_AN(m.promotion);
  return an;
}

std::string to_AN(castle_rights r) {
  auto an = std::string{""};
  if (r.white_short) an += "K";
  if (r.white_long) an += "Q";
  if (r.black_short) an += "k";
  if (r.black_long) an += "q";
  if (an.empty()) an = "-";
  return an;
}

std::string to_AN(const board64 &board) {
  auto an = std::string{""};
  int empty = 0;
  for (square i = 0; i < 64; i++) {
    auto x = board.get_piece(i);
    if (i % 8 == 0) {        // new row
      if (i > 0) an += "/";  // add seperator if not first row
      empty = 0;
    }
    if (x.is_empty()) {
      empty++;
    } else {
      if (empty > 0) {
        an += std::to_string(empty);
        empty = 0;
      }
      an += to_AN(x);
    }
    if (i % 8 == 7 && empty > 0)  // if last column, add spaces
      an += std::to_string(empty);
  }
  return an;
}

square to_square(const std::string &an) {
  if (an.size() != 2)
    throw new std::invalid_argument("square '" + an +
                                    "' should have exactly 2 characters");
  char file = std::tolower(an[0]), rank = an[1];
  if (file < 'a' || file > 'h')
    throw new std::invalid_argument("square '" + an + "' has invalid file '" +
                                    file + "'");
  if (rank < '1' || rank > '8')
    throw new std::invalid_argument("square '" + an + "' has invalid rank '" +
                                    std::to_string(rank) + "'");
  int col = file - 'a', row = 8 - (rank - '0');
  return square{8 * row + col};
}

color to_color(const std::string &an) {
  if (an.size() != 1)
    throw new std::invalid_argument("color '" + an +
                                    "' should have exactly 1 character");
  switch (std::tolower(an[0])) {
    case 'w':
      return color::white;
    case 'b':
      return color::black;
    default:
      throw new std::invalid_argument("color '" + an + "' is not valid");
  }
}

piece to_piece(const std::string &an) {
  if (an.size() != 1)
    throw new std::invalid_argument("piece '" + an +
                                    "' should have exactly 1 character");
  char symbol = an[0];
  auto ptype = piece_type::empty;
  switch (std::toupper(symbol)) {
    case 'P':
      ptype = piece_type::pawn;
      break;
    case 'N':
      ptype = piece_type::knight;
      break;
    case 'B':
      ptype = piece_type::bishop;
      break;
    case 'R':
      ptype = piece_type::rook;
      break;
    case 'Q':
      ptype = piece_type::queen;
      break;
    case 'K':
      ptype = piece_type::king;
      break;
    default:
      throw new std::invalid_argument("piece '" + an + "' is not valid");
  }
  auto pcolor = std::isupper(symbol) ? color::white : color::black;
  return piece{pcolor, ptype};
}

move to_move(const std::string &an) {
  if (an.size() != 4 && an.size() != 53)
    throw new std::invalid_argument("move '" + an +
                                    "' should have exactly 4 or 5 characters");
  auto promote = (an.size() == 4 ? piece{} : to_piece(std::string{an[4]}));
  auto from_sq = an.substr(0, 2), to_sq = an.substr(2, 2);
  return move{to_square(from_sq), to_square(to_sq), promote};
}

castle_rights to_castle_rights(const std::string &an) {
  if (an.empty() || an.size() > 4)
    throw new std::invalid_argument("castle rights '" + an +
                                    "' cannot have more than 4 characters");
  auto r = castle_rights{};
  if (an == "-") return r;
  // helper to set flag, and throw if already set
  auto set_flag = [&an](bool &flag, char c) {
    if (flag)
      throw new std::invalid_argument(
          "castle rights '" + an + "' cannot have more than one '" + c + "'");
    flag = true;
  };
  for (auto c : an) {
    switch (c) {
      case 'K':
        set_flag(r.white_short, c);
        break;
      case 'Q':
        set_flag(r.white_long, c);
        break;
      case 'k':
        set_flag(r.black_short, c);
        break;
      case 'q':
        set_flag(r.black_long, c);
        break;
      default:
        throw new std::invalid_argument("castle rights '" + an +
                                        "' has invalid character '" + c + "'");
    }
  }
  return r;
}

board64 to_chess_board(const std::vector<std::string> &rows) {
  assert(rows.size() == 8);
  auto board = board64{};
  int r = 0, c;
  for (int i = 0; i < 8; i++) {
    auto &row = rows[i];
    c = 0;
    for (auto j = 0U; j < row.size(); j++) {
      auto symbol = row[j];
      if (std::isdigit(symbol)) {
        if (j > 0 && std::isdigit(row[j - 1]))
          throw new std::invalid_argument("row " + std::to_string(i + 1) +
                                          " '" + row +
                                          "' in fen has consecutive digits");
        c += (symbol - '0');
      } else {
        auto x = notation::to_piece(std::string{symbol});
        auto p = 8 * r + c;
        board.set_piece(p, x);
        c++;
      }
    }
    if (c != 8)
      throw new std::invalid_argument("row " + std::to_string(i + 1) + " '" +
                                      row + "' in fen should have 8 cols");
    r++;
  }
  return board;
}

}  // namespace abra::notation
