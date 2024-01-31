#include "header.hpp"
#include "rubik.hpp"
#include "rubik_tables.hpp"
#include "rubik_cycles.hpp"
#include "util.hpp"

namespace rubik{

  small_array corners_cycles[CORNERS_CYCLES_COUNT];
  small_array edges1_cycles[EDGES1_CYCLES_COUNT];
  small_array edges2_cycles[EDGES2_CYCLES_COUNT];
  small_array centers2_cycles[CENTERS2_CYCLES_COUNT];
  small_array centers3_cycles[CENTERS3_CYCLES_COUNT];
  small_array centers4_cycles[CENTERS4_CYCLES_COUNT];
 
  void init_cycles(){
    static bool inited = false;
    if(inited) return;
    inited = true;

    try_load_table_copy
      (vector<tuple<string, char*, size_t>>
       {make_tuple("data/corners_cycles", (char*)corners_cycles, sizeof(corners_cycles)),
        make_tuple("data/edges1_cycles", (char*)edges1_cycles, sizeof(edges1_cycles)),
        make_tuple("data/edges2_cycles", (char*)edges2_cycles, sizeof(edges2_cycles)),
        make_tuple("data/centers2_cycles", (char*)centers2_cycles, sizeof(centers2_cycles)),
        make_tuple("data/centers3_cycles", (char*)centers3_cycles, sizeof(centers3_cycles)),
        make_tuple("data/centers4_cycles", (char*)centers4_cycles, sizeof(centers4_cycles)),
       },
       [&]()
       {
           
         rubik c(7); c.reset();

         set<move_seq> M;
         FOR(axis,3) FOR(i,7) FOR(x,3) {
           move_seq a(7);
           a.move(axis,i,1+x);
           M.insert(a);
         }

         { set<move_seq> M2 = M;
           for(auto const& a : M) for(auto const& b : M) {
               auto a_ = a; a_.reverse();
               auto b_ = b; b_.reverse();
               move_seq c(7); c.move(a); c.move(b); c.move(a_);
               M2.insert(c);
             }
           // for(auto const& a : M) for(auto const& b : M) {
           //     auto a_ = a; a_.reverse();
           //     auto b_ = b; b_.reverse();
           //     move_seq c(7); c.move(a); c.move(b); c.move(a_); c.move(b_);
           //     M2.insert(c);
           //   }
           for(auto const& a : M) for(auto const& b : M) {
               move_seq c(7); c.move(a); c.move(b); 
               M2.insert(c);
             }

           M = M2;
         }
         debug(M.size());
    
         set<small_array> corners_cycles_set;
         set<small_array> edges1_cycles_set;
         set<small_array> edges2_cycles_set;
         set<small_array> centers2_cycles_set;
         set<small_array> centers3_cycles_set;
         set<small_array> centers4_cycles_set;

         i64 iter = 0;
         auto process = [&](move_seq const& a){
           iter++;
           if(iter % 1'000'000 == 0) debug(iter);
           rubik c(7); c.reset();
           c.move(a);
           i32 count_solved = 0;
           if(c.corners.is_solved()) count_solved += 1;
           if(c.edges1.is_solved()) count_solved += 1;
           FOR(x,2) if(c.edges2[x].is_solved()) count_solved += 1;
           if(c.centers1.is_solved()) count_solved += 1;
           FOR(x,2) if(c.centers2[x].is_solved()) count_solved += 1;
           FOR(x,2) if(c.centers3[x].is_solved()) count_solved += 1;
           FOR(x,2) FOR(y,2) if(x != y) if(c.centers4[x][y].is_solved()) count_solved += 1;

           if(count_solved == 10) { // cycle
             // corners
             if(!c.corners.is_solved()) {
               small_array moves;
               for(auto const& am : a.moves) {
                 FOR(m, corners::MOVES) {
                   if(am.axis == corners::move_axis(m) 
                      && am.p[corners::move_real_index(7,m)] == 1+(m%3)) {
                     moves.push(m);
                   }
                 }
               }
               corners_cycles_set.insert(moves);
             }
           }

           if(count_solved == 10) {
             // edges1
             if(!c.edges1.is_solved()) {
               small_array moves;
               for(auto const& am : a.moves) {
                 FOR(m, edges1::MOVES) {
                   if(am.axis == edges1::move_axis(m) 
                      && am.p[edges1::move_real_index(7,m)] == 1+(m%3)) {
                     moves.push(m);
                   }
                 }
               }
               edges1_cycles_set.insert(moves);
             }

             // edges2
             if(!c.edges2[0].is_solved()) {
               small_array moves;
               for(auto const& am : a.moves) {
                 FOR(m, edges2::MOVES) {
                   if(am.axis == edges2::move_axis(m) 
                      && am.p[edges2::move_real_index(7,0,m)] == 1+(m%3)) {
                     moves.push(m);
                   }
                 }
               }
               edges2_cycles_set.insert(moves);
             }
           }

           if(count_solved == 10) {
             // centers2
             if(!c.centers2[0].is_solved()) {
               small_array moves;
               for(auto const& am : a.moves) {
                 FOR(m, centers2::MOVES) {
                   if(am.axis == centers2::move_axis(m) 
                      && am.p[centers2::move_real_index(7,0,m)] == 1+(m%3)) {
                     moves.push(m);
                   }
                 }
               }
               centers2_cycles_set.insert(moves);
             }

             // centers3
             if(!c.centers3[0].is_solved()) {
               small_array moves;
               for(auto const& am : a.moves) {
                 FOR(m, centers3::MOVES) {
                   if(am.axis == centers3::move_axis(m) 
                      && am.p[centers3::move_real_index(7,0,m)] == 1+(m%3)) {
                     moves.push(m);
                   }
                 }
               }
               centers3_cycles_set.insert(moves);
             }
        
             if(!c.centers4[0][1].is_solved()) {
               small_array moves;
               for(auto const& am : a.moves) {
                 FOR(m, centers4::MOVES) {
                   if(am.axis == centers4::move_axis(m) 
                      && am.p[centers4::move_real_index(7,0,1,m)] == 1+(m%3)) {
                     moves.push(m);
                   }
                 }
               }
               centers4_cycles_set.insert(moves);
             }
           }
         };

         { for(auto const& a : M) process(a);
           for(auto const& a : M) for(auto const& b : M) {
               auto a_ = a; a_.reverse();
               auto b_ = b; b_.reverse();
               move_seq c(7); c.move(a); c.move(b); c.move(a_); c.move(b_);
               process(c);
             }
         }

         debug(corners_cycles_set.size());
         debug(edges1_cycles_set.size());
         debug(edges2_cycles_set.size());
         debug(centers2_cycles_set.size());
         debug(centers3_cycles_set.size());
         debug(centers4_cycles_set.size());

         runtime_assert(corners_cycles_set.size() == CORNERS_CYCLES_COUNT);
         runtime_assert(edges1_cycles_set.size() == EDGES1_CYCLES_COUNT);
         runtime_assert(edges2_cycles_set.size() == EDGES2_CYCLES_COUNT);
         runtime_assert(centers2_cycles_set.size() == CENTERS2_CYCLES_COUNT);
         runtime_assert(centers3_cycles_set.size() == CENTERS3_CYCLES_COUNT);
         runtime_assert(centers4_cycles_set.size() == CENTERS4_CYCLES_COUNT);

         copy(all(corners_cycles_set), corners_cycles);
         copy(all(edges1_cycles_set), edges1_cycles);
         copy(all(edges2_cycles_set), edges2_cycles);
         copy(all(centers2_cycles_set), centers2_cycles);
         copy(all(centers3_cycles_set), centers3_cycles);
         copy(all(centers4_cycles_set), centers4_cycles);
       });
    runtime_assert(corners_cycles[0].sz != 0);
  }

