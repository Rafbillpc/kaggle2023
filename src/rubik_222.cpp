#include "rubik_222.hpp"

namespace rubik {

  sym_table<corners_state_sym> corners_sym_table;
  pruning_table<corners_state_pruning> corners_pruning_table;

  struct search_corners_state : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c;
    
    u8 pruning_value(u8) const {
      return corners_pruning_table.distance[c/8];
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) { c = corners_sym_table.move_sym(c, m); }
  };

  vector<i32> search_corners(corners &c) {
    return search1<search_corners_state>({
        .c = corners_sym_table.raw_to_sym_full(corners_full_index(c))
      });
  }

  void solve_corners(move_seq& S, rubik &c) {
    i32 N = c.N;
    auto R = search_corners(c.corners);
    for(auto m : R) {
      i32 axis = corners::move_axis(m), index = corners::move_real_index(N,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }

}
