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
  u64 centers3_phase3_444_index(centers3 const& c) {
    const i32 table1[] =
      {0,1,2,3,
       -1,-1,-1,-1,
       -1,-1,-1,-1,
       4,5,6,7,
       -1,-1,-1,-1,
       -1,-1,-1,-1,
      };
    u64 mask1 = 0;
    mask1 |= (1<<table1[c.c[0]]);
    mask1 |= (1<<table1[c.c[1]]);
    mask1 |= (1<<table1[c.c[2]]);
    mask1 |= (1<<table1[c.c[3]]);
    u32 x1 = get_binomial_index(8, 4, mask1);
    const i32 table2[] =
      {-1,-1,-1,-1,
       0,1,2,3,
       -1,-1,-1,-1,
       -1,-1,-1,-1,
       4,5,6,7,
       -1,-1,-1,-1,
      };
    u64 mask2 = 0;
    mask2 |= (1<<table2[c.c[4]]);
    mask2 |= (1<<table2[c.c[5]]);
    mask2 |= (1<<table2[c.c[6]]);
    mask2 |= (1<<table2[c.c[7]]);
    u32 x2 = get_binomial_index(8, 4, mask2);
    const i32 table3[] =
      {-1,-1,-1,-1,
       -1,-1,-1,-1,
       0,1,2,3,
       -1,-1,-1,-1,
       -1,-1,-1,-1,
       4,5,6,7,
      };
    u64 mask3 = 0;
    mask3 |= (1<<table3[c.c[8]]);
    mask3 |= (1<<table3[c.c[9]]);
    mask3 |= (1<<table3[c.c[10]]);
    mask3 |= (1<<table3[c.c[11]]);
    u32 x3 = get_binomial_index(8, 4, mask3);
    return ((x1 * 70 + x2) * 70) + x3;
  }
 
  static inline
  u64 edges2_phase3_444_index(edges2 const& c) {
    u8 p[12], q[12], r[12];
    FOR(i, 12) {
      u8 x = c.c[edges2::low[i]]/2;
      u8 y = c.c[edges2::high[i]]/2;
      if(edges2::is_low[x]) {
        p[i] = edges2::unlow[x];
        q[i] = edges2::unlow[y];
      }else{
        p[i] = edges2::unlow[y];
        q[i] = edges2::unlow[x];
      }
    }
    FOR(i, 12) r[p[i]] = q[i];
    
    return get_permutation_index(12, r);
  }
  
  struct centers3_phase3_444_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 343'000;

    centers3 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers3_phase3_444_index(c);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 0 || is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) { c.move(m); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern move_table<centers3_phase3_444_move> centers3_phase3_444_move_table;

  struct centers3_phase3_444_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 343'000;

    u32 c1;

    void reset(){
      centers3 c; c.reset();
      c1 = centers3_phase3_444_index(c);
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1);
    }
    
    bool allow_move0(i32 m) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 0 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers3_phase3_444_move_table.move(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers3_phase3_444_pruning> centers3_phase3_444_pruning_table;

  struct edges2_phase3_444_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 479'001'600;

    edges2 c;

    void reset(){
      c.reset();
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(edges2_phase3_444_index(c));
    }
    
    bool allow_move0(i32 m) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = edges2::move_axis(m);
      auto is_slice = edges2::move_is_slice(m);
      if(axis == 0 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c.move(m);
    }
    void print() const { } 
  };
  extern pruning_table<edges2_phase3_444_pruning> edges2_phase3_444_pruning_table;

  struct phase3_444_back : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 DEPTH    = 12;
    static const u64 HASHSIZE = 1ull<< 29;

    u32 c1;
    edges2 c2;

    void reset(){
      c1 = centers3_phase3_444_index(centers3());
      c2.reset();
    }

    u64 key() const {
      return (u64)c1 * 479001600 + edges2_phase3_444_index(c2);
    }
    
    bool allow_move0(i32 m) const { return false; }
    bool allow_move(i32 m) const { 
      auto axis = edges2::move_axis(m);
      auto is_slice = edges2::move_is_slice(m);
      if(axis == 0 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers3_phase3_444_move_table.move(c1, m);
      c2.move(m);
    }
    void print() const { } 
  };
  extern back_table<phase3_444_back> phase3_444_back_table;
 
  void solve_phase3_444(move_seq& S, rubik& c, i32 x);
  void solve_phase3_444_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

  void solve_phase3_444_edges(move_seq& S, rubik& c, i32 x);
  void solve_phase3_444_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

}
