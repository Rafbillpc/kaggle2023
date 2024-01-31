#include "optimal_333.hpp"
#include "puzzle_info.hpp"
#include "rubik.hpp"
#include "rubik_222.hpp"
#include "kociemba_phase1.hpp"
#include "kociemba_phase2.hpp"
#include "rubik_444_phase1.hpp"
#include "rubik_444_phase2.hpp"
#include "rubik_444_phase3.hpp"
#include "rubik_555_phase1.hpp"
#include "rubik_555_phase2.hpp"
#include "rubik_555_phase3.hpp"
#include "rubik_555_phase4.hpp"
#include "rubik_555_phase5.hpp"
#include "rubik_cycles.hpp"

namespace rubik {

  const i32 WIDTH = 50'000;
  
  move_seq solve_222(rubik const& c) {
    move_seq S(c.N);
    rubik d = c;
    solve_corners(S, d);
    d.print();
    return S;
  }

  move_seq solve_333(rubik const& c, i32 count){
    move_seq S(c.N);
    rubik d = c;
    solve_centers1(S, d);
    solve_optimal_333(S, d);
    d.print();
    // auto R = solve_kociemba_phase1_many(S,d, count);
    // move_seq T(c.N);
    // for(auto [T0,d0] : R) {
    //   solve_kociemba_phase2(T0,d0);
    //   if(T.length == 0 || T0.length < T.length) {
    //     T = T0;
    //     debug(T.length);
    //   }
    // }
    return S;
  }

