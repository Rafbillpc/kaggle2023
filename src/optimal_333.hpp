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

  struct optimal_333_back : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const u64 DEPTH    = 9;
    static const u64 HASHSIZE = 1ull<< 30;

    corners c1;
    edges1  c2;

    u32 c3, c4;

    void reset(){
      c1.reset();
      c2.reset();
    }

    u64 key() const {
      auto k = get_permutation_index(8, c1.c.data()) * 479001600 + get_permutation_index(12, c2.c.data());
      FOR(i, 7) k = 3*k + (c1.c[i]%3);
      FOR(i, 11) k = 2*k + (c2.c[i]%2);
      return k;
    }
    
    bool allow_move(i32 m) const { return 1; }
    
    void move(i32 m) {
      i32 axis = move_axis(m), index = move_index(m), dir = m%3;
      c1.move(m);
      c2.move((3*axis + 2*index)*3 + dir);
    }
    void print() const { } 
  };
  extern back_table<optimal_333_back> optimal_333_back_table;
  
  void solve_optimal_333(move_seq& S, rubik& c);
}