  template<class F, class G, class K>
  void solve_cycles_insertions(size_t ncycles, small_array *cycles,
                               i32 num_wildcards, i32 typ, vector<i32> const& tgt, 
                               move_seq &S0, rubik &c0, F&& f, G&& g, K&& k) {
    i32 N = c0.N;

    // auto move_R = [&](rubik& c, i32 m) {
    //   i32 axis, index, dir; tie(axis, index, dir) = g(m);
    //   c.move(axis, index, dir);
    // };
    // auto move_MS = [&](move_seq& S, i32 m) {
    //   i32 axis, index, dir; tie(axis, index, dir) = g(m);
    //   S.move(axis, index, dir);
    // };
    auto move_FMS = [&](fast_move_seq& S, i32 m) {
      i32 axis, index, dir; tie(axis, index, dir) = g(m);
      S.move(axis, index, dir);
    };

    i32 maxs = f(k(c0),k(rubik(N)));
    vector<tuple<move_seq, rubik>> BEAM(maxs+1, make_tuple(move_seq(N), rubik(N)));
    vector<i32> BEAM_best(maxs+1, 1e9);
    vector<i32> BEAM_count(maxs+1, 0);
    BEAM_count[maxs] = 1;
    BEAM[maxs] = mt(S0, c0);
    timer T;
    
    FORD(s, maxs, 0) if(BEAM_count[s]) {
      auto Sc1 = BEAM[s];
      auto S1 = get<0>(Sc1);
      auto c1 = get<1>(Sc1);
      debug(s, T.elapsed(), S1.length);
      if(s == 0) break;
        
      rubik    c2(N);
      move_seq S2(N);
      auto lastd1 = k(c1), lastd2 = k(c2);
 
      while(!S1.moves.empty()) {
        auto m = S1.moves.back();
        FOR(i,N) if(m.p[i]) {
          c1.move(m.axis, i, (4-m.p[i]));
          S1.move(m.axis, i, (4-m.p[i]));
          c2.move(m.axis, i, (4-m.p[i]));
          S2.move(m.axis, i, (4-m.p[i]));
        }
        
        auto d1 = k(c1);
        auto d2 = k(c2);

        if(d1 == lastd1 && d2 == lastd2) continue;
        lastd1 = d1;
        lastd2 = d2;

#pragma omp parallel
        {

          auto e1 = d1, e2 = d2;
          fast_move_seq T1(S1.N, S1.moves.size() + 64);
          T1.move(S1);
          fast_move_seq T2(S2.N, S2.moves.size() + 64);
          T2.move(S2);
          
#pragma omp for
          FOR(icycle, ncycles) {
            auto const& cycle = cycles[icycle];

            i32 ncycle = cycle.sz;
            auto e1_ = e1;
            FOR(i,ncycle) e1_.move(cycle.data[i]);
            i32 ds = f(e1_,e2);
            FORD(i,ncycle-1,0) {
              move_FMS(T2, move_opposite(cycle.data[i]));
            }
            FOR(i,ncycle) {
              move_FMS(T1, cycle.data[i]);
            }
            i32 dz = T1.length + T2.length - ncycle;
              
            if(ds < s && dz <= BEAM_best[ds]) {
#pragma omp critical
              {
                if(dz < BEAM_best[ds]) {
                  BEAM_best[ds] = dz;
                  BEAM_count[ds] = 0;
                }
                if(dz == BEAM_best[ds]) {
                  BEAM_count[ds]++;
                  if(rng.random32(BEAM_count[ds]) == 0) {
                    auto BS1 = S1;
                    auto bc1 = c1;
                    auto BS2 = T2.to_move_seq();

                    while(!BS2.moves.empty()) {
                      auto m = BS2.moves.back(); BS2.moves.pop_back();
                      FOR(i,N) if(m.p[i]) {
                        bc1.move(m.axis, i, (4-m.p[i]));
                        BS1.move(m.axis, i, (4-m.p[i]));
                      }
                    }

                    BEAM[ds] = mt(BS1,bc1);
                  }
                }
              }
            }
            FOR(i,ncycle) {
              move_FMS(T2, cycle.data[i]);
            }
            FORD(i,ncycle-1,0) {
              move_FMS(T1, move_opposite(cycle.data[i]));
            }
          }
        }

      }
    }

    i32 best = 1e9;
    vector<tuple<move_seq, rubik>> idx;
    FOR(s, maxs+1) if(BEAM_count[s]) {
      auto [S,c] = BEAM[s];
      if(s == 0 || c.num_mismatches(typ, rubik(N)) <= num_wildcards) {
        auto len = S.length;
        if(len < best) {
          best = len;
          idx.clear();
        }
        if(len == best) {
          idx.eb(S,c);
        }
      }
    }
    runtime_assert(!idx.empty());
    tie(S0,c0) = rng.sample(idx);
  }

