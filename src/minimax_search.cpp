#include "search.h"

#include <iostream>

namespace abra {

const int inf = int(1e5);

minimax_search::minimax_search() {
  std::random_device rd;
  rng = std::mt19937(rd());  // initialize rng
}

std::pair<int, move> minimax_search::choose_move(const game &g, int max_time) {
  return minimax(g, 6, -inf, inf);
}

// return a score indicating how good this position is
int score(const game &g) {
  auto board = g.get_board();
	auto sc = 0;
	sc += popcount(board.white & board.pawn) * 100;
	sc += popcount(board.white & board.knight) * 300;
	sc += popcount(board.white & board.bishop) * 300;
	sc += popcount(board.white & board.rook) * 500;
	sc += popcount(board.white & board.queen) * 900;
	sc += popcount(board.white & board.king) * inf;

	sc -= popcount(board.black & board.pawn) * 100;
	sc -= popcount(board.black & board.knight) * 300;
	sc -= popcount(board.black & board.bishop) * 300;
	sc -= popcount(board.black & board.rook) * 500;
	sc -= popcount(board.black & board.queen) * 900;
	sc -= popcount(board.black & board.king) * inf;
	return sc;
}

std::pair<int, move> minimax_search::minimax(const game &g, int depth, int alpha, int beta) {
  if (g.is_terminal() || depth <= 0)
    return {score(g), move{}};
  auto moves = g.get_moves();
  if (moves.empty()) return {score(g), move{}};
  depth--;

  std::shuffle(moves.begin(), moves.end(), rng);
  auto best_move = moves[0];
  int score = 0;

  if (g.get_color_to_move() == color::white) {  // Maximize
    score = -inf;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto [x, mv] = minimax(new_game, depth, alpha, beta);
      if (x > score) {
        score = x;
        best_move = m;
      }
      if (score >= beta) break;
      alpha = std::max(alpha, score);
    }
  } else {  // Minimize
    score = inf;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto [x, mv] = minimax(new_game, depth, alpha, beta);
      if (x < score) {
        score = x;
        best_move = m;
      }
      if (score <= alpha) break;
      beta = std::min(beta, score);
    }
  }
  return {score, best_move};
}

}  // namespace abra
