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
  u64 centers3_phase2_444_index(centers3 const& c) {
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
    u32 x1 = get_binomial_index(16, 8, mask1);
    const i32 table2[] =
      {0,1,2,3,
       -1,-1,-1,-1,
       -1,-1,-1,-1,
       4,5,6,7,
       -1,-1,-1,-1,
       -1,-1,-1,-1,
      };
    u64 mask2 = 0;
    mask2 |= (1<<table2[c.c[0]]);
    mask2 |= (1<<table2[c.c[1]]);
    mask2 |= (1<<table2[c.c[2]]);
    mask2 |= (1<<table2[c.c[3]]);
    u32 x2 = get_binomial_index(8, 4, mask2);
    runtime_assert(x2 < 70);
    return x1 * 70 + x2;
  }

  struct centers3_phase2_444_sym : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 NRAW = 900'900;
    static const i32 NSYM = 113'977;

    auto operator<=>(const centers3_phase2_444_sym&) const = default;
    
    centers3 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers3_phase2_444_index(c);
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
  extern sym_table<centers3_phase2_444_sym> centers3_phase2_444_sym_table;

  struct centers3_phase2_444_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 113'977;

    u32 c1;

    void reset(){
      c1 = 0;
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1/8);
    }
    
    bool allow_move0(i32 m) const { 
      auto axis = edges2::move_axis(m);
      auto is_slice = edges2::move_is_slice(m);
      if(axis == 0 || is_slice) return m%3 == 1;
      return true;
    }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis != 0 && is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers3_phase2_444_sym_table.move_sym(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers3_phase2_444_pruning> centers3_phase2_444_pruning_table;

  void solve_phase2_444(move_seq& S, rubik& c, i32 x);
  void solve_phase2_444_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

  void solve_phase2_444_edges(move_seq& S, rubik& c, i32 x);
  void solve_phase2_444_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);
}
