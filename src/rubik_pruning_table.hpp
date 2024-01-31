#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "util.hpp"
#include <compare>

namespace rubik {

  template<class state>
  struct pruning_table : with_moves<state::MOVES_PER_AXIS> {
    static const i32 MOVES_PER_AXIS = state::MOVES_PER_AXIS;
    static const i32 MOVES = MOVES_PER_AXIS * 3 * 3;
    static const u32 N = state::N;

    u8* distance = 0;

    void init(string name) {
      // distance = new u8[N];
 
      try_load_table
        (vector<tuple<string, char **, size_t>>
         {make_tuple("data/" + name + "_distance", (char**)&distance, N * sizeof(u8)),},
         [&]()
         {
           FOR(i, N) distance[i] = 255;
           u64 total = 0, total2 = 0;

           vector<state> Q[255];
           auto add = [&](state const& c, i32 dist) {
             bool added = 0;
             c.iter_indices([&](u64 h) -> void {
               if(distance[h] != 255) return;
               distance[h] = dist;
               total2 += 1;
               if(added) return;
               added = true;
               Q[dist].eb(c);
             });
           };
           { state s; s.reset(); add(s, 0); }
           
           FOR(i, Q[0].size()) {
             auto s = Q[0][i];
             FOR(m, MOVES) if(s.allow_move0(m) && s.allow_move(m)){
               auto c = s;
               c.move(m);
               add(c, 0);
             }
           }
           
           FOR(d, 255) if(!Q[d].empty()) {
             for(auto s : Q[d]) {
               total += 1;
               if(total % 1'000'000 == 0) debug(total);
               FOR(m, MOVES) if(move_length(m) == 1 && s.allow_move(m)){
                 auto c = s;
                 c.move(m);
                 add(c, d+1);
               }
             }
             for(auto s : Q[d]) {
               total += 1;
               if(total % 1'000'000 == 0) debug(total);
               FOR(m, MOVES) if(move_length(m) == 2 && s.allow_move(m)){
                 auto c = s;
                 c.move(m);
                 add(c, d+2);
               }
             }
             Q[d].clear();
             Q[d].shrink_to_fit();
             debug(d,total);
           }
           debug(total, total2);
         });
    }
  };
  
}
