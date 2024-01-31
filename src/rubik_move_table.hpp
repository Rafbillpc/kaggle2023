#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "util.hpp"
#include <compare>

namespace rubik {

  template<class state>
  struct move_table : with_moves<state::MOVES_PER_AXIS> {
    static const i32 MOVES_PER_AXIS = state::MOVES_PER_AXIS;
    static const i32 MOVES = MOVES_PER_AXIS * 3 * 3;
    static const u64 N = state::N;

    u64* moves = 0;
    u64* sym = 0;

    u32 move(u32 x, u32 m) {
      return moves[x * MOVES + m];
    }

    u32 sym_conj(u32 x, u32 m) {
      return sym[x * 8 + m];
    }
    
    void init(string name) {
      // moves = new u64[N*MOVES];
      // sym = new u64[N*8];
 
      try_load_table
        (vector<tuple<string, char **, size_t>>
         {make_tuple("data/" + name + "_moves", (char**)&moves, N*MOVES * sizeof(u64)),
          make_tuple("data/" + name + "_sym", (char**)&sym, N*8 * sizeof(u64)),},
         [&]()
         {
           FOR(i,N*MOVES) moves[i] = -1;

           vector<bool> seen(N, 0);
           u64 total = 0;

           vector<state> Q[255];
           auto add = [&](u64 sh, i32 m, state const& c, i32 dist) {
             auto h = c.index();
             moves[sh * MOVES + m] = h;
             if(seen[h]) return;
             FOR(s,8) {
               auto d = c;
               d.sym_conj(s);
               sym[h*8+s] = d.index();
             }
             seen[h] = 1;
             total += 1;
             Q[dist].eb(c);
             return;
           };
           { state s; s.reset(); add(0,0, s, 0); }

           FOR(d, 255) if(!Q[d].empty()) {
             for(auto s : Q[d]) {
               auto h = s.index();
               FOR(m, MOVES) if(move_length(m) == 1 && s.allow_move(m)){
                 s.move(m);
                 add(h,m, s, d+1);
                 s.move(move_opposite(m));
               }
             }
             for(auto s : Q[d]) {
               auto h = s.index();
               FOR(m, MOVES) if(move_length(m) == 2 && s.allow_move(m)){
                 s.move(m);
                 add(h,m, s, d+2);
                 s.move(move_opposite(m));
               }
             }
           }
           debug(total);
         });
    }
  };
  
}
