#ifndef ABRA_MOVEMENT_H
#define ABRA_MOVEMENT_H

#include <cassert>
#include <vector>

#include "types.h"

namespace abra::movement {

const square up = -8, down = +8, left = -1, right = +1;

// get direction of pawn movement for color
inline square get_pawn_direction(color _color) {
  assert(_color != color::none);
  return (_color == color::white) ? up : down;
}

const auto king = std::vector<square>{
    up, left, down, right, up + left, up + right, down + right, down + left};

const auto knight = std::vector<square>{
    up + up + left,      up + up + right,    down + down + left,
    down + down + right, up + right + right, down + right + right,
    up + left + left,    down + left + left};

const auto orthogonal_dir = std::vector<square>{up, down, left, right};

const auto diagonal_dir =
    std::vector<square>{up + left, up + right, down + right, down + left};

}  // namespace abra::movement

#endif
