#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>

#include "display.h"
#include "game.h"
#include "notation.h"
#include "search.h"
#include "types.h"

using namespace abra;
using std::cin;
using std::cout;

struct bot_config {
  abra::color their_color;
  int max_search_time_ms;
  std::string position;
  std::string policy;
};

// the game loop
void play_game(strategy& strat, bot_config& config) {
  using namespace std::chrono;

  auto us = get_opposite_color(config.their_color);

  game g = (config.position.empty() ? game{} : game{config.position});

  show_board(g);
  show_moves(g);
  while (!g.is_terminal()) {
    if (g.get_color_to_move() == us) {
      auto begin = steady_clock::now();
      auto [sc, _move] = strat.choose_move(g, config.max_search_time_ms);
      auto end = steady_clock::now();
      cout << "BOT: " << notation::to_AN(_move) << "  " << (sc > 0 ? "+" : "")
           << sc << " ("
           << duration_cast<std::chrono::milliseconds>(end - begin).count()
           << "ms)\n";
      g.make_move(_move);
    } else {
      std::string their_move;
      cout << "move: ";
      cin >> their_move;
      if (their_move == "resign") {
        cout << "you resigned\n";
        return;
      }
      auto moves = g.get_moves();
      try {
        auto mv = notation::to_move(their_move);
        if (std::find(moves.begin(), moves.end(), mv) == moves.end()) {
          cout << "illegal!" << std::endl;
          continue;
        }
        g.make_move(mv);
      } catch (std::invalid_argument* err) {
        cout << "ERROR: " << err->what() << std::endl;
        continue;
      }
    }
    show_board(g);
    show_moves(g);
  }
  auto result = g.get_result();
  if (result == us) {
    cout << "bot won \n";
  } else if (result != color::none) {
    cout << "you won!\n";
  } else {
    cout << "draw\n";
  }
}

int main(int argc, const char* argv[]) {
  try {
    auto config = bot_config{color::white, 30000, "", ""};
    // parse fen
    for (int i = 1; i < argc; i += 2) {
      auto flg = std::string{argv[i]};
      if (i + 1 >= argc)
        throw new std::invalid_argument(flg + " does not have a value");
      auto val = std::string{argv[i + 1]};
      if (flg == "--color") {
        if (val == "black")
          config.their_color = color::black;
        else if (val == "white")
          config.their_color = color::white;
        else
          throw new std::invalid_argument("invalid color " + val);
      } else if (flg == "--think") {
        config.max_search_time_ms = std::stoi(val);
      } else if (flg == "--fen") {
        config.position = std::string{val};
      } else if (flg == "--strategy") {
        if (val == "minimax")
          config.policy = "minimax";
        else
          throw new std::invalid_argument("invalid strategy " + val);
      } else {
        throw new std::invalid_argument("invalid arguement " + flg);
      }
    }
    if (config.policy.empty()) {
      auto strat = strategy{};
      play_game(strat, config);
    } else {
      auto strat = minimax_search{};
      play_game(strat, config);
    }

  } catch (std::invalid_argument* err) {
    cout << "ERROR: " << err->what() << "\n";
  }
  return 0;
}
