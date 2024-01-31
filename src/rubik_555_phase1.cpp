#include "rubik_555_phase1.hpp"

namespace rubik {
  sym_table<centers2_phase1_555_sym> centers2_phase1_555_sym_table;
  move_table<centers3_phase1_555_move> centers3_phase1_555_move_table;

  pruning_table<centers2_phase1_555_pruning> centers2_phase1_555_pruning_table;
  pruning_table<centers3_phase1_555_pruning> centers3_phase1_555_pruning_table;

  back_table<centers_phase1_555_back> centers_phase1_555_back_table;

  struct phase1_555_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c1, c2;
    
    u8 pruning_value(u8 l) const {
      if(l <= centers_phase1_555_back::DEPTH) {
        u64 k = centers_phase1_555_back { .c1 = c1, .c2 = c2 }.key(); 
        if(u8 v = centers_phase1_555_back_table.find_distance(k); v != 255) {
          return v;
        }
        return 255;
      }

      return max(centers2_phase1_555_pruning_table.distance[c1/8],
                 centers3_phase1_555_pruning_table.distance[c2]);
    }
    
    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      c1 = centers2_phase1_555_sym_table.move_sym(c1, m);
      c2 = centers3_phase1_555_move_table.move(c2, m);
    }
  };

  phase1_555_search make_phase1_555_search(centers2 const& c1, centers3 const& c2) {
    u32 h1 = centers2_phase1_555_index(c1);
    u32 h2 = centers3_phase1_555_index(c2);
    return {
      .c1 = centers2_phase1_555_sym_table.raw_to_sym_full(h1),
      .c2 = h2,
    };
  }

  void solve_phase1_555(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase1_555_search>(make_phase1_555_search(c.centers2[x], c.centers3[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
  
  void solve_phase1_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<tuple<u32, phase1_555_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase1_555_search(c.centers2[0], c.centers3[0]));
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
