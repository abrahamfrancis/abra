#include <random>

#include "search.h"

namespace abra {

zobrist_hash::zobrist_hash() {
  std::random_device rd;
  std::mt19937_64 rng(rd());
  std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 6; j++) {
      for (int k = 0; k < 2; k++) pieces[i][j][k] = dist(rng);
    }
  }
  for (int k = 0; k < 2; k++) colors[k] = dist(rng);
  for (int k = 0; k < 4; k++) castlings[k] = dist(rng);
  for (int k = 0; k < 8; k++) ep_files[k] = dist(rng);
  for (int k = 0; k < 6; k++) depths[k] = dist(rng);
}

size_t zobrist_hash::operator()(const state& st) const {
	auto g = st.first;
	auto depth = st.second;
  auto hash = depths[depth];
  auto board = g.get_board();
  for (int i = 0; i < 64; i++) {
    auto p = board.get_piece(i);
    int c = static_cast<int>(p.pcolor) - 1, pt = static_cast<int>(p.ptype) - 1;
    hash ^= pieces[i][pt][c];
  }
  int c = static_cast<int>(g.get_color_to_move()) - 1;
  hash ^= colors[c];
  auto castle = g.get_castle_rights();
  if (castle.white_short) hash ^= castlings[0];
  if (castle.white_long) hash ^= castlings[1];
  if (castle.black_short) hash ^= castlings[2];
  if (castle.black_long) hash ^= castlings[3];
  auto ep_sq = g.get_en_passant_sq();
  int file = get_col(ep_sq) % 8;
  hash ^= ep_files[file];
  return hash;
}

}  // namespace abra
