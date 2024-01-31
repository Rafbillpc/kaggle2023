#include "kociemba_phase1.hpp"
#include "rubik.hpp"
#include "rubik_search.hpp"

namespace rubik {
  sym_table<flipudslice_sym> flipudslice_sym_table;
  move_table<corners_twist_move> corners_twist_move_table;
  // pruning_table<flipudslice_pruning> flipudslice_pruning_table;
  pruning_table<kociemba_phase1_pruning> kociemba_phase1_pruning_table;

  
  struct centers_search : with_moves<1> {
    static const i32 MOVES_PER_AXIS = 1;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    centers1 c;
    
    u8 pruning_value(u8) const {
      return c.is_solved() ? 0 : 1;
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) { c.move(m); }
  };

  struct kociemba_phase1_search : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    u32 c1, c2;
    
    u8 pruning_value(u8) const {
      auto sym_which = flipudslice_sym_table.raw_sym_which[flipudslice_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = corners_twist_move_table.sym_conj
          (c2, sym_inv[sym]);
        return kociemba_phase1_pruning_table.distance[(c1/8) * 2187 + c2_];
      }
      runtime_assert(false);
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) {
      c1 = flipudslice_sym_table.move_sym(c1, m);
      c2 = corners_twist_move_table.move(c2, m);
    }
  };

  vector<i32> search_centers1(centers1 const& c) {
    return search1<centers_search>({
        .c = c
      });
  }

  void solve_centers1(move_seq& S, rubik& c) {
    auto N = c.N;
    auto R = search_centers1(c.centers1);
    for(auto m : R) {
      i32 axis = centers1::move_axis(m), index = centers1::move_real_index(N,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }
  
  kociemba_phase1_search make_kociemba_phase1_search(corners const& c1, edges1 const& c2) {
    auto h1 = edges1_flipudslice_index(c2);
    return {
      .c1 = flipudslice_sym_table.raw_to_sym[h1] * 8 + __builtin_ctz(flipudslice_sym_table.raw_sym_which[h1]),
      .c2 = corners_twist_index(c1),
    };
  }

  vector<i32> search_kociemba_phase1(corners const& c1, edges1 const& c2) {
    auto h1 = edges1_flipudslice_index(c2);
    return search1<kociemba_phase1_search>({
        .c1 = flipudslice_sym_table.raw_to_sym[h1] * 8 + __builtin_ctz(flipudslice_sym_table.raw_sym_which[h1]),
        .c2 = corners_twist_index(c1),
      });
  }

  void solve_kociemba_phase1(move_seq& S, rubik& c) {
    auto N = c.N;
    runtime_assert(N&1);
    auto R = search_kociemba_phase1(c.corners, c.edges1);
    for(auto m : R) {
      i32 axis = corners::move_axis(m), index = corners::move_real_index(N,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }

  vector<vector<i32>> search_kociemba_phase1_many(corners const& c1, edges1 const& c2, i32 count) {
    auto h1 = edges1_flipudslice_index(c2);
    return search_many<kociemba_phase1_search>({
        .c1 = flipudslice_sym_table.raw_to_sym[h1] * 8 + __builtin_ctz(flipudslice_sym_table.raw_sym_which[h1]),
        .c2 = corners_twist_index(c1),
      }, count);
  }

  vector<tuple<move_seq,rubik>> solve_kociemba_phase1_many(move_seq S, rubik c, i32 count) {
    auto N = c.N;
    runtime_assert(N&1);
    auto RS = search_kociemba_phase1_many(c.corners, c.edges1, count);
    vector<tuple<move_seq,rubik>> A;
    for(auto const& R : RS) {
      auto T = S;
      auto d = c;
      for(auto m : R) {
        i32 axis = corners::move_axis(m), index = corners::move_real_index(N,m), dir = 1+m%3;
        T.move(axis, index, dir);
        d.move(axis, index, dir);
      }
      A.eb(T,d);
    }
    return A;
  }

  void solve_kociemba_phase1_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    i32 N = 3;
    vector<tuple<u32, kociemba_phase1_search>> cstates;
    for(auto const& [data,len,S,c] : states) {
      cstates.eb(len+S.length, make_kociemba_phase1_search(c.corners, c.edges1));
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