  template<class F, class G, class K>
  void solve_cycles_insertions_tries(i32 ntries, size_t ncycles, small_array *cycles,
                                     i32 num_wildcards, i32 typ, vector<i32> const& tgt,
                                     move_seq &S1, rubik &c1, F&& f, G&& g, K&& k) {
    auto N = c1.N;
    move_seq bestT(N);
    rubik    bestD(N);

    FOR(itry, ntries) {
      auto T = S1;
      auto d = c1;
      solve_cycles_insertions(ncycles, cycles, num_wildcards, typ, tgt, T, d, f,g,k);
      if(bestT.length == 0 || T.length < bestT.length) {
        bestT = T;
        bestD = d;
      }
    }

    S1 = bestT;
    c1 = bestD;
  }
  
  const i32 NTRIES    = 3;

  void solve_cycles_edges2(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c){
    debug("solve_cycles_edges2");
    FOR(x,c.NHALF) {
      if(c.num_mismatches(typ, rubik(c.N)) <= num_wildcards) continue;
      debug(x, S.length, c.num_mismatches(typ, rubik(c.N)));
      solve_cycles_insertions_tries
        (NTRIES, EDGES2_CYCLES_COUNT, edges2_cycles,
         num_wildcards, typ, tgt,
         S, c,
         [&](edges2 const& a, edges2 const& b){
           return a.cycle_distance(typ, b);
         }, [&](i32 m) -> tuple<i32,i32,i32> {
          return make_tuple(edges2::move_axis(m), edges2::move_real_index(c.N, x, m), 1+m%3);
        }, [&](rubik const& c) {
          return c.edges2[x];
        });
    }
  }

