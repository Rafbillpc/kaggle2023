#pragma once
#include "globe.hpp"
#include "header.hpp"
#include "util.hpp"

namespace globe {
  const u64 LIMIT = 200'000'000;
  
  template<i32 N>
  i32 solve_globe1(globe1<N> const& tgt, globe1<N>& c){
    i32 y = 0;
    while(1) {
      bool solved = 1;
      FOR(i,2*N) if(tgt.ring[i] != c.ring[i]) solved = 0;
      if(solved) break;
      c.rotate();
      y += 1;
    }
    return y;
  }

  template<i32 N>
  void double_swap(move_seq<N>& MS, globe2<N>& c, i32 at) {
    // [[1,1],[66,1],[0,1],[67,1],[66,65],[0,1],[67,65],[1,1]]
    i32 at1 = (at+1)%(2*N);
    vector<array<i32,2>> A =
      { {at1,    1},
        {2*N,    1},
        {at,     1},
        {2*N+1,  1},
        {2*N,    2*N-1},
        {at,     1},
        {2*N+1,  2*N-1},
        {at1,    1}
      };
    for(auto p : A) {
      MS.move(p[0],p[1]);
      c.move(p[0],p[1]);
    }
  }

  static inline
  i32 get_start_limit(i32 N) {
    return 10;
  }
 
