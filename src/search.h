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

using state = std::pair<game, int>;

class zobrist_hash {
  uint64_t pieces[64][6][2];
  uint64_t colors[2];
  uint64_t castlings[4];
  uint64_t ep_files[8];
  uint64_t depths[8];

 public:
  zobrist_hash();
  size_t operator()(const state &p) const;
};

class minimax_search : public strategy {
  std::mt19937 rng;
  std::unordered_map<state, move, zobrist_hash> cache;
  size_t max_cache_size;

 public:
  minimax_search(size_t = size_t(1e7));
  std::pair<int, move> choose_move(const game &g, int) override;
  std::pair<int, move> minimax(const game &, int depth, int alpha, int beta);
};

}  // namespace abra

#endif
