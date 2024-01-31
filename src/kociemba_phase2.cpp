#include "kociemba_phase2.hpp"
#include <vector>

namespace rubik {

  sym_table<corners_perm_sym> corners_perm_sym_table;
  move_table<edges_perm_move> edges_perm_move_table;
  move_table<udslice_perm_move> udslice_perm_move_table;
  pruning_table<udslice_perm_pruning> udslice_perm_pruning_table;
  pruning_table<kociemba_phase2_pruning> kociemba_phase2_pruning_table;

  struct kociemba_phase2_search : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c1, c2, c3;
    
    u8 pruning_value(u8) const {
      auto sym_which = corners_perm_sym_table.raw_sym_which[corners_perm_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = edges_perm_move_table.sym_conj
          (c2, sym_inv[sym]);
        auto v = kociemba_phase2_pruning_table.distance[(c1/8) * 40'320 + c2_];
        return max(v, udslice_perm_pruning_table.distance[c3]);
      }
      runtime_assert(false);
    }
    
    bool allow_move(i32 m) const { 
      i32 axis = move_axis(m), dir = m%3;
      return axis == 0 || dir == 1;
    }
    void move(i32 m) {
      c1 = corners_perm_sym_table.move_sym(c1, m);
      c2 = edges_perm_move_table.move(c2, m);
      c3 = udslice_perm_move_table.move(c3, m);
    }
  };
 
  kociemba_phase2_search make_kociemba_phase2_search(corners const& c1, edges1 const& c2) {
    auto h1 = corners_perm_index(c1);
    return {
      .c1 = corners_perm_sym_table.raw_to_sym[h1] * 8 + __builtin_ctz(corners_perm_sym_table.raw_sym_which[h1]),
      .c2 = edges_perm_index(c2),
      .c3 = udslice_perm_index(c2),
    };
  }

  void solve_kociemba_phase2(move_seq& S, rubik& c) {
    auto N = c.N;
    runtime_assert(N&1);
    auto R = search1<kociemba_phase2_search>(make_kociemba_phase2_search(c.corners, c.edges1));
    for(auto m : R) {
      i32 axis = corners::move_axis(m), index = corners::move_real_index(N,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }

  void solve_kociemba_phase2_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 3;
    vector<tuple<u32, kociemba_phase2_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_kociemba_phase2_search(c.corners, c.edges1));
    }
    auto R = search_beam(cstates, width);
    vector<tuple<u32,u32,move_seq,rubik>> nstates;
    for(auto [idx, M] : R) {
      auto [data,len,S,c] = states[idx];
      for(auto m : M) {
        i32 axis = corners::move_axis(m), index = corners::move_real_index(N,m), dir = 1+m%3;
        S.move(axis, index, dir);
        c.move(axis, index, dir);
      }
      nstates.eb(data,len,S,c);
    }
    states = nstates;
    rng.shuffle(states);
  }

}