  template<i32 N>
  vector<array<i32,2>> solve_globe2(i32 num_wildcards,
                                    vector<array<i32,2>> MS0, globe2<N> tgt, globe2<N> c0, bool& success){
    bool is_hard = N==33 && *max_element(tgt.data, tgt.data+4*N) == 4*N-1;
    i32 dist[4*N][4*N];

    FOR(i,4*N) FOR(j,4*N) {
      i32 d = abs(i%(2*N) - j%(2*N));
      d = min(d, 2*N-d);
      if(i/(2*N) != j/(2*N)) d += 2;
      dist[i][j] = d;
    }
    
    auto score = [&](globe2<N> const& c, globe2<N> const& d) -> i32 {
      i32 v = 0;
      if(is_hard) {
        FOR(i,4*N) {
          if((c.data[i]/2) != (d.data[i]/2)) {
            v += 1;
          }
        }
      }else{
        FOR(i,4*N) if(c.data[i] != d.data[i]) v += 1;
      }
      return v;
    };
    
    i32 maxs = score(c0, tgt);
    vector<tuple<vector<array<i32,2>>, globe2<N>>> BEAM(maxs+1);
    vector<i32> BEAM_best(maxs+1, 1e9);
    vector<i32> BEAM_count(maxs+1, 0);
    BEAM[maxs] = mt(MS0, c0);
    BEAM_count[maxs] = 1;
    timer T;

    FORD(s, maxs, 0) if(BEAM_count[s]) {
      auto Sc = BEAM[s];
      auto S = get<0>(Sc);
      auto c = get<1>(Sc);
      debug(T.elapsed(), s, S.size());
      if(s == 0) break;
      atomic<bool> found = false;
      atomic<u64> total = 0;
      auto limit = get_start_limit(N);
      auto test = [&](vector<array<i32,2>> X, vector<array<i32,2>> Y, globe2<N> c0, globe2<N> d){
        auto c = c0;
        i32 na = 0, nb = 0;
        array<i32,2> A[32], B[32];

        auto dfs = letrec([&](auto dfs, i32 depth, i32 zdepth, i32 limit, i32 last, i32 allow) -> void {
          if(found && total > LIMIT) return;
          total += 1;
          if(total % 25'000'000 == 0) {
#pragma omp critical
            {
              debug(total);
            }
          }
          if(nb && last >= 2*N) {
            auto m = B[--nb];
            A[na++] = m;
            c.move(m[0],1);
            dfs(depth, zdepth, limit, last, allow);
            c.move(m[0],1);
            na -= 1;
            B[nb++] = m;
          }
          if(last != -1 && zdepth > 0 && !nb) {
            i32 ds = score(c,d);
            bool ok = 1;
            if(is_hard && ds == 0) {
              u64 p = 0;
              FOR(i,4*N) if(c.data[i] != d.data[i]) p += 1;
              ok = (p%4) == 0;
            }
            i32 dz = X.size() + Y.size() + na;
            if(ok && ds < s) {
              if(!found) {
#pragma omp critical
                {
                  debug(ds, dz);
                }
              }
              found = 1;
            }
            if(ok && ds < s && dz <= BEAM_best[ds]) {
#pragma omp critical
              {
                if(dz < BEAM_best[ds]) {
                  BEAM_best[ds] = dz;
                  BEAM_count[ds] = 0;
                }
                if(dz == BEAM_best[ds]) {
                  BEAM_count[ds] += 1;
                  if(rng.random32(BEAM_count[ds]) == 0) {
                    debug(ds, dz, vector<array<i32,2>>(A, A+na));

                    auto S1 = X;
                    auto c1 = c0;

                    auto BY = Y;
                    FORD(i,na-1,0) BY.eb(A[i]);

                    while(!BY.empty()) {
                      auto p = BY.back(); BY.pop_back();
                      c1.move(p);
                      S1.eb(p);
                    }

                    // debug(S.size(), zdepth, X.size(), Y.size(), S1.size(), dz, na);
                    runtime_assert((i32)S1.size() == dz);
                    
                    BEAM[ds] = mt(S1,c1);
                  }
                }
              }
            }
          }
          if(depth == limit) return;
          if(!Y.empty() && !nb) {
            auto p = Y.back();
            Y.pop_back();
            c.move(p);
            d.move(p);
            A[na++] = p;
            dfs(depth+1, zdepth, limit, -1, allow);
            na--;
            c.move_inv(p);
            d.move_inv(p);
            Y.eb(p);
          }
          if(allow) FOR(x,N) if(!nb || B[nb-1][0] != x) {
              A[na++] = {(i32)x,1};
              B[nb++] = {(i32)x,1};
              c.move(x,1);
              if(depth+2 <= limit) dfs(depth+2, zdepth+2, limit, x, allow-1);
              c.move(x,1);
              na--; nb--;
            }
          if(last != 2*N+1) {
            for(auto x : {1,2*N-1}) if(!na || A[na-1] != array<i32,2>{2*N,(i32)(2*N-x)}) {
                A[na++] = {2*N,(i32)x};
                c.move(2*N,x);
                dfs(depth+1, zdepth+1, limit, 2*N, allow);
                c.move(2*N,2*N-x);
                na--;
              }
          }
          for(auto x : {1,2*N-1}) if(!na || A[na-1] != array<i32,2>{2*N+1,(i32)(2*N-x)}) {
              A[na++] = {2*N+1,(i32)x};
              c.move(2*N+1,x);
              dfs(depth+1, zdepth+1, limit, 2*N+1, allow);
              c.move(2*N+1,2*N-x);
              na--;
            }
        });
        dfs(0,0,limit,-1,3);
      };

      vector<array<i32,2>> Y;
      auto d = tgt;
      vector<tuple<vector<array<i32,2>>, vector<array<i32,2>>, globe2<N>, globe2<N>>> V;
      V.eb(S,Y,c,d);
      while(!S.empty()) {
        auto p = S.back(); S.pop_back();
        c.move_inv(p);
        d.move_inv(p);
        Y.pb(p);
        V.eb(S,Y,c,d);
      }
      rng.shuffle(V);

      i32 cnt = 0;
      i32 nt = V.size();
#pragma omp parallel for num_threads(nt)
      FOR(iv, (int)V.size()) {
#pragma omp critical
        {
          cnt++;
        }
        auto [X,Y,c,d] = V[iv];
        test(X,Y,c,d);
      }
    }

    i32 best = 1e9;
    vector<tuple<vector<array<i32,2>>, globe2<N>>> idx;
    FOR(s, min(maxs+1, num_wildcards+1)) if(BEAM_count[s]) {
      i32 len = get<0>(BEAM[s]).size();
      if(len < best) {
          best = len;
          idx.clear();
      }
      if(len == best) {
        idx.eb(BEAM[s]);
      }
    }
    runtime_assert(!idx.empty());
    success = 1;
    return get<0>(rng.sample(idx));
  }

  template<i32 N>
  move_seq<N> merge_move_seq(i32 x0,i32 x1, move_seq<N> M1, move_seq<N> M2) {
    move_seq<N> OUT;
    vector<i32> stack;
    i32 i = 0;
    while(i < (int)M1.moves.size() && M1.moves[i][0] >= 2*N) {
      OUT.move(M1.moves[i][0], M1.moves[i][1]);
      i += 1;
    }
    
    for(auto const& m : M2.moves) {
      if(m[0] < 2*N) {
        OUT.move(m[0],1);
        if(!stack.empty() && m[0] == stack.back()) {
          stack.pop_back();
        }else if(stack.empty() && i < (int)M1.moves.size() && M1.moves[i][0] == m[0]) {
          i += 1;
          while(i < (int)M1.moves.size() && M1.moves[i][0] >= 2*N) {
            OUT.move(M1.moves[i][0], M1.moves[i][1]);
            i += 1;
          }
        }else{
          stack.push_back(m[0]);
        }
      }else if(m[0] == 2*N) {
        OUT.move(2*N+x0, m[1]);
      }else{
        OUT.move(2*N+x1, m[1]);
      }
    }
    debug(i, M1.moves.size(), stack.size(), OUT.moves.size());
    debug(OUT.moves);
    return OUT;
  }
  
