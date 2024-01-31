#pragma once
#include "util.hpp"
#include "wreath.hpp"
#include "header.hpp"
#include <cstdint>

namespace wreath {

  template<i32 A, i32 B>
  move_seq<A,B> solve_wreath_optimal(wreath<A,B> src){
    i32 N  = A+B+2;
    wreath<A,B> tgt; tgt.reset();

    auto get_key = [&](wreath<A,B> const& x) -> u64 {
      u64 mask1 = 0, mask2 = 0;
      FOR(i, A+B+A+B+2) {
        if(x.data[i] == 1) mask1 |= 1ull<<i;
        if(x.data[i] == 2) mask2 |= 1ull<<i;
      }
      runtime_assert(__builtin_popcountll(mask1) == A+B);
      runtime_assert(__builtin_popcountll(mask2) == A+B);
      return uint64_hash::hash_int(uint64_hash::hash_int(mask1) + mask2);
    };

    hash_map<u64, i32> HS1, HS2;
    bool found = 0;

    auto recon1 = [&](wreath<A,B> x) -> move_seq<A, B> {
      move_seq<A,B> MS;
      while(HS1[get_key(x)] != 0) {
        x.print();
        i32 d = HS1[get_key(x)];
        debug(d);
        auto test = [&](wreath<A,B> const& y) {
          debug(HS1.find(get_key(y)) != HS1.end());
          if(HS1.find(get_key(y)) != HS1.end()) {
            debug(HS1[get_key(y)]);
            if(HS1[get_key(y)] < d) {
              x = y;
              return true;
            }
          }
          return false;
        };
        { auto y = x; y.move1(); MS.move(0, 1); if(test(y)) goto end1; MS.move(0, N-1); }
        { auto y = x; y.move1_inv(); MS.move(0, N-1); if(test(y)) goto end1; MS.move(0, 1); }
        { auto y = x; y.move2(); MS.move(1, N-1); if(test(y)) goto end1; MS.move(1, 1); }
        { auto y = x; y.move2_inv(); MS.move(1, 1); if(test(y)) goto end1; MS.move(1, N-1); }
        impossible();
      end1:;
      }
      return MS;
    };

    auto recon2 = [&](wreath<A,B> x) -> move_seq<A, B> {
      move_seq<A,B> MS;
      while(HS2[get_key(x)] != 0) {
        x.print();
        i32 d = HS2[get_key(x)];
        debug(d);
        auto test = [&](wreath<A,B> const& y) {
          debug(HS2.find(get_key(y)) != HS2.end());
          if(HS2.find(get_key(y)) != HS2.end()) {
            debug(HS2[get_key(y)]);
            if(HS2[get_key(y)] < d) {
              x = y;
              return true;
            }
          }
          return false;
        };
        { auto y = x; y.move1(); MS.move(0, 1); if(test(y)) goto end1; MS.move(0, N-1); }
        { auto y = x; y.move1_inv(); MS.move(0, N-1); if(test(y)) goto end1; MS.move(0, 1); }
        { auto y = x; y.move2(); MS.move(1, N-1); if(test(y)) goto end1; MS.move(1, 1); }
        { auto y = x; y.move2_inv(); MS.move(1, 1); if(test(y)) goto end1; MS.move(1, N-1); }
        impossible();
      end1:;
      }
      return MS;
    };

    move_seq<A,B> SOL;
    
    auto recon = [&](wreath<A,B> const& x) {
      found = 1;
      debug("ok");
      auto S = recon1(x);
      auto T = recon2(x);
      S.reverse();
      S.move(T);
      SOL = S;
    };
    
    vector<wreath<A,B>> Q1[255];
    vector<wreath<A,B>> Q2[255];
    auto add1 = [&](wreath<A,B> const& x, i32 d){
      if(HS1.find(get_key(x)) != end(HS1)) return;
      HS1[get_key(x)] = d;
      if(HS2.find(get_key(x)) != HS2.end()) {
        recon(x);
      }
      Q1[d].eb(x);
    };
    auto add2 = [&](wreath<A,B> const& x, i32 d){
      if(HS2.find(get_key(x)) != end(HS2)) return;
      HS2[get_key(x)] = d;
      if(HS1.find(get_key(x)) != HS1.end()) {
        recon(x);
      }
      Q2[d].eb(x);
    };

    add1(src, 0);
    add2(tgt, 0);
    for(i32 d1 = 0, d2 = 0;;) {
      if(found) break;
      debug(d1+d2,Q1[d1].size(), Q2[d2].size());
      if(Q1[d1].size() < Q2[d2].size()) {
        for(auto x : Q1[d1]) {
          if(found) break;
          { auto y = x; y.move1(); add1(y, d1+1); }
          { auto y = x; y.move1_inv(); add1(y, d1+1); }
          { auto y = x; y.move2(); add1(y, d1+1); }
          { auto y = x; y.move2_inv(); add1(y, d1+1); }
        }
        Q1[d1].clear(); Q1[d1].shrink_to_fit();
        d1++;
      }else{
        for(auto x : Q2[d2]) {
          if(found) break;
          { auto y = x; y.move1(); add2(y, d2+1); }
          { auto y = x; y.move1_inv(); add2(y, d2+1); }
          { auto y = x; y.move2(); add2(y, d2+1); }
          { auto y = x; y.move2_inv(); add2(y, d2+1); }
        }
        Q2[d2].clear(); Q2[d2].shrink_to_fit();
        d2++;
      }
    }

    src.move(SOL);
    src.print();
    tgt.print();
    
    return SOL;
  }

