#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "rubik_tables.hpp"
#include "rubik_pruning_table.hpp"
#include "rubik_sym_table.hpp"
#include "rubik_move_table.hpp"
#include "rubik_search.hpp"
#include "util.hpp"

namespace rubik {

  static inline
  u32 corners_perm_index(corners const& c) {
    return get_permutation_index(8, c.c.data());
  }

  static inline
  u32 edges_perm_index(edges1 const& c) {
    u8 data[8];
    FOR(i,8) data[i] = c.c[i];
    return get_permutation_index(8, data);
  }
 
  static inline
  u32 udslice_perm_index(edges1 const& c) {
    u8 data[4];
    FOR(i,4) data[i] = c.c[8+i];
    return get_permutation_index(4, data);
  }
  
  struct corners_perm_sym : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 NRAW = 40'320;
    static const i32 NSYM = 5'288;
    
    auto operator<=>(const corners_perm_sym&) const = default;

    corners c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return corners_perm_index(c); 
    }

    bool allow_move(i32 m) const {
      i32 axis = move_axis(m), dir = m%3;
      return axis == 0 || dir == 1;
    }
    void move(i32 m) {
      c.move(m);
    }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };

  extern sym_table<corners_perm_sym> corners_perm_sym_table;
  
  struct edges_perm_move : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 N = 40'320;
    
    edges1 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return edges_perm_index(c);
    }

    bool allow_move(i32 m) const {
      i32 axis = move_axis(m), dir = m%3;
      return axis == 0 || dir == 1;
    }
    void move(i32 m) {
      i32 axis = move_axis(m), index = move_index(m), dir = m%3;
      c.move((3*axis + 2*index)*3 + dir);
    }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };

  extern move_table<edges_perm_move> edges_perm_move_table;
  
  struct udslice_perm_move : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 N = 40'320;
    
    edges1 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return udslice_perm_index(c);
    }

    bool allow_move(i32 m) const {
      i32 axis = move_axis(m), dir = m%3;
      return axis == 0 || dir == 1;
    }
    void move(i32 m) {
      i32 axis = move_axis(m), index = move_index(m), dir = m%3;
      c.move((3*axis + 2*index)*3 + dir);
    }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };

  extern move_table<udslice_perm_move> udslice_perm_move_table;
  
  struct udslice_perm_pruning : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 N = 40'320;
    
    u32 c;

    void reset(){
      c = 0;
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c);
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const {
      i32 axis = move_axis(m), dir = m%3;
      return axis == 0 || dir == 1;
    }
    void move(i32 m) {
      c = udslice_perm_move_table.move(c, m);
    }
    void print() const { } 
  };

  extern pruning_table<udslice_perm_pruning> udslice_perm_pruning_table;


  struct kociemba_phase2_pruning : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const u64 N = 5'288 * 40'320;

    u32 c1, c2;

    void reset(){
      c1 = c2 = 0;
    }

    template<class F>
    void iter_indices(F&& f) const {
      auto sym_which = corners_perm_sym_table.raw_sym_which[corners_perm_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = edges_perm_move_table.sym_conj
          (c2, sym_inv[sym]);
        f((c1/8) * 40'320 + c2_);
      }
    }
    
    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const { 
      i32 axis = move_axis(m), dir = m%3;
      return axis == 0 || dir == 1;
    }
    void move(i32 m) {
      c1 = corners_perm_sym_table.move_sym(c1, m);
      c2 = edges_perm_move_table.move(c2, m);
    }
    void print() const { } 
  };

  extern pruning_table<kociemba_phase2_pruning> kociemba_phase2_pruning_table;
 
  void solve_kociemba_phase2(move_seq& S, rubik& c);
  void solve_kociemba_phase2_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);

}
