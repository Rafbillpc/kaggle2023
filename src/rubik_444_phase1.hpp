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
  u64 centers3_phase1_444_index(centers3 const& c) {
    u8 data[8] = {
      c.c[0],  c.c[1],  c.c[2],  c.c[3],
      c.c[12], c.c[13], c.c[14], c.c[15],
    };
    u32 mask = 0;
    FOR(i,8) mask |= 1<< data[i];
    return get_binomial_index(24, 8, mask);
  }
  
  struct centers3_phase1_444_sym : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 NRAW = 735'471;
    static const i32 NSYM = 92'247;

    auto operator<=>(const centers3_phase1_444_sym&) const = default;
    
    centers3 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers3_phase1_444_index(c);
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) { c.move(m); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern sym_table<centers3_phase1_444_sym> centers3_phase1_444_sym_table;

  struct centers3_phase1_444_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 92'247;

    u32 c1;

    void reset(){
      c1 = 0;
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1/8);
    }
    
    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      c1 = centers3_phase1_444_sym_table.move_sym(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers3_phase1_444_pruning> centers3_phase1_444_pruning_table;

  void solve_phase1_444(move_seq& S, rubik& c, i32 x);
  void solve_phase1_444_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);
}
