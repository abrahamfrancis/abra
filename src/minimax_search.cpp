#include <iostream>

#include "search.h"

namespace abra {

const int inf = int(1e5);

minimax_search::minimax_search(size_t cache_size) {
  std::random_device rd;
  rng = std::mt19937(rd());  // initialize rng
  // max_cache_size = cache_size;
  max_cache_size = int(1e8);  // ~ 1gb
}

std::pair<int, move> minimax_search::choose_move(const game &g, int max_time) {
  return minimax(g, 6, -inf, inf);
}

// clang-format off
// store pst: taken from https://www.chessprogramming.org/Simplified_Evaluation_Function
// modify to change evaluation function
const int pst[][64] = {
  // pawn
  {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
  },
  // knight
  {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
  },
  // bishop
  {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
  },
  // rook
  {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
  },
  // queen
  {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
  },
  // king - midgame
  {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20

  },
  // king - endgame
  {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
  }
};

// piece values for P N B R Q K
const int pv[] = { 100, 320, 330, 500, 900, 20000 };
// clang-format on

// return a score indicating how good this position is
int score(const game &g) {
  if (g.is_terminal()) {
    auto winner = g.get_result();
    if (winner == color::none) return 0;
    return (winner == color::white ? inf : -inf);
  }
  auto board = g.get_board();
  auto sc = 0;
  for (int i = 0; i < 64; i++) {
    auto p = board.get_piece(i);
    if (p.ptype == piece_type::empty) continue;
    auto j = static_cast<int>(p.ptype) - 1;  // subtract to make zero indexed
    if (p.pcolor == color::white) {
      sc += pst[j][i] + pv[j];
    } else {
      sc -= pst[j][63 - i] + pv[j];  // mirrored
    }
  }
  return sc;
}

std::pair<int, move> minimax_search::minimax(const game &g, int depth,
                                             int alpha, int beta) {
  if (depth <= 0 || g.is_terminal()) return {score(g), move{}};

  auto moves = g.get_moves();
  if (moves.empty()) return {score(g), move{}};

  auto st = std::make_pair(g, depth - 1);
  if (cache.find(st) != cache.end()) {
    auto mv = cache[st];
    auto ng = g;
    ng.make_move(mv);
    return {score(ng), mv};
  }
  if (cache.size() >= max_cache_size) cache.clear();

  std::shuffle(moves.begin(), moves.end(), rng);
  auto best_move = moves[0];
  int score = 0;

  if (g.get_color_to_move() == color::white) {  // Maximize
    score = -inf;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto [x, mv] = minimax(new_game, depth - 1, alpha, beta);
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
      auto [x, mv] = minimax(new_game, depth - 1, alpha, beta);
      if (x < score) {
        score = x;
        best_move = m;
      }
      if (score <= alpha) break;
      beta = std::min(beta, score);
    }
  }
  cache[st] = best_move;
  return {score, best_move};
}

}  // namespace abra
