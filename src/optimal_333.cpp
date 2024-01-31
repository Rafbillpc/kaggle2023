#include "optimal_333.hpp"
#include "kociemba_phase1.hpp"
#include "rubik.hpp"


namespace rubik {
  back_table<optimal_333_back> optimal_333_back_table;

  struct optimal_333_search : with_moves<2> {
    static const i32 MOVES_PER_AXIS = 2;
    static const i32 MOVES = MOVES_PER_AXIS * 9;

    corners c1;
    edges1  c2;

    u8 pruning_value(u8 l) const {
      if(l <= optimal_333_back::DEPTH) {
        u64 k = optimal_333_back { .c1 = c1, .c2 = c2 }.key(); 
        if(u8 w = optimal_333_back_table.find_distance(k); w != 255) {
          return w;
        }else{
          return 255;
        }
      }

      u8 v = 0;
      v = max(v, kociemba_phase1_pruning_table.distance
              [flipudslice_sym_table.raw_to_sym_full(edges1_flipudslice_index(c2)) +
               corners_twist_index(c1)]);
      { auto d1 = c1;
        auto d2 = c2;
        d1.sym_conj_01_12();
        d2.sym_conj_01_12();
        v = max(v, kociemba_phase1_pruning_table.distance
                [flipudslice_sym_table.raw_to_sym_full(edges1_flipudslice_index(d2)) +
                 corners_twist_index(d1)]);
      }
      { auto d1 = c1;
        auto d2 = c2;
        d1.sym_conj(1);
        d1.sym_conj_01_12();
        d2.sym_conj(1);
        d2.sym_conj_01_12();
        v = max(v, kociemba_phase1_pruning_table.distance
                [flipudslice_sym_table.raw_to_sym_full(edges1_flipudslice_index(d2)) +
                 corners_twist_index(d1)]);
      }
      
      return v;
    }
    
    bool allow_move(i32 m) const { return 1; }

    void move(i32 m) {
      i32 axis = move_axis(m), index = move_index(m), dir = m%3;
      c1.move(m);
      c2.move((3*axis + 2*index)*3 + dir);
    }
  };

  void solve_optimal_333(move_seq& S, rubik& c) {
    auto N = c.N;
    runtime_assert(N&1);
    auto R = search1<optimal_333_search>({
        .c1 = c.corners,
        .c2 = c.edges1,
      });
    for(auto m : R) {
      i32 axis = corners::move_axis(m), index = corners::move_real_index(N,m), dir = 1+m%3;
      S.move(axis, index, dir);
      c.move(axis, index, dir);
    }
  }

}