  template<i32 A, i32 B>
  move_seq<A,B> solve_wreath(i32 num_wildcards, wreath<A,B>& c){
    i32 N  = A+B+2;
    if(N <= 21) return solve_wreath_optimal(c);

    vector<i32> X;
    i32 shusz = rng.random32(A+B+N);
    
    if(rng.random32(2)) {
      for(i32 i = 0; i < shusz; ++i) {
        if(c.both2 == 1) {
          X.pb(0);
          c.move1();
        }else{
          X.pb(2);
          c.move2();
        }
      }
    }else{
      for(i32 i = 0; i < shusz; ++i) {
        if(c.both1 == 1) {
          X.pb(1);
          c.move1_inv();
        }else{
          X.pb(3);
          c.move2_inv();
        }
      }
    }
    c.print();

    while(1) {
      wreath<A,B> d; d.reset();
      auto diff_distance = [&](){
        i32 r = 0;
        FOR(i,N+A+B) if(c.data[i] != d.data[i]) r += 1;
        return r;
      };
      
      i32 s = diff_distance();
      if(s <= num_wildcards) break;
      i32 FoundS = s;
      i32 FoundZ = 100;
      i32 FoundC = 0;
      debug(s);
      
      vector<i32> Y;
      vector<i32> BY;
      while(!X.empty()) {
        auto p = X.back(); X.pop_back();
        if(p == 0) { c.move1_inv(); d.move1_inv(); }
        if(p == 1) { c.move1(); d.move1(); }
        if(p == 2) { c.move2_inv(); d.move2_inv(); }
        if(p == 3) { c.move2(); d.move2(); }
        Y.pb(p); BY.pb(p);

        auto c_do_move = [&](i32 p) {
          if(p == 0) c.move1();
          if(p == 1) c.move1_inv();
          if(p == 2) c.move2();
          if(p == 3) c.move2_inv();
        };

        vector<i32> I;
        auto test_I = [&]() {
          i32 szi = I.size();
          if(szi == 0) return;
          FOR(i,szi) c_do_move(I[i]);
          i32 ds = diff_distance();
          if(ds < s) {
            if((s-ds) * FoundZ > (s-FoundS) * szi) {
              FoundS = ds;
              FoundZ = szi;
              FoundC = 0;
            }
            if((s-ds) * FoundZ == (s-FoundS) * szi) {
              FoundC += 1;
              if(rng.random32(FoundC) == 0) {
                BY = Y;
                FORD(i,szi-1,0) BY.pb(I[i]);
              }
            }
          }
          FORD(i,szi-1,0) c_do_move(I[i]^1);
        };
        
        auto dfs = letrec([&](auto dfs, i32 depth, i32 limit, i32 last) -> void {
          test_I();
          if(depth == limit) return;
          FOR(p,4) if(p != (last^1)) {
            I.eb(p);
            dfs(depth+1, limit, p);
            I.pop_back();
          }
        });

        dfs(0,10, -1);
      }

      debug(FoundS, FoundZ);

      while(!BY.empty()) {
        auto p = BY.back(); BY.pop_back();
        if(p == 0) { c.move1(); d.move1(); }
        if(p == 1) { c.move1_inv(); d.move1_inv(); }
        if(p == 2) { c.move2(); d.move2(); }
        if(p == 3) { c.move2_inv(); d.move2_inv(); }
        X.pb(p);
      }
      if(s == FoundS) break;
    }

    c.print();
    move_seq<A,B> S;
    for(auto const& p : X) {
      if(p == 0) S.move(0,1);
      if(p == 1) S.move(0,N-1);
      if(p == 2) S.move(1,N-1);
      if(p == 3) S.move(1,1);
    }

    return S;
  }
  

}
