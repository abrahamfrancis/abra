#ifndef ABRA_DISPLAY_H
#define ABRA_DISPLAY_H

#include "game.h"
#include "notation.h"

namespace abra {

// ANSI escape sequences for printing in the console
const std::string underline{"\033[4m"};
const std::string reset_underline{"\033[24m"};
const std::string reset_color{"\033[39m"};
const std::string dark_color{"\033[94m"};
const std::string light_color{"\033[93m"};
const std::string bold{"\033[1m"};
const std::string reset_bold{"\033[21m"};

// display the board in a pretty format
void show_board(const abra::game&, color);

// display list of legal moves
void show_moves(const abra::game&);

}  // namespace abra

#endif
