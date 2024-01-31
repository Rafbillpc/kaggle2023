#include "rubik_555_phase5.hpp"
#include "header.hpp"

namespace rubik {
  move_table<centers2_phase5_555_move> centers2_phase5_555_move_table;
  move_table<centers3_phase5_555_move> centers3_phase5_555_move_table;
  move_table<edges_low_phase5_555_move> edges_low_phase5_555_move_table;
  move_table<edges_high_phase5_555_move> edges_high_phase5_555_move_table;

  pruning_table<centers2_phase5_555_pruning> centers2_phase5_555_pruning_table;
  pruning_table<centers3_phase5_555_pruning> centers3_phase5_555_pruning_table;
  pruning_table<edges_low_phase5_555_pruning> edges_low_phase5_555_pruning_table;
  pruning_table<edges_high_phase5_555_pruning> edges_high_phase5_555_pruning_table;

  back_table<phase5_555_back> phase5_555_back_table;
  back_table<centers_phase5_555_back> centers_phase5_555_back_table;
  back_table<edges_phase5_555_back> edges_phase5_555_back_table;
 
  struct phase5_555_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;
    
    u32 c1, c2, c3, c4;
    
    u8 pruning_value(u8 l) const {
      if(l <= phase5_555_back::DEPTH) {
        u64 k = phase5_555_back { .c1 = c1, .c2 = c2, .c3 = c3, .c4 = c4 }.key(); 
        if(u8 w = phase5_555_back_table.find_distance(k); w != 255) {
          return w;
        }else{
          return 255;
        }
      }
      
      u8 v = 0;
      v = max(v, centers2_phase5_555_pruning_table.distance[c1]);
      v = max(v, centers3_phase5_555_pruning_table.distance[c2]);
      v = max(v, edges_low_phase5_555_pruning_table.distance[c3]);
      v = max(v, edges_high_phase5_555_pruning_table.distance[c4]);
      
      if(l <= centers_phase5_555_back::DEPTH) {
        u64 k = centers_phase5_555_back { .c1 = c1, .c2 = c2 }.key(); 
        if(u8 w = centers_phase5_555_back_table.find_distance(k); w != 255) {
          v = max(v, w);
        }else{
          return 255;
        }
      }

      if(l <= edges_phase5_555_back::DEPTH) {
        u64 k = edges_phase5_555_back { .c3 = c3, .c4 = c4 }.key(); 
        if(u8 w = edges_phase5_555_back_table.find_distance(k); w != 255) {
          v = max(v, w);
        }else{
          return 255;
        }
      }

      return v;
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers2_phase5_555_move_table.move(c1, m);
      c2 = centers3_phase5_555_move_table.move(c2, m);
      c3 = edges_low_phase5_555_move_table.move(c3, m);
      c4 = edges_high_phase5_555_move_table.move(c4, m);
    }
  };

 
  struct phase5_555_edges_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;
    
    u32 c3, c4;
    
    u8 pruning_value(u8 l) const {
      u8 v = 0;
      v = max(v, edges_low_phase5_555_pruning_table.distance[c3]);
      v = max(v, edges_high_phase5_555_pruning_table.distance[c4]);

      if(l <= edges_phase5_555_back::DEPTH) {
        u64 k = edges_phase5_555_back { .c3 = c3, .c4 = c4 }.key(); 
        if(u8 w = edges_phase5_555_back_table.find_distance(k); w != 255) {
          v = max(v, w);
        }else{
          return 255;
        }
      }

      return v;
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c3 = edges_low_phase5_555_move_table.move(c3, m);
      c4 = edges_high_phase5_555_move_table.move(c4, m);
    }
  };

  phase5_555_search make_phase5_555_search(centers2 const& c1, centers3 const& c2, edges1 const& c3, edges2 const& c4) {
    u8 a[8];
    auto mask = c3.fb_colors();
    runtime_assert(__builtin_popcount(mask) == 4);
    i32 cnt = 0;
    FOR(i,12) if(!(mask&(1<<i))) {
      if(cnt < 8) {
        a[cnt++] = i;
      }
    }
    runtime_assert(cnt == 8);

    u32 h1 = centers2_phase5_555_index(c1);
    u32 h2 = centers3_phase5_555_index(c2);
    u32 h3 = edges_low_phase5_555_index(a, c3, c4);
    u32 h4 = edges_high_phase5_555_index(a, c3, c4);
    return {
      .c1 = h1,
      .c2 = h2,
      .c3 = h3,
      .c4 = h4,
    };
  }

  void solve_phase5_555(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    
    auto R = search1<phase5_555_search>(make_phase5_555_search(c.centers2[x], c.centers3[x], c.edges1, c.edges2[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
  
  void solve_phase5_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<tuple<u32, phase5_555_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase5_555_search(c.centers2[0], c.centers3[0], c.edges1, c.edges2[0]));
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


  phase5_555_edges_search make_phase5_555_edges_search(centers2 const& c1, centers3 const& c2, edges1 const& c3, edges2 const& c4) {
    u8 a[8];
    auto mask = c3.fb_colors();
    runtime_assert(__builtin_popcount(mask) == 4);
    i32 cnt = 0;
    FOR(i,12) if(!(mask&(1<<i))) {
      if(cnt < 8) {
        a[cnt++] = i;
      }
    }
    runtime_assert(cnt == 8);

    u32 h3 = edges_low_phase5_555_index(a, c3, c4);
    u32 h4 = edges_high_phase5_555_index(a, c3, c4);
    return {
      .c3 = h3,
      .c4 = h4,
    };
  }

  void solve_phase5_555_edges(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    
    auto R = search1<phase5_555_edges_search>(make_phase5_555_edges_search(c.centers2[x], c.centers3[x], c.edges1, c.edges2[x]));
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
  
  void solve_phase5_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<tuple<u32, phase5_555_edges_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_phase5_555_edges_search(c.centers2[0], c.centers3[0], c.edges1, c.edges2[0]));
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
