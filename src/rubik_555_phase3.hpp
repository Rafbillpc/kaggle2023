#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "rubik_tables.hpp"
#include "rubik_pruning_table.hpp"
#include "rubik_sym_table.hpp"
#include "rubik_move_table.hpp"
#include "rubik_back_table.hpp"
#include "rubik_search.hpp"
#include "util.hpp"

namespace rubik {
  
  static inline
  u64 centers2_phase3_555_index(centers2 const& c) {
    const i32 table[24] =
      {
        0,1,2,3,
        -1,-1,-1,-1,
        -1,-1,-1,-1,
        4,5,6,7,
        -1,-1,-1,-1,
        -1,-1,-1,-1,
      };
    u64 mask = 0;
    mask |= (1<<table[c.c[0]]);
    mask |= (1<<table[c.c[1]]);
    mask |= (1<<table[c.c[2]]);
    mask |= (1<<table[c.c[3]]);
    return get_binomial_index(8, 4, mask);
  }
  
  static inline
  u64 centers3_phase3_555_index(centers3 const& c) {
    const i32 table[24] =
      {
        0,1,2,3,
        -1,-1,-1,-1,
        -1,-1,-1,-1,
        4,5,6,7,
        -1,-1,-1,-1,
        -1,-1,-1,-1,
      };
    u64 mask = 0;
    mask |= (1<<table[c.c[0]]);
    mask |= (1<<table[c.c[1]]);
    mask |= (1<<table[c.c[2]]);
    mask |= (1<<table[c.c[3]]);
    return get_binomial_index(8, 4, mask);
  }
 
  static inline
  u64 edges1_phase3_555_index(edges1 c, u32 flip) {
    FOR(i,12) c.c[i] ^= ((flip>>i)&1);
    c.sym_conj_01_12();
    u8 at[12];
    FOR(i,12) at[c.c[i]/2] = 2*i + (c.c[i]&1);

    u64 mask = 0;
    FOR(i,11) if(at[i]&1) mask |= 1<<i;
    
    return mask;
  }
 
  static inline
  u64 edges2_phase3_555_index(edges2 c, u32 flip) {
    u8 at[24];
    FOR(i,24) at[c.c[i]/2] = i;
    u64 mask = 0;
    FOR(i,24) if(edges2::is_low[at[i]] ^ ((flip>>edges2::unlow[at[i]])&1)) {
      mask |= 1<<i;
    }
    runtime_assert(__builtin_popcount(mask) == 12);
    return get_binomial_index(24,12,mask);
  }
  
  struct centers_phase3_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 4'900;

    auto operator<=>(const centers_phase3_555_move&) const = default;
    
    centers2 c1;
    centers3 c2;

    void reset(){
      c1.reset();
      c2.reset();
    }

    u64 index() const {
      return centers2_phase3_555_index(c1) * 70 + centers3_phase3_555_index(c2);
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) {
      c1.move(centers3::move_to_centers2(m));
      c2.move(m);
    }
    void sym_conj(i32 m) { c1.sym_conj(m); c2.sym_conj(m); }
    void sym(i32 m) { c1.sym(m); c2.sym(m); }
  };
  extern move_table<centers_phase3_555_move> centers_phase3_555_move_table;

  struct edges2_phase3_555_sym : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 NRAW = 2'704'156;
    static const i32 NSYM = 339'406;

    auto operator<=>(const edges2_phase3_555_sym&) const = default;
    
    edges2 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return edges2_phase3_555_index(c, 0);
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) { c.move(m); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern sym_table<edges2_phase3_555_sym> edges2_phase3_555_sym_table;

  struct edges1_phase3_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 2048;

    edges1 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return edges1_phase3_555_index(c, 0);
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) {
      if(!edges2::move_is_slice(m)) {
        c.move(edges2::move_to_edges1(m));
      }
    }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern move_table<edges1_phase3_555_move> edges1_phase3_555_move_table;

  struct centers_phase3_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 10'035'200;

    u32 c1, c2;

    void reset(){
      c1 = centers2_phase3_555_index(centers2()) * 70 + centers3_phase3_555_index(centers3());
      c2 = edges1_phase3_555_index(edges1(), 0);
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1 * 2048 + c2);
    }
    
    bool allow_move0(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 2 || is_slice) return m%3 == 1;
      return true;
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers_phase3_555_move_table.move(c1, m);
      c2 = edges1_phase3_555_move_table.move(c2, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers_phase3_555_pruning> centers_phase3_555_pruning_table;

  struct edges2_phase3_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 695'103'488;

    u32 c1, c2;

    void reset(){
      c1 = 0;
      c2 = edges1_phase3_555_index(edges1(), 0);
    }

    template<class F>
    void iter_indices(F&& f) const {
      auto sym_which = edges2_phase3_555_sym_table.raw_sym_which[edges2_phase3_555_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = edges1_phase3_555_move_table.sym_conj
          (c2, sym_inv[sym]);
        f((c1/8) * 2048 + c2_);
      }
    }

    bool allow_move0(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 2 || is_slice) return m%3 == 1;
      return true;
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = edges2_phase3_555_sym_table.move_sym(c1, m);
      c2 = edges1_phase3_555_move_table.move(c2, m);
    }
    void print() const { } 
  };
  extern pruning_table<edges2_phase3_555_pruning> edges2_phase3_555_pruning_table;

  struct phase3_555_back : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 DEPTH    = 8;
    static const u64 HASHSIZE = 1ull<< 29;

    u32 c1, c2, c3;

    void reset(){
      c1 = 0;
      c2 = centers2_phase3_555_index(centers2()) * 70 + centers3_phase3_555_index(centers3());
      c3 = edges1_phase3_555_index(edges1(), 0);
    }

    u64 key() const {
      return (((u64)edges2_phase3_555_sym_table.sym_to_raw[c1] * 4900 + c2) * 2048 + c3) * 0xDE;
    }
    
    bool allow_move0(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 2 || is_slice) return m%3 == 1;
      return true;
    }

    bool allow_move(i32 m) const {
      auto is_slice = centers3::move_is_slice(m);
      if(is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = edges2_phase3_555_sym_table.move_sym(c1, m);
      c2 = centers_phase3_555_move_table.move(c2, m);
      c3 = edges1_phase3_555_move_table.move(c3, m);
    }
    void print() const { } 
  };
  extern back_table<phase3_555_back> phase3_555_back_table;
  
  void solve_phase3_555(move_seq& S, rubik& c, i32 x);
  void solve_phase3_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

  void solve_phase3_555_edges(move_seq& S, rubik& c, i32 x);
  void solve_phase3_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);
}
