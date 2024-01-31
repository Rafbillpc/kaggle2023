#include "rubik_555_phase3.hpp"
#include "header.hpp"

namespace rubik {
  move_table<centers_phase3_555_move> centers_phase3_555_move_table;
  sym_table<edges2_phase3_555_sym> edges2_phase3_555_sym_table;
  move_table<edges1_phase3_555_move> edges1_phase3_555_move_table;

  pruning_table<centers_phase3_555_pruning> centers_phase3_555_pruning_table;
  pruning_table<edges2_phase3_555_pruning> edges2_phase3_555_pruning_table;
  back_table<phase3_555_back> phase3_555_back_table;

  struct phase3_555_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;
    
    u32 c1, c2, c3;
    edges1 e1;
    edges2 e2;
    
    u8 pruning_value(u8 l) const {
      if(l <= phase3_555_back::DEPTH) {
        u64 k = phase3_555_back { .c1 = c2, .c2 = c1, .c3 = c3 }.key(); 
        if(u8 v = phase3_555_back_table.find_distance(k); v != 255) {
          if(v == 0) {
            u32 m = e1.lr_colors() | e2.lr_colors();
            return __builtin_popcount(m) <= 8 ? 0 : 1;
          }
          return v;
        }
        return 255;
      }
      
      u8 v = 0;
      v = max(v, centers_phase3_555_pruning_table.distance[c1 * 2048 + c3]);

      { auto sym_which = edges2_phase3_555_sym_table.raw_sym_which[edges2_phase3_555_sym_table.sym_to_raw[c2]];
        FOR(sym, 8) if(sym_which & (1<<sym)) {
          auto c3_ = edges1_phase3_555_move_table.sym_conj
            (c3, sym_inv[sym]);
          v = max(v, edges2_phase3_555_pruning_table.distance[(c2/8) * 2048 + c3_]);
          break;
        }
      }

      if(v == 0) {
        u32 m = e1.lr_colors() | e2.lr_colors();
        return __builtin_popcount(m) <= 8 ? 0 : 1;
      }

      return v;
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers_phase3_555_move_table.move(c1, m);
      c2 = edges2_phase3_555_sym_table.move_sym(c2, m);
      c3 = edges1_phase3_555_move_table.move(c3, m);
      if(!edges2::move_is_slice(m)) e1.move(edges2::move_to_edges1(m));
      e2.move(m);
    }
  };

  struct phase3_555_edges_search : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 MOVES = MOVES_PER_AXIS * 9;
    
    u32 c2, c3;
    edges1 e1;
    edges2 e2;
    
    u8 pruning_value(u8 l) const {
      u8 v = 0;
      { auto sym_which = edges2_phase3_555_sym_table.raw_sym_which[edges2_phase3_555_sym_table.sym_to_raw[c2]];
        FOR(sym, 8) if(sym_which & (1<<sym)) {
          auto c3_ = edges1_phase3_555_move_table.sym_conj
            (c3, sym_inv[sym]);
          v = max(v, edges2_phase3_555_pruning_table.distance[(c2/8) * 2048 + c3_]);
          break;
        }
      }

      if(v == 0) {
        u32 m = e1.lr_colors() | e2.lr_colors();
        return __builtin_popcount(m) <= 8 ? 0 : 1;
      }

      return v;
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c2 = edges2_phase3_555_sym_table.move_sym(c2, m);
      c3 = edges1_phase3_555_move_table.move(c3, m);
      if(!edges2::move_is_slice(m)) e1.move(edges2::move_to_edges1(m));
      e2.move(m);
    }
  };

  vector<phase3_555_search> make_phase3_555_search(centers2 const& c1, centers3 const& c2, edges1 const& c3, edges2 const& c4) {
    vector<phase3_555_search> ret;
    FOR(flip, 2048) if(__builtin_popcount(flip)%2 == 0) {
      if(flip != 0) continue;
      u32 h1 = centers2_phase3_555_index(c1);
      u32 h2 = centers3_phase3_555_index(c2);
      u32 h3 = edges1_phase3_555_index(c3, flip);
      u32 h4 = edges2_phase3_555_index(c4, flip);
      ret.pb({
          .c1 = h1 * 70 + h2,
          .c2 = edges2_phase3_555_sym_table.raw_to_sym_full(h4),
          .c3 = h3,
          .e1 = c3,
          .e2 = c4,
        });
    }
    return ret;
  }

  void solve_phase3_555(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase3_555_search>(make_phase3_555_search(c.centers2[x], c.centers3[x], c.edges1, c.edges2[x])[0]);
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
 
  void solve_phase3_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<u32> from_idx;
    vector<tuple<u32, phase3_555_search>> cstates;
    FOR(idx, states.size()) {
      auto const& [data,len,S,c] = states[idx];
      auto v = make_phase3_555_search(c.centers2[0], c.centers3[0], c.edges1, c.edges2[0]);
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


  vector<phase3_555_edges_search> make_phase3_555_edges_search(centers2 const& c1, centers3 const& c2, edges1 const& c3, edges2 const& c4) {
    vector<phase3_555_edges_search> ret;
    FOR(flip, 2048) if(__builtin_popcount(flip)%2 == 0) {
      if(flip != 0) continue;
      u32 h3 = edges1_phase3_555_index(c3, flip);
      u32 h4 = edges2_phase3_555_index(c4, flip);
      ret.pb({
          .c2 = edges2_phase3_555_sym_table.raw_to_sym_full(h4),
          .c3 = h3,
          .e1 = c3,
          .e2 = c4,
        });
    }
    return ret;
  }

  void solve_phase3_555_edges(move_seq& S, rubik& c, i32 x) {
    auto N = c.N;
    auto R = search1<phase3_555_edges_search>(make_phase3_555_edges_search(c.centers2[x], c.centers3[x], c.edges1, c.edges2[x])[0]);
    for(auto m : R) {
      i32 axis = centers3::move_axis(m), index = centers3::move_real_index(N,x,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
 
  void solve_phase3_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 5;
    vector<u32> from_idx;
    vector<tuple<u32, phase3_555_edges_search>> cstates;
    FOR(idx, states.size()) {
      auto const& [data,len,S,c] = states[idx];
      auto v = make_phase3_555_edges_search(c.centers2[0], c.centers3[0], c.edges1, c.edges2[0]);
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
