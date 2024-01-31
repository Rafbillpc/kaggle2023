#include "rubik_555_phase2.hpp"
#include "header.hpp"

namespace rubik {
  sym_table<centers2_phase2_555_sym> centers2_phase2_555_sym_table;
  move_table<centers3_phase2_555_move> centers3_phase2_555_move_table;

  pruning_table<centers_phase2_555_pruning> centers_phase2_555_pruning_table;

  struct phase2_555_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;
    
    u32 c1, c2;
    edges2 e2;
    
    u8 pruning_value(u8) const {
      auto sym_which = centers2_phase2_555_sym_table.raw_sym_which[centers2_phase2_555_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = centers3_phase2_555_move_table.sym_conj
          (c2, sym_inv[sym]);
        auto v = centers_phase2_555_pruning_table.distance[(c1/8) * 12'870 + c2_];
        if(v == 0) {
          if(e2.parity()) return 1;
        }
        return v;
      }
      impossible();
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 0 && is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers2_phase2_555_sym_table.move_sym(c1, m);
      c2 = centers3_phase2_555_move_table.move(c2, m);
      e2.move(m);
    }
  };

  struct phase2_555_edges_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;
    
    edges2 e2;
    
    u8 pruning_value(u8) const {
      if(e2.parity()) return 1;
      return 0;
    }

    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      e2.move(m);
    }
  };

  phase2_555_search make_phase2_555_search(centers2 const& c1, centers3 const& c2, edges2 const& c3) {
    u32 h1 = centers2_phase2_555_index(c1);
    u32 h2 = centers3_phase2_555_index(c2);
    return {
        .c1 = centers2_phase2_555_sym_table.raw_to_sym_full(h1),
        .c2 = h2,
        .e2 = c3,
      };
  }

  void solve_phase2_555(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase2_555_search>(make_phase2_555_search(c.centers2[x], c.centers3[x], c.edges2[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
 
  void solve_phase2_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<tuple<u32, phase2_555_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase2_555_search(c.centers2[0], c.centers3[0], c.edges2[0]));
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
 
   phase2_555_edges_search make_phase2_555_edges_search(centers2 const& c1, centers3 const& c2, edges2 const& c3) {
    return {
      .e2 = c3,
    };
  }

  void solve_phase2_555_edges(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase2_555_edges_search>(make_phase2_555_edges_search(c.centers2[x], c.centers3[x], c.edges2[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
 
  void solve_phase2_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<tuple<u32, phase2_555_edges_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase2_555_edges_search(c.centers2[0], c.centers3[0], c.edges2[0]));
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
