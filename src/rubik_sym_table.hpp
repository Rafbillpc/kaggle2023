#pragma once
#include "header.hpp"
#include "rubik.hpp"
#include "util.hpp"
#include <compare>

namespace rubik {

  template<class state>
  struct sym_table : with_moves<state::MOVES_PER_AXIS> {
    static const i32 MOVES_PER_AXIS = state::MOVES_PER_AXIS;
    static const i32 MOVES = MOVES_PER_AXIS * 3 * 3;
    static const u64 NRAW = state::NRAW;
    static const u64 NSYM = state::NSYM;

    u32* sym_to_raw = 0;
    u32* raw_to_sym = 0;
    u8*  raw_sym_which = 0;
    u64* sym_moves = 0;

    u8 sym_move[8][MOVES];
    u8 sym_mult[8][8];

    u32 raw_to_sym_full(u32 x) {
      runtime_assert(raw_to_sym[x] != (u32)(-1));
      return raw_to_sym[x] * 8 + __builtin_ctz(raw_sym_which[x]);
    }
    
    u32 move_sym(u32 x, u32 m) {
      auto x1   = x / 8;
      auto sym1 = x % 8;
      auto m_ = sym_move[sym1][m];
      auto x2 = sym_moves[x1 * MOVES + m_] / 8;
      auto sym2 = sym_moves[x1 * MOVES + m_] % 8;
      return x2 * 8 + sym_mult[sym1][sym2];
    }

    void init_sym(){
      FOR(sym, 8) FOR(m1, MOVES) {
        sym_move[sym][m1] = 255;
      }
      FOR(sym, 8) FOR(m1, MOVES) {
        state s1, s2; s1.reset(); s2.reset();
        s1.sym(sym_inv[sym]);
        if(s1.allow_move(m1)) {
          s1.move(m1);
          s1.sym(sym);
          i32 cnt = 0;
          FOR(m2,MOVES) if(s2.allow_move(m2)) {
            s2.move(m2);
            if(s1 == s2) {
              // If triggered: check that state has a comparator
              runtime_assert(sym_move[sym][m1] == 255);
              sym_move[sym][m1] = m2;
              cnt += 1;
            }
            s2.move(move_opposite(m2));
          }
          runtime_assert(cnt == 1);
        }
      }
      FOR(sym1, 8) FOR(sym2, 8) {
        i32 cnt = 0;
        FOR(sym3, 8) {
          state s1, s2; s1.reset(); s2.reset();
          s1.sym(sym1);
          s1.sym(sym2);
          s2.sym(sym3);
          if(s1 == s2) {
            // debug(sym1, sym2, sym3);
            sym_mult[sym1][sym2] = sym3;
            cnt += 1;
          }
        }
        runtime_assert(cnt == 1);
      }
    }
    
    void init(string name) {
      init_sym();
      // raw_to_sym    = new u32[NRAW];
      // sym_to_raw    = new u32[NSYM*8];
      // raw_sym_which = new u8[NRAW];
      // sym_moves     = new u64[NSYM*MOVES];
 
      try_load_table
        (vector<tuple<string, char **, size_t>>
         {make_tuple("data/" + name + "_raw_to_sym", (char**)&raw_to_sym, NRAW * sizeof(u32)),
          make_tuple("data/" + name + "_sym_to_raw", (char**)&sym_to_raw, NSYM*8 * sizeof(u32)),
          make_tuple("data/" + name + "_raw_sym_which", (char**)&raw_sym_which, NRAW * sizeof(u8)),
          make_tuple("data/" + name + "_sym_moves", (char**)&sym_moves, NSYM*MOVES * sizeof(u64)),},
         [&]()
         {
           FOR(i, NRAW) raw_to_sym[i] = (u32)(-1);
           FOR(i, NRAW) raw_sym_which[i] = 0;
           FOR(i, NSYM*MOVES) sym_moves[i] = -1;
           u64 total_raw = 0, total_sym = 0;

           vector<state> Q[255];
           auto add = [&](u64 sh, i32 m, state const& c, i32 dist) {
             auto h = c.index();
             if(raw_to_sym[h] != (u32)(-1)) {
               runtime_assert(raw_sym_which[h]);
               sym_moves[sh * MOVES + m] = raw_to_sym[h] * 8 + __builtin_ctz(raw_sym_which[h]);
               return;
             }
             sym_moves[sh * MOVES + m] = total_sym * 8;
             FOR(s, 8) {
               auto d = c;
               d.sym_conj(s);
               auto hd = d.index();
               raw_sym_which[hd] |= (1<<s);
               sym_to_raw[total_sym*8 + s] = hd;
               if(raw_to_sym[hd] != (u32)(-1)) continue;
               raw_to_sym[hd] = total_sym;
               total_raw += 1;
               if(total_raw % 1'000'000 == 0) debug(total_raw, total_sym);
             }
             total_sym += 1;
             Q[dist].eb(c);
             return;
           };
           { state s; s.reset(); add(0,0, s, 0); }

           FOR(d, 255) if(!Q[d].empty()) {
             for(auto s : Q[d]) {
               auto h = raw_to_sym[s.index()];
               FOR(m, MOVES) if(move_length(m) == 1 && s.allow_move(m)){
                 s.move(m);
                 add(h,m, s, d+1);
                 s.move(move_opposite(m));
               }
             }
             for(auto s : Q[d]) {
               auto h = raw_to_sym[s.index()];
               FOR(m, MOVES) if(move_length(m) == 2 && s.allow_move(m)){
                 s.move(m);
                 add(h,m, s, d+2);
                 s.move(move_opposite(m));
               }
             }
           }
           debug(total_raw, total_sym);
         });
    }
  };
  
}