  void solve_cycles_centers2(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c){
    debug("solve_cycles_centers2");
    if(c.N&1) FOR(x,c.NHALF) {
        if(c.num_mismatches(typ, rubik(c.N)) <= num_wildcards) continue;
        debug(x, S.length, c.num_mismatches(typ, rubik(c.N)));
        solve_cycles_insertions_tries
          (NTRIES, CENTERS2_CYCLES_COUNT, centers2_cycles,
           num_wildcards, typ, tgt,
           S, c,
           [&](centers2 const& a, centers2 const& b){
             return a.cycle_distance(typ, b);
           }, [&](i32 m) -> tuple<i32,i32,i32> {
            return make_tuple(centers2::move_axis(m), centers2::move_real_index(c.N, x, m), 1+m%3);
          }, [&](rubik const& c) {
            return c.centers2[x];
          });
      }
  }

  void solve_cycles_centers3(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c){
    debug("solve_cycles_centers3");
    FOR(x,c.NHALF) {
      if(c.num_mismatches(typ, rubik(c.N)) <= num_wildcards) continue;
      debug(x, S.length, c.num_mismatches(typ, rubik(c.N)));
      solve_cycles_insertions_tries
        (NTRIES, CENTERS3_CYCLES_COUNT, centers3_cycles, 
         num_wildcards, typ, tgt,
         S, c,
         [&](centers3 const& a, centers3 const& b){
           return a.cycle_distance(typ, b);
         }, [&](i32 m) -> tuple<i32,i32,i32> {
          return make_tuple(centers3::move_axis(m), centers3::move_real_index(c.N, x, m), 1+m%3);
        }, [&](rubik const& c) {
          return c.centers3[x];
        });
    }
  }

  void solve_cycles_centers4(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c){
    debug("solve_cycles_centers4");
    FOR(x,c.NHALF) FOR(y,c.NHALF) if(x != y) {
      if(c.num_mismatches(typ, rubik(c.N)) <= num_wildcards) continue;
      debug(x,y, S.length, c.num_mismatches(typ, rubik(c.N)));
      solve_cycles_insertions_tries
        (NTRIES, CENTERS4_CYCLES_COUNT, centers4_cycles, 
         num_wildcards, typ, tgt,
         S, c,
         [&](centers4 const& a, centers4 const& b){
           return a.cycle_distance(typ, b);
         }, [&](i32 m) -> tuple<i32,i32,i32> {
          return make_tuple(centers4::move_axis(m), centers4::move_real_index(c.N, x, y, m), 1+m%3);
        }, [&](rubik const& c) {
          return c.centers4[x][y];
        });
    }
  }
  
  
}
