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
  u64 centers2_phase2_555_index(centers2 const& c) {
    const i32 table1[] =
      {-1,-1,-1,-1,
       0,1,2,3,
       4,5,6,7,
       -1,-1,-1,-1,
       8,9,10,11,
       12,13,14,15,
      };
    u64 mask1 = 0;
    mask1 |= (1<<table1[c.c[4]]);
    mask1 |= (1<<table1[c.c[5]]);
    mask1 |= (1<<table1[c.c[6]]);
    mask1 |= (1<<table1[c.c[7]]);
    mask1 |= (1<<table1[c.c[16]]);
    mask1 |= (1<<table1[c.c[17]]);
    mask1 |= (1<<table1[c.c[18]]);
    mask1 |= (1<<table1[c.c[19]]);
    return get_binomial_index(16, 8, mask1);
  }
  
  static inline
  u64 centers3_phase2_555_index(centers3 const& c) {
    const i32 table1[] =
      {-1,-1,-1,-1,
       0,1,2,3,
       4,5,6,7,
       -1,-1,-1,-1,
       8,9,10,11,
       12,13,14,15,
      };
    u64 mask1 = 0;
    mask1 |= (1<<table1[c.c[4]]);
    mask1 |= (1<<table1[c.c[5]]);
    mask1 |= (1<<table1[c.c[6]]);
    mask1 |= (1<<table1[c.c[7]]);
    mask1 |= (1<<table1[c.c[16]]);
    mask1 |= (1<<table1[c.c[17]]);
    mask1 |= (1<<table1[c.c[18]]);
    mask1 |= (1<<table1[c.c[19]]);
    return get_binomial_index(16, 8, mask1);
  }
  
  struct centers2_phase2_555_sym : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 NRAW = 12'870;
    static const i32 NSYM = 1'703;

    auto operator<=>(const centers2_phase2_555_sym&) const = default;
    
    centers2 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers2_phase2_555_index(c);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 0 && is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) { c.move(centers3::move_to_centers2(m)); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern sym_table<centers2_phase2_555_sym> centers2_phase2_555_sym_table;

  struct centers3_phase2_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 12'870;

    auto operator<=>(const centers3_phase2_555_move&) const = default;
    
    centers3 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers3_phase2_555_index(c);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 0 && is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) { c.move(m); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern move_table<centers3_phase2_555_move> centers3_phase2_555_move_table;

  struct centers_phase2_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 21'917'610;

    u32 c1, c2;

    void reset(){
      c1 = 0;
      centers3 c; c.reset();
      c2 = centers3_phase2_555_index(c);
    }

    template<class F>
    void iter_indices(F&& f) const {
      auto sym_which = centers2_phase2_555_sym_table.raw_sym_which[centers2_phase2_555_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = centers3_phase2_555_move_table.sym_conj
          (c2, sym_inv[sym]);
        f((c1/8) * 12'870 + c2_);
      }
    }
    
    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 0 && is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers2_phase2_555_sym_table.move_sym(c1, m);
      c2 = centers3_phase2_555_move_table.move(c2, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers_phase2_555_pruning> centers_phase2_555_pruning_table;
  
  void solve_phase2_555(move_seq& S, rubik& c, i32 x);
  void solve_phase2_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

  void solve_phase2_555_edges(move_seq& S, rubik& c, i32 x);
  void solve_phase2_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

}
