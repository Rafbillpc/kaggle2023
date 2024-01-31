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
  
  const i32 pow3[8] = {1,3,9,27,81,243,729,2187};

  static inline
  u32 edges1_udslice_index(edges1 const& c){
    u32 m = 0;
    m |= (1<< (c.c[8]/2));
    m |= (1<< (c.c[9]/2));
    m |= (1<< (c.c[10]/2));
    m |= (1<< (c.c[11]/2));
    return get_binomial_index(12,4,m);
  }

  static inline
  u32 edges1_flip_index(edges1 const& c){
    u32 o = 0;
    FOR(i,12) if(c.c[i]&1) o |= 1<< (c.c[i]/2);
    return o&2047;
  }

  static inline
  u32 edges1_flipudslice_index(edges1 const& c){
    return edges1_udslice_index(c) * 2048 + edges1_flip_index(c);
  }

  static inline
  u32 corners_twist_index(corners const& c){
    u32 o = 0;
    FOR(i,8) o += (c.c[i]%3) * pow3[c.c[i]/3];
    return (o%2187);
  }
  
  struct flipudslice_sym : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 NRAW = 2048 * 495;
    static const i32 NSYM = 127'166;

    auto operator<=>(const flipudslice_sym&) const = default;
    
    edges1 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return edges1_flipudslice_index(c);
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) {
      i32 axis = move_axis(m), index = move_index(m), dir = m%3;
      c.move((3*axis + 2*index)*3 + dir);
    }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };

  extern sym_table<flipudslice_sym> flipudslice_sym_table;

  struct corners_twist_move : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 N = 2187;

    auto operator<=>(const corners_twist_move&) const = default;
    
    corners c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return corners_twist_index(c);
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) {
      c.move(m);
    }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };

  extern move_table<corners_twist_move> corners_twist_move_table;

  struct kociemba_phase1_pruning : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const u64 N = 127'166 * 2187;

    u32 c1, c2;

    void reset(){
      c1 = c2 = 0;
    }

    template<class F>
    void iter_indices(F&& f) const {
      auto sym_which = flipudslice_sym_table.raw_sym_which[flipudslice_sym_table.sym_to_raw[c1]];
      FOR(sym, 8) if(sym_which & (1<<sym)) {
        auto c2_ = corners_twist_move_table.sym_conj
          (c2, sym_inv[sym]);
        f((c1/8) * 2187 + c2_);
      }
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32) const { return true; }
    void move(i32 m) {
      c1 = flipudslice_sym_table.move_sym(c1, m);
      c2 = corners_twist_move_table.move(c2, m);
    }
    void print() const { } 
  };

  extern pruning_table<kociemba_phase1_pruning> kociemba_phase1_pruning_table;

  vector<i32> search_centers1(centers1 const& c);
  void solve_centers1(move_seq& S, rubik& c);
  
  void solve_kociemba_phase1(move_seq& S, rubik& c);
  vector<tuple<move_seq,rubik>> solve_kociemba_phase1_many(move_seq S, rubik c, i32 count);
  void solve_kociemba_phase1_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);
}

