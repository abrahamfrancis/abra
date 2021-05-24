#ifndef ABRA_MOVEMENT_H
#define ABRA_MOVEMENT_H

#include <cassert>

#include "types.h"

namespace abra::movement {

const square up = -8, down = +8, left = -1, right = +1;

// get direction of pawn movement for color
inline square get_pawn_direction(color c) {
  assert(c != color::none);
  return (c == color::white) ? up : down;
}

}  // namespace abra::movement

#endif
