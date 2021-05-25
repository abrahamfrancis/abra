#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "game.h"
#include "notation.h"
#include "types.h"

#define underline "\033[4m"
#define reset_underline "\033[24m"
#define reset_color "\033[39m"
#define dark_color "\033[94m"
#define light_color "\033[93m"
#define bold "\033[1m"
#define reset_bold "\033[21m"

constexpr abra::color us = abra::color::white;
constexpr int inf = int(1e7);

using namespace abra;

static auto _split_string(const std::string& str, char delim) {
  auto tokens = std::vector<std::string>{};
  auto token = std::string{};
  auto str_stream = std::istringstream{str};
  while (std::getline(str_stream, token, delim)) tokens.push_back(token);
  return tokens;
}

void show_board(game& g) {
  auto fen = g.to_fen();
  auto pieces = _split_string(fen, ' ')[0];
  auto board = notation::to_chess_board(_split_string(pieces, '/'));
  using std::cout;
  cout << "\nBOARD: " << fen << std::endl;
  cout << '\n';
  cout << "\n   a b c d e f g h";
  cout << "\n   ";
  for (int i = 0; i < 8; i++)
    cout << underline << "_" << reset_underline << " ";
  for (int i = 0; i < 64; i++) {
    if (i % 8 == 0) {
      cout << "\n" << (8 - i / 8) << " ";
    }
    cout << "|";
    cout << underline;
    auto x = board.get_piece(i);
    if (!x.is_empty()) {
      if (x.pcolor == color::black)
        cout << dark_color;
      else
        cout << light_color;
      cout << bold << notation::to_AN(x) << reset_bold << reset_color;
    } else {
      cout << ' ';
    }
    cout << reset_underline;
    if (i % 8 == 7) cout << "| " << (8 - i / 8);
  }
  cout << "\n   a b c d e f g h";
  cout << "\n" << std::endl;
}

int get_score(piece p) {
  if (p.is_empty()) return 0;
  int score;
  switch (p.ptype) {
    case piece_type::pawn:
      score = 100;
      break;
    case piece_type::knight:
      score = 300;
      break;
    case piece_type::bishop:
      score = 320;
      break;
    case piece_type::rook:
      score = 500;
      break;
    case piece_type::queen:
      score = 900;
      break;
    case piece_type::king:
      score = inf;
      break;
    default:
      assert(false);
  }
  if (p.pcolor == color::black) score = -score;
  return score;
}

// alpha beta search

int score(const game& g) {
  if (g.is_terminal()) {
    auto w = g.get_result();
    if (w == color::none) return 0;
    return w == color::white ? inf : -inf;
  }
  int s = 0;
  for (auto i = 0; i < 64; i++) {
    auto p = g.piece_at(i);
    s += get_score(p);
  }
  return s;
}

auto minimax(const game& g, int alpha, int beta, int depth) {
  using std::max;
  using std::min;
  static std::random_device rd;
  static std::mt19937 gen(rd());
  if (g.is_terminal() || depth <= 0) {
    return std::make_tuple(score(g), move{});
  }
  auto moves = g.get_moves();
  if (moves.empty()) return std::make_tuple(score(g), move{});
  depth--;
  std::shuffle(moves.begin(), moves.end(), gen);
  auto best_move = moves[0];
  int score = 0;

  if (g.get_color_to_move() == color::white) {
    // MAXIMIZE
    score = -inf;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto [x, mv] = minimax(new_game, alpha, beta, depth);
      if (x > score) {
        score = x;
        best_move = m;
      }
      if (score >= beta) break;
      alpha = max(alpha, score);
    }
  } else {
    // MINIMIZE
    score = inf;
    for (auto m : moves) {
      game new_game{g};
      new_game.make_move(m);
      auto [x, mv] = minimax(new_game, alpha, beta, depth);
      if (x < score) {
        score = x;
        best_move = m;
      }
      if (score <= alpha) break;
      beta = min(beta, score);
    }
  }
  if (score >= inf / 10)
    score--;
  else if (score <= -inf / 10)
    score++;
  return std::make_tuple(score, best_move);
}

void show_moves(const game& g) {
  auto moves = g.get_moves();
  std::cout << "moves: ";
  for (auto m : moves) {
    std::cout << notation::to_AN(m) << " ";
  }
  std::cout << "\n";
}

auto choose_move(const game& g) {
  int depth = 6;
  std::cout << "searching: (depth=" << depth << ")\n";
  return minimax(g, -inf, inf, depth);
}

int main(int argc, const char* argv[]) {
  try {
    game g;
    if (argc > 1) {
      auto fen = std::string{argv[1]};
      std::cout << "LOADING FROM: " << fen << std::endl;
      g = game{fen};
    }
    show_board(g);
    show_moves(g);
    while (!g.is_terminal()) {
      if (g.get_color_to_move() == us) {
        auto begin = std::chrono::steady_clock::now();
        auto [sc, _move] = choose_move(g);
        auto end = std::chrono::steady_clock::now();
        std::cout << "BOT: " << notation::to_AN(_move) << "  "
                  << (sc > 0 ? "+" : "") << sc << " ("
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         end - begin)
                         .count()
                  << "ms)\n";
        g.make_move(_move);
      } else {
        std::string their_move;
        std::cout << "move: ";
        std::cin >> their_move;
        if (their_move == "resign") {
          std::cout << "you resigned\n";
          return 0;
        }
        auto mv = notation::to_move(their_move);
        auto moves = g.get_moves();
        if (std::find(moves.begin(), moves.end(), mv) == moves.end()) {
          std::cout << "illegal!" << std::endl;
          continue;
        }
        g.make_move(mv);
      }
      show_board(g);
      show_moves(g);
    }
    auto result = g.get_result();
    if (result == us) {
      std::cout << "random bot won \n";
    } else if (result != color::none) {
      std::cout << "you won!\n";
    } else {
      std::cout << "draw\n";
    }
  } catch (std::invalid_argument* err) {
    std::cout << "ERROR: " << err->what() << "\n";
  }
  return 0;
}
