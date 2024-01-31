#include "rubik_444_phase1.hpp"

namespace rubik {
  sym_table<centers3_phase1_444_sym> centers3_phase1_444_sym_table;

  pruning_table<centers3_phase1_444_pruning> centers3_phase1_444_pruning_table;

  struct phase1_444_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c1;
    
    u8 pruning_value(u8) const {
      return centers3_phase1_444_pruning_table.distance[c1/8];
    }
    
    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      c1 = centers3_phase1_444_sym_table.move_sym(c1, m);
    }
  };

  phase1_444_search make_phase1_444_search(centers3 const& c) {
    auto h1 = centers3_phase1_444_index(c);
    return {
      .c1 = centers3_phase1_444_sym_table.raw_to_sym_full(h1),
    };
  }

  vector<i32> search_phase1_444(centers3 const& c) {
    auto h1 = centers3_phase1_444_index(c);
    return search1<phase1_444_search>({
        .c1 = centers3_phase1_444_sym_table.raw_to_sym_full(h1),
      });
  }

  void solve_phase1_444(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase1_444_search>(make_phase1_444_search(c.centers3[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
  
  void solve_phase1_444_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 4;
    vector<tuple<u32, phase1_444_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase1_444_search(c.centers3[0]));
    }
    auto R = search_beam(cstates, width);
    vector<tuple<u32,u32,move_seq,rubik>> nstates;
    for(auto [idx, M] : R) {
      auto [data,len,S,c] = states[idx];
      for(auto m : M) {
        i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,0,m), dir = 1+m%3;
        S.move(axis, index, dir);
        c.move(axis, index, dir);
      }
      nstates.eb(data,len,S,c);
    }
    states = nstates;
    rng.shuffle(states);
  }
 
}