  void solve_333_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width){
    solve_kociemba_phase1_beam(states, width);
    solve_kociemba_phase2_beam(states, width);
  }
  
  void reduce_444_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    solve_phase1_444_beam(states, width);
    solve_phase2_444_beam(states, width);
    solve_phase3_444_beam(states, width);
  }

  void reduce_444(move_seq& S, rubik& d) {
    solve_phase1_444(S, d, 0);
    solve_phase2_444(S, d, 0);
    solve_phase3_444(S, d, 0);
  }


  void reduce_444_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    solve_phase2_444_edges_beam(states, width);
    solve_phase3_444_edges_beam(states, width);
  }

  void reduce_444_edges(move_seq& S, rubik& d) {
    solve_phase2_444_edges(S, d, 0);
    solve_phase3_444_edges(S, d, 0);
  }

  
  move_seq solve_444(rubik c){
    vector<tuple<u32,u32,move_seq,rubik>> states;
    states.eb(0,0,move_seq(c.N),c);
    reduce_444_beam(states, WIDTH);

    states.erase
      (remove_if(all(states),
                 [&](auto const& s) -> bool {
                   auto const& c = get<3>(s);
                   bool good = c.edges2[0].to_edges1().total_orientation() == 0 &&
                     c.corners.parity() == c.edges2[0].to_edges1().parity();
                   return !good;
                 }), 
       end(states));

    { vector<tuple<u32,u32,move_seq,rubik>> states3;
      FOR(idx, states.size()) {
        auto [_, __, S, c] = states[idx];

        rubik e(3);
        e.corners = c.corners;
        e.centers1.reset();
        e.edges1 = c.edges2[0].to_edges1();

        states3.eb(idx, S.length, move_seq(3), e);
      }

      solve_333_beam(states3, WIDTH);

      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states3) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          c.move(m.axis, 0, m.p[0]);
          c.move(m.axis, 3, m.p[2]);
          S.move(m.axis, 0, m.p[0]);
          S.move(m.axis, 3, m.p[2]);
        }
        nstates.eb(0,0, S, c);
      }
      states = nstates;
    }

    sort(all(states), [&](auto const& a, auto const& b) {
      return get<2>(a).length < get<2>(b).length;
    });

    runtime_assert(!states.empty());
    return get<2>(states[0]);
  }

  void reduce_555_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    solve_phase1_555_beam(states, width);
    solve_phase2_555_beam(states, width);
    solve_phase3_555_beam(states, width);
    solve_phase4_555_beam(states, width);
    solve_phase5_555_beam(states, width);
  }

  void reduce_555(move_seq& S, rubik& d) {
    solve_phase1_555(S, d, 0);
    solve_phase2_555(S, d, 0);
    runtime_assert(d.edges2[0].parity() == 0);
    solve_phase3_555(S, d, 0);
    runtime_assert(d.edges2[0].is_split_lowhigh());
    solve_phase4_555(S, d, 0);
    solve_phase5_555(S, d, 0);
  }
 
  void reduce_555_edges_beam(vector<tuple<u32,u32,move_seq,rubik>> &states, i32 width) {
    solve_phase2_555_edges_beam(states, width);
    solve_phase3_555_edges_beam(states, width);
    solve_phase4_555_edges_beam(states, width);
    solve_phase5_555_edges_beam(states, width);
  }

  void reduce_555_edges(move_seq& S, rubik& d) {
    solve_phase2_555_edges(S, d, 0);
    runtime_assert(d.edges2[0].parity() == 0);
    solve_phase3_555_edges(S, d, 0);
    runtime_assert(d.edges2[0].is_split_lowhigh());
    solve_phase4_555_edges(S, d, 0);
    solve_phase5_555_edges(S, d, 0);
  }
  
  move_seq solve_555(rubik c){
    vector<tuple<u32,u32,move_seq,rubik>> states;
    { move_seq S(c.N);
      solve_centers1(S, c);
      if(c.corners.parity() != c.edges1.parity()) return S;
      states.eb(0,0,S,c);
    }

    reduce_555_beam(states, WIDTH);

    { vector<tuple<u32,u32,move_seq,rubik>> states3;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        rubik e(3);
        e.corners = c.corners;
        e.centers1.reset();
        e.edges1 = c.edges1;

        states3.eb(idx, S.length, move_seq(3), e);
      }

      solve_333_beam(states3, WIDTH);

      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states3) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          c.move(m.axis, 0, m.p[0]);
          c.move(m.axis, 4, m.p[2]);
          S.move(m.axis, 0, m.p[0]);
          S.move(m.axis, 4, m.p[2]);
        }
        nstates.eb(0,0, S, c);
      }
      states = nstates;
    }

    sort(all(states), [&](auto const& a, auto const& b) {
      return get<2>(a).length < get<2>(b).length;
    });

    runtime_assert(!states.empty());
    return get<2>(states[0]);
  }

  move_seq solve_large(i32 typ, i32 num_wildcards, vector<i32> const& tgt, rubik c, bool& success){
    vector<tuple<u32,u32,move_seq,rubik>> states;
    if(c.N&1) {
      move_seq S(c.N);
      solve_centers1(S, c);
      
      if(c.corners.parity() != c.edges1.parity()) {
        success = false;
        return S;
      }
      states.eb(0,0,S,c);
    }else{
      states.eb(0,0,move_seq(c.N),c);
    }

    if(c.N%2 == 0) {
      vector<tuple<u32,u32,move_seq,rubik>> states4;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        rubik e(4);
        e.corners = c.corners;
        e.edges2[0] = c.edges2[0];
        e.centers3[0] = c.centers3[0];

        states4.eb(idx, S.length, move_seq(4), e);
      }

      reduce_444_beam(states4, WIDTH);
      states4.erase
        (remove_if(all(states4),
                   [&](auto const& s) -> bool {
                     auto const& c = get<3>(s);
                     bool good = c.edges2[0].to_edges1().total_orientation() == 0 &&
                       c.corners.parity() == c.edges2[0].to_edges1().parity();
                     return !good;
                   }), 
         end(states4));


      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states4) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          i32 table[4] = {0,1,c.N-2,c.N-1};
          FOR(j,4) if(m.p[j]) {
            S.move(m.axis, table[j], m.p[j]);
            c.move(m.axis, table[j], m.p[j]);
          }
        }
        nstates.eb(0, 0, S, c);
      }
      states = nstates;
    }
    
    FORU(i, c.IS_ODD ? 0 : 1, c.NHALF-1) {
      debug("iter", i);

      vector<tuple<u32,u32,move_seq,rubik>> states5;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        auto last = i == 0 ? c.edges1 : c.edges2[0].to_edges1();
        
        rubik e(5);
        e.centers1 = c.centers1;
        e.edges1 = last;
        e.edges2[0] = c.edges2[i];
        e.centers2[0] = c.centers2[i];
        e.centers3[0] = c.centers3[i];
      
        states5.eb(idx, S.length, move_seq(5), e);
      }

      reduce_555_beam(states5, WIDTH);

      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states5) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          i32 table[5] = {0,(i32)(1+i),-1,(i32)(c.N-2-i),c.N-1};
          FOR(j,5) if(m.p[j]) {
            S.move(m.axis, table[j], m.p[j]);
            c.move(m.axis, table[j], m.p[j]);
          }
        }
        nstates.eb(0,0, S, c);
      }
      states = nstates;
    }

    { vector<tuple<u32,u32,move_seq,rubik>> states3;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        rubik e(3);
        e.corners = c.corners;
        e.centers1.reset();
        e.edges1 = c.IS_ODD ? c.edges1 : c.edges2[0].to_edges1();

        runtime_assert(e.edges1.total_orientation() == 0);
        runtime_assert(e.corners.parity() == e.edges1.parity());

        states3.eb(idx, S.length, move_seq(3), e);
      }

      solve_333_beam(states3, WIDTH);

      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states3) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          c.move(m.axis, 0, m.p[0]);
          c.move(m.axis, c.N-1, m.p[2]);
          S.move(m.axis, 0, m.p[0]);
          S.move(m.axis, c.N-1, m.p[2]);
        }
        nstates.eb(0,0, S, c);
      }
      states = nstates;
    }
    
    sort(all(states), [&](auto const& a, auto const& b) -> bool {
      return get<2>(a).length + 2 * get<3>(a).centers4_distance(typ) <
        get<2>(b).length + 2 * get<3>(b).centers4_distance(typ);
    });

    auto S = get<2>(states[0]);
    auto d = get<3>(states[0]);
    debug(S.length, d.centers4_distance(typ));
    d.print();
    
    init_cycles();
    solve_cycles_centers4(typ, num_wildcards, tgt, S, d);
    d.print();

    success = true;
    return S;
  }
  
  move_seq solve_cycles(i32 typ, i32 num_wildcards, vector<i32> const& tgt,
                        rubik c, bool& success) {
    vector<tuple<u32,u32,move_seq,rubik>> states;
    if(c.N&1) {
      move_seq S(c.N);
      solve_centers1(S, c);
      
      if(c.corners.parity() != c.edges1.parity()) {
        success = false;
        return S;
      }
      states.eb(0,0,S,c);
    }else{
      states.eb(0,0,move_seq(c.N),c);
    }

    if(c.N%2 == 0) {
      vector<tuple<u32,u32,move_seq,rubik>> states4;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        rubik e(4);
        e.corners = c.corners;
        e.edges2[0] = c.edges2[0];
        e.centers3[0] = c.centers3[0];

        states4.eb(idx, S.length, move_seq(4), e);
      }

      reduce_444_edges_beam(states4, WIDTH);
      states4.erase
        (remove_if(all(states4),
                   [&](auto const& s) -> bool {
                     auto const& c = get<3>(s);
                     bool good = c.edges2[0].to_edges1().total_orientation() == 0 &&
                       c.corners.parity() == c.edges2[0].to_edges1().parity();
                     return !good;
                   }), 
         end(states4));


      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states4) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          i32 table[4] = {0,1,c.N-2,c.N-1};
          FOR(j,4) if(m.p[j]) {
            S.move(m.axis, table[j], m.p[j]);
            c.move(m.axis, table[j], m.p[j]);
          }
        }
        nstates.eb(0, 0, S, c);
      }
      states = nstates;
    }
    
    FORU(i, c.IS_ODD ? 0 : 1, c.NHALF-1) {
      debug("iter", i);

      vector<tuple<u32,u32,move_seq,rubik>> states5;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        auto last = i == 0 ? c.edges1 : c.edges2[0].to_edges1();
        
        rubik e(5);
        e.centers1 = c.centers1;
        e.edges1 = last;
        e.edges2[0] = c.edges2[i];
        e.centers2[0] = c.centers2[i];
        e.centers3[0] = c.centers3[i];
      
        states5.eb(idx, S.length, move_seq(5), e);
      }

      reduce_555_edges_beam(states5, WIDTH);

      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states5) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          i32 table[5] = {0,(i32)(1+i),-1,(i32)(c.N-2-i),c.N-1};
          FOR(j,5) if(m.p[j]) {
            S.move(m.axis, table[j], m.p[j]);
            c.move(m.axis, table[j], m.p[j]);
          }
        }
        nstates.eb(0,0, S, c);
      }
      states = nstates;
    }

    { vector<tuple<u32,u32,move_seq,rubik>> states3;
      FOR(idx, states.size()) {
        auto [_,__, S, c] = states[idx];

        rubik e(3);
        e.corners = c.corners;
        e.centers1.reset();
        e.edges1 = c.IS_ODD ? c.edges1 : c.edges2[0].to_edges1();

        runtime_assert(e.edges1.total_orientation() == 0);
        runtime_assert(e.corners.parity() == e.edges1.parity());

        states3.eb(idx, S.length, move_seq(3), e);
      }

      solve_333_beam(states3, WIDTH);

      vector<tuple<u32,u32,move_seq,rubik>> nstates;
      for(auto const& [idx,len, M, d] : states3) {
        auto [_,__, S, c] = states[idx];
        for(auto m : M.moves) {
          c.move(m.axis, 0, m.p[0]);
          c.move(m.axis, c.N-1, m.p[2]);
          S.move(m.axis, 0, m.p[0]);
          S.move(m.axis, c.N-1, m.p[2]);
        }
        nstates.eb(0,0, S, c);
      }
      states = nstates;
    }
    
    sort(all(states), [&](auto const& a, auto const& b) -> bool {
      return get<2>(a).length + 2 * get<3>(a).centers4_distance(typ) <
        get<2>(b).length + 2 * get<3>(b).centers4_distance(typ);
    });

    auto S = get<2>(states[0]);
    auto d = get<3>(states[0]);
   
    init_cycles();
    
    solve_cycles_edges2(typ, num_wildcards, tgt, S, d);
    solve_cycles_centers4(typ, num_wildcards, tgt, S, d);
    solve_cycles_centers2(typ, num_wildcards, tgt, S, d);
    solve_cycles_centers3(typ, num_wildcards, tgt, S, d);

    success = true;
    d.print();
    debug(S.length);
    return S;
  }
  
  move_seq solve(i32 typ, i32 num_wildcards, vector<i32> const& tgt, rubik const& c) {
    debug(typ);
    if(c.N == 2) {
      return solve_222(c);
    }
    if(c.N == 3) {
      return solve_333(c, 1'000'000);
    }

    if(typ == 0 || typ == 1) {
      if(c.N == 4) {
        return solve_444(c);
      }
      if(c.N == 5) {
        return solve_555(c);
      }

      move_seq S(c.N);
      bool success = false;
      do {
        S = solve_large(typ, num_wildcards, tgt, c, success);
      } while(!success);
      return S;
    }else{
      debug("here");
      move_seq S(c.N);
      bool success = false;
      do {
        S = solve_cycles(typ, num_wildcards, tgt, c, success);
      } while(!success);
      return S;
    }
    
  }
}
