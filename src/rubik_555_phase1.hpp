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
  u64 centers2_phase1_555_index(centers2 const& c) {
    u8 data[8] = {
      c.c[0],  c.c[1],  c.c[2],  c.c[3],
      c.c[12], c.c[13], c.c[14], c.c[15],
    };
    u32 mask = 0;
    FOR(i,8) mask |= 1<< data[i];
    return get_binomial_index(24, 8, mask);
  }
  
  static inline
  u64 centers3_phase1_555_index(centers3 const& c) {
    u8 data[8] = {
      c.c[0],  c.c[1],  c.c[2],  c.c[3],
      c.c[12], c.c[13], c.c[14], c.c[15],
    };
    u32 mask = 0;
    FOR(i,8) mask |= 1<< data[i];
    return get_binomial_index(24, 8, mask);
  }
  
  struct centers2_phase1_555_sym : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 NRAW = 735'471;
    static const i32 NSYM = 92'247;

    auto operator<=>(const centers2_phase1_555_sym&) const = default;
    
    centers2 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers2_phase1_555_index(c);
    }

    bool allow_move(i32 m) const { return true; }
    void move(i32 m) { c.move(centers3::move_to_centers2(m)); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern sym_table<centers2_phase1_555_sym> centers2_phase1_555_sym_table;

  struct centers3_phase1_555_move : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const i32 N = 735'471;

    auto operator<=>(const centers3_phase1_555_move&) const = default;
    
    centers3 c;

    void reset(){
      c.reset();
    }

    u64 index() const {
      return centers3_phase1_555_index(c);
    }

    bool allow_move(i32) const { return true; }
    void move(i32 m) { c.move(m); }
    void sym_conj(i32 m) { c.sym_conj(m); }
    void sym(i32 m) { c.sym(m); }
  };
  extern move_table<centers3_phase1_555_move> centers3_phase1_555_move_table;

  struct centers2_phase1_555_pruning : with_moves<4> {
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
    bool allow_move(i32 m) const { return true; }

    void move(i32 m) {
      c1 = centers2_phase1_555_sym_table.move_sym(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers2_phase1_555_pruning> centers2_phase1_555_pruning_table;

  struct centers3_phase1_555_pruning : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 N = 735'471;

    u32 c1;

    void reset(){
      centers3 c; c.reset();
      c1 = centers3_phase1_555_index(c);
    }

    template<class F>
    void iter_indices(F&& f) const {
      f(c1);
    }
    
    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      c1 = centers3_phase1_555_move_table.move(c1, m);
    }
    void print() const { } 
  };
  extern pruning_table<centers3_phase1_555_pruning> centers3_phase1_555_pruning_table;

  struct centers_phase1_555_back : with_moves<4> {
    static const i32 MOVES_PER_AXIS = 4;
    static const u64 DEPTH    = 8;
    static const u64 HASHSIZE = 1ull<< 26;

    u32 c1, c2;

    void reset(){
      c1 = 0;
      centers3 c; c.reset();
      c2 = centers3_phase1_555_index(c);
    }

    u64 key() const {
      u64 k = numeric_limits<u64>::max();

      // debug(c1/8, c2/8, c1, cm1, cb1, c2, cm2, cb2);
      
      { auto sym_which = centers2_phase1_555_sym_table.raw_sym_which[centers2_phase1_555_sym_table.sym_to_raw[c1]];
        FOR(sym, 8) if(sym_which & (1<<sym)) {
          auto c2_ = centers3_phase1_555_move_table.sym_conj
            (c2, sym_inv[sym]);
          k = min(k, (u64)(c1/8) * 735'471 + c2_);
        }
      }

      return k;
    }
    
    bool allow_move0(i32) const { return false; }
    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      c1 = centers2_phase1_555_sym_table.move_sym(c1, m);
      c2 = centers3_phase1_555_move_table.move(c2, m);
    }
    void print() const { } 
  };
  extern back_table<centers_phase1_555_back> centers_phase1_555_back_table;
  
  void solve_phase1_555(move_seq& S, rubik& c, i32 x);
  void solve_phase1_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width);
}