  template<i32 N>
  move_seq<N> solve_globe2_hard(globe2<N> tgt, globe2<N> c){
    move_seq<N> M;
    vector<bool> x1(N), x2(N);
    FOR(i,N) x1[i] = c.ring1[2*i] > c.ring1[2*i+1];
    FOR(i,N) x2[i] = c.ring2[2*i] > c.ring2[2*i+1];
    vector<i32> i1, i2;
    FOR(i,N) if(x1[i]) i1.eb(i);
    FOR(i,N) if(x2[i]) i2.eb(i);
    runtime_assert((i1.size()+i2.size())%2 == 0);
    vector<vector<u8>> sw(N, vector<u8>(N,false));
    while(!i1.empty() && !i2.empty()) {
      sw[i1.back()][i2.back()] = 1;
      i1.pop_back();
      i2.pop_back();
    }
    while(!i1.empty()) {
      i32 a = i1.back(); i1.pop_back();
      i32 b = i1.back(); i1.pop_back();
      sw[a][0] ^= 1;
      sw[b][0] ^= 1;
    }
    while(!i2.empty()) {
      i32 a = i2.back(); i2.pop_back();
      i32 b = i2.back(); i2.pop_back();
      sw[0][a] ^= 1;
      sw[0][b] ^= 1;
    }
    FOR(i, N) {
      debug("step", i);
      c.print();
      FOR(a,N) if(sw[a][(a+N-i)%N]) {
        debug(a,(a+N-i)%N);
        double_swap(M,c,2*((a+N-i)%N));
        c.print();
      }
      M.move({2*N,2});
      c.move({2*N,2});
    }
    return M;
  }

  
  template<i32 N, i32 M>
  move_seq<N> solve_globe(i32 num_wildcards, globe<N,M> const& tgt, globe<N,M>& c){
    move_seq<N> MS;

    bool is_hard = N==33 && *max_element(tgt.g2[0].data, tgt.g2[0].data+4*N) == 4*N-1;

    move_seq<N> PREMS;
    
    i32 cnt = rng.random32(3);
    debug(cnt);
    FOR(k,cnt) {
      i32 x = rng.random32(2*N);
      PREMS.move(x, 1);
      c.move(x, 1);
    }

    c.print();
    tgt.print();
    
    if(M&1) {
      auto y = solve_globe1<N>(tgt.g1[0], c.g1[0]);
      if(y) MS.move(2*N+(M/2),y);
    }
    vector<i32> I;
    FOR(i,M/2) I.eb(i);
    rng.shuffle(I);
    bool first = true;
    for(int i : I) {
      move_seq<N> M1;
      for(auto const& m : MS.moves) if(m[0] < 2*N) {
          M1.moves.pb({m[0],m[1]});
        }

      debug(M1.moves);
      move_seq<N> M2;
      bool success = false;
      i32 iter = 0;
      while(!success) {
        if(iter == 4) return move_seq<N>();
        iter++;
        M2.moves = solve_globe2<N>(first ? num_wildcards : 0, M1.moves, tgt.g2[i], c.g2[i], success);
      }
      MS = merge_move_seq<N>(i,M-1-i, MS, M2);
      first = false;
    }

    c.move(MS);
    c.print();

    move_seq<N> MS2;
    if(is_hard) {
      for(int i : I){ 
        move_seq<N> T = solve_globe2_hard<N>(tgt.g2[i], c.g2[i]);
        for(auto p : T.moves) {
          if(p[0] == 2*N) {
            MS2.move(2*N+i, p[1]);
          }else if(p[0] == 2*N+1) {
            MS2.move(2*N+(M-1-i), p[1]);
          }else{
            MS2.move(p);
          }
        }
      }
    }
    PREMS.move(MS);
    PREMS.move(MS2);
    c.move(MS2);
    c.print();
    return PREMS;
  }
  
}
