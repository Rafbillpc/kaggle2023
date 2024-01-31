#pragma once
#include "puzzle_info.hpp"
#include "rubik.hpp"

namespace rubik {
  move_seq solve(i32 typ, i32 num_wildcards, vector<i32> const& tgt, rubik const& c);
}
