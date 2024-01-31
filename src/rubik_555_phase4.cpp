#include "rubik_555_phase4.hpp"
#include "header.hpp"
#include "rubik.hpp"

namespace rubik {
  move_table<centers2_phase4_555_move> centers2_phase4_555_move_table;
  move_table<centers3_phase4_555_move> centers3_phase4_555_move_table;
  move_table<edges_low_phase4_555_move> edges_low_phase4_555_move_table;
  move_table<edges_high_phase4_555_move> edges_high_phase4_555_move_table;

  pruning_table<centers_phase4_555_pruning> centers_phase4_555_pruning_table;
  pruning_table<edges_low_phase4_555_pruning> edges_low_phase4_555_pruning_table;
  pruning_table<edges_high_phase4_555_pruning> edges_high_phase4_555_pruning_table;

  back_table<edges_phase4_555_back> edges_phase4_555_back_table;
  back_table<phase4_555_back> phase4_555_back_table;
  
  struct phase4_555_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c1, c2, c3, c4;
    edges1 e1;
    edges2 e2;
   
    u8 pruning_value(u8 l) const {
      if(l <= phase4_555_back::DEPTH) {
        u64 k = phase4_555_back { .c1 = c1, .c2 = c2, .c3 = c3, .c4 = c4 }.key(); 
        if(u8 w = phase4_555_back_table.find_distance(k); w != 255) {
          return w;
        }else{
          return 255;
        }
      }
      
      u8 v = 0;
      if(l <= edges_phase4_555_back::DEPTH) {
        u64 k = edges_phase4_555_back { .c3 = c3, .c4 = c4 }.key(); 
        if(u8 w = edges_phase4_555_back_table.find_distance(k); w != 255) {
          v = max(v, w);
        }else{
          return 255;
        }
      }

      v = max(v, centers_phase4_555_pruning_table.distance[c1*4900+c2]);
      v = max(v, edges_low_phase4_555_pruning_table.distance[c3]);
      v = max(v, edges_high_phase4_555_pruning_table.distance[c4]);

      return v;
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 2 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers2_phase4_555_move_table.move(c1, m);
      c2 = centers3_phase4_555_move_table.move(c2, m);
      c3 = edges_low_phase4_555_move_table.move(c3, m);
      c4 = edges_high_phase4_555_move_table.move(c4, m);
      if(!edges2::move_is_slice(m)) e1.move(edges2::move_to_edges1(m));
      e2.move(m);
    }
  };

  struct phase4_555_edges_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c3, c4;
    edges1 e1;
    edges2 e2;
   
    u8 pruning_value(u8 l) const {
      u8 v = 0;
      if(l <= edges_phase4_555_back::DEPTH) {
        u64 k = edges_phase4_555_back { .c3 = c3, .c4 = c4 }.key(); 
        if(u8 w = edges_phase4_555_back_table.find_distance(k); w != 255) {
          v = max(v, w);
        }else{
          return 255;
        }
      }

      v = max(v, edges_low_phase4_555_pruning_table.distance[c3]);
      v = max(v, edges_high_phase4_555_pruning_table.distance[c4]);

      return v;
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 2 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c3 = edges_low_phase4_555_move_table.move(c3, m);
      c4 = edges_high_phase4_555_move_table.move(c4, m);
      if(!edges2::move_is_slice(m)) e1.move(edges2::move_to_edges1(m));
      e2.move(m);
    }
  };
  
  vector<phase4_555_search> make_phase4_555_search(centers2 const& c1, centers3 const& c2, edges1 const& c3, edges2 const& c4) {
    vector<phase4_555_search> ret;
    
    auto mask = c3.lr_colors() | c4.lr_colors();
    runtime_assert(__builtin_popcount(mask) <= 8);
    
    FOR(mask1, 1<<12) if(__builtin_popcount(mask1) == 4 && (mask1&~mask) == mask1) {
      u8 a[4];
      i32 cnt = 0;
      FOR(i,12) if(mask1&(1<<i)) {
        a[cnt++] = i;
      }
      runtime_assert(cnt == 4);

      u32 h1 = centers2_phase4_555_index(c1);
      u32 h2 = centers3_phase4_555_index(c2);
      u32 h3 = edges_low_phase4_555_index(a, c3, c4);
      u32 h4 = edges_high_phase4_555_index(a, c3, c4);
      ret.pb({
          .c1 = h1,
          .c2 = h2,
          .c3 = h3,
          .c4 = h4,
          .e1 = c3,
          .e2 = c4,
        });
    }
    return ret;
  }

  void solve_phase4_555(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    
    auto R = search1<phase4_555_search>(make_phase4_555_search(c.centers2[x], c.centers3[x], c.edges1, c.edges2[x])[0]);
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
 
  void solve_phase4_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<u32> from_idx;
    vector<tuple<u32, phase4_555_search>> cstates;
    FOR(idx, states.size()) {
      auto const& [data,len,S,c] = states[idx];
      auto v = make_phase4_555_search(c.centers2[0], c.centers3[0], c.edges1, c.edges2[0]);
      for(auto const& s : v) {
        from_idx.eb(idx);
        cstates.eb(len+S.length, s);
      }
    }
    auto R = search_beam(cstates, width);
    vector<tuple<u32,u32,move_seq,rubik>> nstates;
    for(auto [idx, M] : R) {
      auto [data,len,S,c] = states[from_idx[idx]];
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
 
  
  vector<phase4_555_edges_search> make_phase4_555_edges_search(centers2 const& c1, centers3 const& c2, edges1 const& c3, edges2 const& c4) {
    vector<phase4_555_edges_search> ret;
    
    auto mask = c3.lr_colors() | c4.lr_colors();
    runtime_assert(__builtin_popcount(mask) <= 8);
    
    FOR(mask1, 1<<12) if(__builtin_popcount(mask1) == 4 && (mask1&~mask) == mask1) {
      u8 a[4];
      i32 cnt = 0;
      FOR(i,12) if(mask1&(1<<i)) {
        a[cnt++] = i;
      }
      runtime_assert(cnt == 4);

      u32 h3 = edges_low_phase4_555_index(a, c3, c4);
      u32 h4 = edges_high_phase4_555_index(a, c3, c4);
      ret.pb({
          .c3 = h3,
          .c4 = h4,
          .e1 = c3,
          .e2 = c4,
        });
    }
    return ret;
  }

  void solve_phase4_555_edges(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    
    auto R = search1<phase4_555_edges_search>(make_phase4_555_edges_search(c.centers2[x], c.centers3[x], c.edges1, c.edges2[x])[0]);
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
 
  void solve_phase4_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<u32> from_idx;
    vector<tuple<u32, phase4_555_edges_search>> cstates;
    FOR(idx, states.size()) {
      auto const& [data,len,S,c] = states[idx];
      auto v = make_phase4_555_edges_search(c.centers2[0], c.centers3[0], c.edges1, c.edges2[0]);
      for(auto const& s : v) {
        from_idx.eb(idx);
        cstates.eb(len+S.length, s);
      }
    }
    auto R = search_beam(cstates, width);
    vector<tuple<u32,u32,move_seq,rubik>> nstates;
    for(auto [idx, M] : R) {
      auto [data,len,S,c] = states[from_idx[idx]];
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
