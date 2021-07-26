#include "display.h"

#include <algorithm>
#include <iostream>

using std::cout;

namespace abra {

void show_board(const game& g, color perspective) {
  auto board = g.get_board();
  auto fen = g.to_fen();
  if (perspective == color::black) board.rotate();
  cout << "\nBOARD: " << fen << std::endl << '\n';
  auto labels = (perspective == color::white ? std::string{"a b c d e f g h"}
                                             : std::string{"h g f e d c b a"});
  cout << "\n   " << labels;
  cout << "\n   ";
  for (int i = 0; i < 8; i++)
    cout << underline << "_" << reset_underline << " ";
  for (int i = 0; i < 64; i++) {
    int rank = (perspective == color::white ? 8 - i / 8 : 1 + i / 8);
    if (i % 8 == 0) cout << "\n" << rank << " ";
    cout << "|";
    cout << underline;
    auto x = board.get_piece(i);
    if (!x.is_empty()) {
      cout << (x.pcolor == color::black ? dark_color : light_color) << bold
           << notation::to_AN(x) << reset_bold << reset_color;
    } else {
      cout << ' ';
    }
    cout << reset_underline;
    if (i % 8 == 7) cout << "| " << rank;
  }
  cout << "\n   " << labels;
  cout << "\n" << std::endl;
}

void show_moves(const game& g) {
  auto moves = g.get_moves();
  std::cout << "moves: ";
  for (auto m : moves) {
    std::cout << notation::to_AN(m) << " ";
  }
  std::cout << "\n";
}

}  // namespace abra
