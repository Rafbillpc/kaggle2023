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
  u64 centers2_phase5_555_index(centers2 const& c) {
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
  u64 centers3_phase5_555_index(centers3 const& c) {
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
  u64 edges_low_phase5_555_index(u8 a[8], edges1 const& c1, edges2 const& c2) {
    // position of edges a[0] .. a[8]
    u8 p[8], q[8];
    FOR(i,8) {
      p[i] = edges1::to_phase5_555[c1.c[a[i]]/2];
      auto x = c2.c[edges2::low[a[i]]]/2;
      auto y = c2.c[edges2::high[a[i]]]/2;
      runtime_assert(edges2::is_low[x] != edges2::is_low[y]);
      if(edges2::is_low[x]) {
        q[i] = edges1::to_phase5_555[edges2::unlow[x]];
      }else{
        q[i] = edges1::to_phase5_555[edges2::unlow[y]];
      }
      runtime_assert(p[i] != 255);
      runtime_assert(q[i] != 255);
    }
    
    u8 r[8];
    FOR(i,8) r[p[i]] = q[i];
   
    return get_permutation_index(8, r);
  }
  
  static inline
  u64 edges_high_phase5_555_index(u8 a[8], edges1 const& c1, edges2 const& c2) {
    // position of edges a[0] .. a[8]
    u8 p[8], q[8];
    FOR(i,8) {
      p[i] = edges1::to_phase5_555[c1.c[a[i]]/2];
      auto x = c2.c[edges2::low[a[i]]]/2;
      auto y = c2.c[edges2::high[a[i]]]/2;
      runtime_assert(edges2::is_low[x] != edges2::is_low[y]);
      if(edges2::is_low[x]) {
        q[i] = edges1::to_phase5_555[edges2::unlow[y]];
      }else{
        q[i] = edges1::to_phase5_555[edges2::unlow[x]];
      }
      runtime_assert(p[i] != 255);
      runtime_assert(q[i] != 255);
    }
    
    u8 r[8];
    FOR(i,8) r[p[i]] = q[i];
   
    return get_permutation_index(8, r);
  }
  struct centers2_phase5_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 343'000;

    auto operator<=>(const centers2_phase5_555_move&) const = default;
    
    centers2 c1;

    void reset(){
      c1.reset();
    }

    u64 index() const {
      return centers2_phase5_555_index(c1);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) {
      c1.move(centers3::move_to_centers2(m));
    }
    void sym_conj(i32 m) { c1.sym_conj(m); }
    void sym(i32 m) { c1.sym(m); }
  };
  extern move_table<centers2_phase5_555_move> centers2_phase5_555_move_table;
  
  struct centers3_phase5_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 343'000;

    auto operator<=>(const centers3_phase5_555_move&) const = default;
    
    centers3 c1;

    void reset(){
      c1.reset();
    }

    u64 index() const {
      return centers3_phase5_555_index(c1);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) {
      c1.move(m);
    }
    void sym_conj(i32 m) { c1.sym_conj(m); }
    void sym(i32 m) { c1.sym(m); }
  };
  extern move_table<centers3_phase5_555_move> centers3_phase5_555_move_table;

  struct edges_low_phase5_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 40'320;

    edges1 c1;
    edges2 c2;

    void reset(){
      c1.reset();
      c2.reset();
    }

    u64 index() const {
      u8 x[8] = {0,2,4,6,8,9,10,11};
      return edges_low_phase5_555_index(x, c1, c2);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) {
      if(!edges2::move_is_slice(m)) {
        c1.move(edges2::move_to_edges1(m));
      }
      c2.move(m);
    }
    void sym_conj(i32 m) { }
    void sym(i32 m) { }
  };
  extern move_table<edges_low_phase5_555_move> edges_low_phase5_555_move_table;

  struct edges_high_phase5_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 40'320;

    edges1 c1;
    edges2 c2;

    void reset(){
      c1.reset();
      c2.reset();
    }

    u64 index() const {
      u8 x[8] = {0,2,4,6,8,9,10,11};
      return edges_high_phase5_555_index(x, c1, c2);
    }

    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }
    void move(i32 m) {
      if(!edges2::move_is_slice(m)) {
        c1.move(edges2::move_to_edges1(m));
      }
      c2.move(m);
    }
    void sym_conj(i32 m) { }
    void sym(i32 m) { }
  };
  extern move_table<edges_high_phase5_555_move> edges_high_phase5_555_move_table;

  struct centers2_phase5_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 343'000;

    u32 c1;

    void reset(){
      c1 = centers2_phase5_555_index(centers2());
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1);
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers2_phase5_555_move_table.move(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers2_phase5_555_pruning> centers2_phase5_555_pruning_table;

  struct centers3_phase5_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 343'000;

    u32 c1;

    void reset(){
      c1 = centers3_phase5_555_index(centers3());
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1);
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = centers3_phase5_555_move_table.move(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers3_phase5_555_pruning> centers3_phase5_555_pruning_table;

  struct edges_low_phase5_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 40'320;

    u32 c1;

    void reset(){
      u8 x[8] = {0,2,4,6,8,9,10,11};
      c1 = edges_low_phase5_555_index(x, edges1(), edges2());
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1);
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = edges_low_phase5_555_move_table.move(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<edges_low_phase5_555_pruning> edges_low_phase5_555_pruning_table;

  struct edges_high_phase5_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 40'320;

    u32 c1;

    void reset(){
      u8 x[8] = {0,2,4,6,8,9,10,11};
      c1 = edges_high_phase5_555_index(x, edges1(), edges2());
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1);
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const {
      auto axis = centers3::move_axis(m);
      auto is_slice = centers3::move_is_slice(m);
      if(axis == 1 && is_slice) return 0;
      if(axis != 1 || is_slice) return m%3 == 1;
      return true;
    }

    void move(i32 m) {
      c1 = edges_high_phase5_555_move_table.move(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<edges_high_phase5_555_pruning> edges_high_phase5_555_pruning_table;
  
  struct centers_phase5_555_back : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 DEPTH    = 30;
    static const u64 HASHSIZE = 1ull<< 26;

    u32 c1, c2;

    void reset(){
      c1 = centers2_phase5_555_index(centers2());
      c2 = centers3_phase5_555_index(centers3());
    }

    u64 key() const {
      return (u64)c1 * 343'000 + c2;
    }
    
    bool allow_move0(i32) const { return false; }
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
    }
    void print() const { } 
  };
  extern back_table<centers_phase5_555_back> centers_phase5_555_back_table;
  
  struct edges_phase5_555_back : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 DEPTH    = 20;
    static const u64 HASHSIZE = 1ull<< 28;

    u32 c3, c4;

    void reset(){
      u8 x[8] = {0,2,4,6,8,9,10,11};
      c3 = edges_low_phase5_555_index(x, edges1(), edges2());
      c4 = edges_high_phase5_555_index(x, edges1(), edges2());
    }

    u64 key() const {
      return (u64)c3 * 40320 + c4;
    }
    
    bool allow_move0(i32) const { return false; }
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
    void print() const { } 
  };
  extern back_table<edges_phase5_555_back> edges_phase5_555_back_table;

  struct phase5_555_back : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 DEPTH    = 16;
    static const u64 HASHSIZE = 1ull<< 29;

    u32 c1, c2, c3, c4;

    void reset(){
      u8 x[8] = {0,2,4,6,8,9,10,11};
      c1 = centers2_phase5_555_index(centers2());
      c2 = centers3_phase5_555_index(centers3());
      c3 = edges_low_phase5_555_index(x, edges1(), edges2());
      c4 = edges_high_phase5_555_index(x, edges1(), edges2());
    }

    u64 key() const {
      u64 k1 = (u64)(c1 * 343'000 + c2);
      u64 k2 = (u64)(c3 * 40320 + c4);
      return uint64_hash::hash_int(2*k1+1) ^ uint64_hash::hash_int(2*k2+2);
    }
    
    bool allow_move0(i32) const { return false; }
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
    void print() const { } 
  };
  extern back_table<phase5_555_back> phase5_555_back_table;
  
  void solve_phase5_555(move_seq& S, rubik& c, i32 x);
  void solve_phase5_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

  void solve_phase5_555_edges(move_seq& S, rubik& c, i32 x);
  void solve_phase5_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);
}
