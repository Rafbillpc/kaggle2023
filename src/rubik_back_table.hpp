#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "util.hpp"
#include <compare>

namespace rubik {

  template<class state>
  struct back_table : with_moves<state::MOVES_PER_AXIS> {
    static const i32 MOVES_PER_AXIS = state::MOVES_PER_AXIS;
    static const i32 MOVES = MOVES_PER_AXIS * 3 * 3;
    static const i64 DEPTH = state::DEPTH;
    static const u64 HASHSIZE = state::HASHSIZE;

    u32* hashset  = 0;
    u8*  distance = 0;

    bool hashset_insert(u64 k, u8 d) {
      u64 h = uint64_hash::hash_int(1+k);
      u64 x = h % HASHSIZE;
      u32 y = h / HASHSIZE;
      if(y == 0) {
        debug(k, h, x, y);
        runtime_assert(y != 0);
      }
      while(hashset[x] != 0) {
        if(hashset[x] == y) return false;
        x = x+1;
        if(x == HASHSIZE) x = 0;
      }
      hashset[x] = y;
      distance[x] = d;
      return true;
    }
 
    u8 find_distance(u64 k) {
      u64 h = uint64_hash::hash_int(1+k);
      u64 x = h % HASHSIZE;
      u32 y = h / HASHSIZE;
      while(hashset[x] != 0) {
        if(hashset[x] == y) {
          return distance[x];
        }
        x = x+1;
        if(x == HASHSIZE) x = 0;
      }
      return 255;
    }

    
    void init(string name) {
      // hashset  = new u32[HASHSIZE];
      // distance = new u8[HASHSIZE];
 
      try_load_table
        (vector<tuple<string, char **, size_t>>
         {make_tuple("data/" + name + "_back_hashset", (char**)&hashset, HASHSIZE * sizeof(u32)),
          make_tuple("data/" + name + "_back_distance", (char**)&distance, HASHSIZE * sizeof(u8)),},
         [&]()
         {
           FOR(i, HASHSIZE) hashset[i]  = 0;
           FOR(i, HASHSIZE) distance[i] = 255;

           u64 total = 0;
           
           vector<state> Q[255];
           auto add = [&](state const& c, i32 dist) {
             u64 k = c.key();
             if(dist <= 1) debug(k);
             if(hashset_insert(k, dist)) {
               total += 1;
               if(total % 1'000'000 == 0) debug(total);
               if(dist < DEPTH) Q[dist].eb(c);
             }
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
           
           FOR(d, DEPTH) if(!Q[d].empty()) {
             debug(d, Q[d].size());
             if(d+1 <= DEPTH) for(auto s : Q[d]) {
               FOR(m, MOVES) if(move_length(m) == 1 && s.allow_move(m)){
                 auto c = s;
                 c.move(m);
                 add(c, d+1);
               }
             }
             if(d+2 <= DEPTH) for(auto s : Q[d]) {
               FOR(m, MOVES) if(move_length(m) == 2 && s.allow_move(m)){
                 auto c = s;
                 c.move(m);
                 add(c, d+2);
               }
             }
             Q[d].clear();
             Q[d].shrink_to_fit();
           }
           debug(total);

         });
    }
  };
  
}
