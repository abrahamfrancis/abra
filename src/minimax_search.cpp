#include <iostream>

#include "search.h"

namespace abra {

const int inf = int(1e5);

minimax_search::minimax_search(size_t cache_size) {
  max_cache_size = cache_size;
}

// TODO: return prematurely if time crosses max_time
std::pair<int, move> minimax_search::choose_move(const game &g, int max_time) {
  // return minimax(g, 6, -inf, inf);
  auto guess = 0;
  auto max_depth = 6;
  for (int d = 1; d <= max_depth; d++) {
    guess = mtdf(g, d, guess);
  }
  auto st = std::make_pair(g, max_depth - 1);
  assert(cache.find(st) != cache.end());  // move should be present in cache
  return {guess, cache[st].m};
}

// clang-format off
// Piece Square Table: taken from https://www.chessprogramming.org/Simplified_Evaluation_Function
// modify to change evaluation function & bot behaviour
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

// Implement MTD(f), referred to from https://www.chessprogramming.org/MTD(f)

int minimax_search::mtdf(const game &g, int depth, int f) {
  auto guess = f;
  auto upper = inf;
  auto lower = -inf;
  auto beta = 0;
  while (lower < upper) {
    if (guess == lower) {
      beta = guess + 1;
    } else {
      beta = guess;
    }
    guess = minimax(g, depth, beta - 1, beta);
    if (guess < beta) {
      upper = guess;
    } else {
      lower = guess;
    }
  }
  minimax(g, depth, beta - 1, beta); // to ensure presence in cache
  return guess;
}

int minimax_search::minimax(const game &g, int depth, int alpha, int beta) {
  using std::max;
  using std::min;

  if (depth <= 0 || g.is_terminal()) return score(g);

  auto moves = g.get_moves();
  if (moves.empty()) return score(g);

  auto st = std::make_pair(g, depth - 1);

  if (cache.find(st) != cache.end()) {
    auto &n = cache[st];
    if (n.lb >= beta)
      return n.lb;
    if (n.ub <= alpha)
      return n.ub;
    alpha = max(alpha, n.lb);
    beta = min(beta, n.ub);
  } else {
    if (cache.size() >= max_cache_size) cache.clear();
    cache[st] = node_info{move{}, -inf, inf};
  }

  auto &node = cache[st];

  auto guess = 0;

  auto best_move = moves[0];

  if (depth == 0) {
    guess = score(g);
  } else if (g.get_color_to_move() == color::white) {  // Maximize
    guess = -inf;
    auto alpha_new = alpha;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto x = minimax(new_game, depth - 1, alpha_new, beta);
      if (x > guess) {
        guess = x;
        best_move = m;
      }
      if (guess >= beta) break;
      alpha_new = max(alpha_new, guess);
    }
  } else {  // Minimize
    guess = inf;
    auto beta_new = beta;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto x = minimax(new_game, depth - 1, alpha, beta_new);
      if (x < guess) {
        guess = x;
        best_move = m;
      }
      if (guess <= alpha) break;
      beta_new = min(beta_new, guess);
    }
  }
  node.m = best_move;
  if (guess <= alpha) {
    node.ub = guess;
  } else if (guess < beta) {
    node.lb = node.ub = guess;
  } else {
    node.lb = guess;
  }
  return guess;
}

}  // namespace abra
