#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "rubik_tables.hpp"
#include "rubik_pruning_table.hpp"
#include "rubik_sym_table.hpp"
#include "rubik_search.hpp"
#include "util.hpp"

namespace rubik {

  static inline
  u32 corners_full_index(corners const& c) {
    u64 p = get_permutation_index(8, c.c.data());
    u64 o = 0;
    FOR(i,7) o = 3*o + c.c[i]%3;
    return p * 2187 + o;
  }
  
  struct corners_state_sym {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 NRAW = 88'179'840;
    static const i32 NSYM = 11'028'984;
    
    auto operator<=>(const corners_state_sym&) const = default;
    
    corners c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return corners_full_index(c);
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) { c.move(m); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };

  extern sym_table<corners_state_sym> corners_sym_table;
  
  struct corners_state_pruning {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 N = 11'028'984;
    
    u32 c;

    void reset(){
      c = 0;
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c/8);
    }

    bool allow_move0(i32) const { return false; }
    bool allow_move(i32) const { return true; }
    void move(i32 m) {
      c = corners_sym_table.move_sym(c, m);
    }
    void print() const { } 
  };
  
  extern pruning_table<corners_state_pruning> corners_pruning_table;

  vector<i32> search_corners(corners &c);
  void solve_corners(move_seq& S, rubik &c);
}
