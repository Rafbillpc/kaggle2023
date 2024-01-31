#include "rubik_444_phase2.hpp"
#include "rubik_444_phase3.hpp"

namespace rubik {
  sym_table<centers3_phase2_444_sym> centers3_phase2_444_sym_table;

  pruning_table<centers3_phase2_444_pruning> centers3_phase2_444_pruning_table;

  struct phase2_444_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32    c1;
    edges2 c2;

    u8 pruning_value(u8) const {
      u8 v = centers3_phase2_444_pruning_table.distance[c1/8];
      if(v == 0) {
        return c2.is_split_lowhigh() && edges2_phase3_444_pruning_table.distance[edges2_phase3_444_index(c2)] != 255 ? 0 : 1;
      }
      return v;
    }
    
    bool allow_move(i32 m) const { 
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 0 && is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers3_phase2_444_sym_table.move_sym(c1, m);
      c2.move(m);
    }
  };

  struct phase2_444_edges_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32    c1;
    edges2 c2;

    u8 pruning_value(u8) const {
      return c2.is_split_lowhigh() && edges2_phase3_444_pruning_table.distance[edges2_phase3_444_index(c2)] != 255 ? 0 : 1;
    }
    
    bool allow_move(i32 m) const {
      return 1;
    }

    void move(i32 m) {
      c2.move(m);
    }
  };


  phase2_444_search make_phase2_444_search(centers3 const& c1, edges2 const& c2) {
    u32 h1 = centers3_phase2_444_index(c1);
    return {
      .c1 = centers3_phase2_444_sym_table.raw_to_sym_full(h1),
      .c2 = c2,
    };
  }
  
  void solve_phase2_444(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase2_444_search>(make_phase2_444_search(c.centers3[x], c.edges2[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }

  void solve_phase2_444_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 4;
    vector<tuple<u32, phase2_444_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase2_444_search(c.centers3[0], c.edges2[0]));
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

  phase2_444_edges_search make_phase2_444_edges_search(centers3 const& c1, edges2 const& c2) {
    return {
      .c2 = c2,
    };
  }
  
  void solve_phase2_444_edges(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase2_444_edges_search>(make_phase2_444_edges_search(c.centers3[x], c.edges2[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }

  void solve_phase2_444_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 4;
    vector<tuple<u32, phase2_444_edges_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase2_444_edges_search(c.centers3[0], c.edges2[0]));
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
