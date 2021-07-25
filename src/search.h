#ifndef ABRA_SEARCH_H
#define ABRA_SEARCH_H

#include <algorithm>
#include <chrono>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#include "game.h"
#include "types.h"

using std::chrono::steady_clock;

namespace abra {

// strategy class with a virtual method for choosing move
class strategy {
 public:
  // return the chosen move along with an estimate of the score
  // by default, this just returns the first move and a score 0
  virtual std::pair<int, move> choose_move(const game &g, int) {
    auto moves = g.get_moves();
    return {-1, moves[0]};
  };
};

class minimax_search : public strategy {
  std::mt19937 rng;

 public:
  minimax_search();
  std::pair<int, move> choose_move(const game &g, int) override;
  std::pair<int, move> minimax(const game &, int depth, int alpha, int beta);
};

}  // namespace abra

#endif
