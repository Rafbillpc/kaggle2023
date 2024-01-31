#pragma once
#include "rubik.hpp"

namespace rubik {

  template<class state, class F>
  void search_callback(vector<tuple<u32, state>> const& states, F&& f) {
    bool stop = 0;
    vector<i32> R;

    u32 start_limit = 1e9;
    for(auto [cbase,c] : states) {
      runtime_assert(c.pruning_value(255) != 255);
      start_limit = min(start_limit, cbase + c.pruning_value(255));
    }

    
    for(u32 limit = start_limit;; ++limit){
      atomic<u64> total = 0;
      timer T;

#pragma omp parallel for schedule(dynamic)
      FOR(idx, (int)states.size()) {
        i32 moves[256];
        u32 cbase = get<0>(states[idx]);
        state c = get<1>(states[idx]);
        
        auto dfs = letrec([&](auto dfs, i32 idepth,
                              i32 depth, i32 limit,
                              i32 last_axis, i32 last_index) -> void
        {
          if(stop) return;
          total += 1;
          i32 cpruning = c.pruning_value(limit-depth);
          if(depth == limit && cpruning == 0) {
#pragma omp critical
            {
              R.assign(moves, moves+idepth);
              f(idx, stop, R);
            }
            return;
          }
          if(depth + cpruning > limit) {
            return;
          }
          FOR(m, state::MOVES) if(c.allow_move(m)) {
            i32 axis = state::move_axis(m), index = state::move_index(m);
            if(axis == last_axis && index <= last_index) continue;
            moves[idepth] = m;
            c.move(m);
            dfs(idepth+1, depth+move_length(m), limit, axis, index);
            c.move(move_opposite(m));
          }
        });

        if(limit >= cbase) {
          dfs(0, 0, limit-cbase, -1, -1);
        }
      }
      
      (void)total;
      debug(limit, total, T.elapsed(), stop);
      if(stop) break;
    }
  }

  template<class state>
  vector<i32> search1(state c) {
    vector<i32> R;
    search_callback(vector<tuple<u32, state>>{make_tuple(0,c)},
                    [&](u32 idx, bool& stop, vector<i32> const& T){
                      R = T;
                      stop = 1;
                    });
    return R;
  }

  template<class state>
  vector<vector<i32>> search_many(state c, i32 count) {
    vector<vector<i32>> R;
    search_callback(vector<tuple<u32, state>>{make_tuple(0,c)},
                    [&](u32 idx, bool& stop, vector<i32> const& T){
                      R.eb(T);
                      if((int)R.size() >= count) stop = 1;
                    });
    return R;
  }

  template<class state>
  vector<tuple<u32, vector<i32>>> search_beam(vector<tuple<u32, state>> const& states, i32 count) {
    vector<tuple<u32, vector<i32>>> R;
    i32 best = 1e9;
    search_callback(states,
                    [&](u32 idx, bool& stop, vector<i32> const& T){
                      best = min<i32>(best, T.size());
                      R.eb(idx, T);
                      if(R.size()%100 == 0) debug(R.size(), count);
                      if((int)R.size() >= count) stop = 1;
                    });
    return R;
  }
  
}
