#pragma once
#include "header.hpp"

namespace globe {
  inline i32 move_length(i32 N, i32 y) {
    return min(y, 2*N-y);
  }

  template<i32 N>
  struct move_seq {
    vector<array<i32,2>> moves;
    i32 length = 0;

    void reset(){
      moves.clear();
      length = 0;
    }

    void move(i32 x, i32 y) {
      if(moves.empty() || moves.back()[0] != x) {
        moves.pb({x,y});
        if(y == 1) length += 1;
        else length += move_length(N, y);
      }else if(x < 2*N) {
        i32& z = moves.back()[1];
        length -= z;
        z = (z + y) % 2;
        length += z;
        if(z == 0) moves.pop_back();
      }else{
        i32& z = moves.back()[1];
        length -= move_length(N, z);
        z = (z + y) % (2*N);
        length += move_length(N, z);
        if(z == 0) moves.pop_back();
      }
    }

    void move(array<i32,2> const& p) {
      move(p[0],p[1]);
    }
    
    void move(move_seq<N> const& o) {
      for(auto p : o.moves) move(p);
    }

    void reverse(){
      std::reverse(all(moves));
      for(auto& m : moves) if(m[0] >= 2*N) {
          m[1] = (2*N-m[1]) % (2*N);
        }
    }

  };

  template<i32 N>
  move_seq<N> convert_from_solution(vector<string> const& S){
    move_seq<N> M;
    for(auto s : S) {
      bool inv = (s[0] == '-');
      char c = inv ? s[1] : s[0];
      i32 x = 0;
      FORU(i,inv?2:1,(i32)s.size()-1){
        x = 10*x + (s[i]-'0');
      }

      if(c == 'f') {
        M.move(x, 1);
      }else{
        M.move(2*N + x, inv ? 2*N-1 : 1);
      }
    }
    return M;
  }

  template<i32 N>
  vector<string> convert_to_solution(move_seq<N> const& MS){
    vector<string> S;
    for(auto const& m : MS.moves) {
      if(m[0] < 2*N) {
        S.eb("f" + to_string(m[0]));
      }else{
        if(m[1] < 2*N-m[1]) {
          FOR(k,m[1]) S.eb("r" + to_string(m[0]-2*N));
        }else{
          FOR(k,2*N-m[1]) S.eb("-r" + to_string(m[0]-2*N));
        }
      }
    }
    return S;
  }
 
  
  template<i32 N>
  struct globe1{
    i32 ring[2 * N];

    void reset(){
      FOR(i,2*N) ring[i] = i;
    }

    void rotate() {
      i32 tmp = ring[0];
      FOR(i,2*N-1) ring[i] = ring[i+1];
      ring[2*N-1] = tmp;
    }
    
    void rotate_inv() {
      i32 tmp = ring[2*N-1];
      FORD(i,2*N-2,0) ring[i+1] = ring[i];
      ring[0] = tmp;
    }

    void rev(i32 i) {
      // FOR(j,N/2) {
      //   i32 a = (i+j)%(2*N);
      //   i32 b = (i+N-1-j)%(2*N);
      //   swap(ring[a], ring[b]);
      // }
    }

    void normalize(){
      // set<i32> S;
      // FOR(i,2*N) S.insert(ring[i]);
      // vector<i32> AS(all(S));
      // map<i32,i32> TO;
      // FOR(i,AS.size()) TO[AS[i]] = i;
      // FOR(i,2*N) ring[i] = TO[ring[i]];
    }
  };
  
  template<i32 N>
  struct globe2{
    union {
      struct {
        i32 ring1[2 * N];
        i32 ring2[2 * N];
      };
      i32 data[4 * N];
    };

    void reset(){
      FOR(i,2*N) ring1[i] = i;
      FOR(i,2*N) ring2[i] = 2*N + i;
    }
    
    void rotate1() {
      i32 tmp = ring1[0];
      FOR(i,2*N-1) ring1[i] = ring1[i+1];
      ring1[2*N-1] = tmp;
    }
    
    void rotate1_inv() {
      i32 tmp = ring1[2*N-1];
      FORD(i,2*N-2,0) ring1[i+1] = ring1[i];
      ring1[0] = tmp;
    }

    void rotate2() {
      i32 tmp = ring2[0];
      FOR(i,2*N-1) ring2[i] = ring2[i+1];
      ring2[2*N-1] = tmp;
    }
    
    void rotate2_inv() {
      i32 tmp = ring2[2*N-1];
      FORD(i,2*N-2,0) ring2[i+1] = ring2[i];
      ring2[0] = tmp;
    }
    
    void rev(i32 i) {
      FOR(j,N) {
        i32 a = (i+j)%(2*N);
        i32 b = (i+N-1-j)%(2*N);
        swap(ring1[a], ring2[b]);
      }
    }

    void normalize(){
      set<i32> S;
      FOR(i,4*N) S.insert(data[i]);
      vector<i32> AS(all(S));
      map<i32,i32> TO;
      FOR(i,AS.size()) TO[AS[i]] = i;
      FOR(i,4*N) data[i] = TO[data[i]];
    }
    
    void print() const {
      FOR(i,6*N) cout << '-';
      cout << endl;
      FOR(x,2*N) cout << setw(2) << ring1[x] << ' ';
      cout << endl;
      FOR(x,2*N) cout << setw(2) << ring2[x] << ' ';
      cout << endl;
      FOR(i,6*N) cout << '-';
      cout << endl;
    }

    void move(i32 x, i32 y) {
      if(x < 2*N) {
        rev(x);
        return;
      }
      x -= 2*N;
      if(x == 0) {
        if(y < 2*N-y) {
          FOR(k,y) rotate1();
        }else{
          FOR(k,2*N-y) rotate1_inv();
        }
      }else{
        if(y < 2*N-y) {
          FOR(k,y) rotate2();
        }else{
          FOR(k,2*N-y) rotate2_inv();
        }
      }
    }

    void move(array<i32,2> p) {
      move(p[0],p[1]);
    }
    void move_inv(array<i32,2> p) {
      if(p[0] >= 2*N) {
        p[1] = (2*N-p[1])%(2*N);
      }
      move(p[0],p[1]);
    }
  };
 
  template<i32 N, i32 M>
  struct globe {
    static const bool IS_ODD = M%2 == 1;
    globe1<N> g1[IS_ODD];
    globe2<N> g2[M/2];

    void rev(i32 x) {
      if(IS_ODD) g1[0].rev(x);
      FOR(i,M/2) g2[i].rev(x);
    }

    void reset(){
      if(IS_ODD) g1[0].reset();
      FOR(i,M/2) g2[i].reset();
    }

    void print() const {
      FOR(i,6*N) cout << '-';
      cout << endl;
      if(IS_ODD) {
        FOR(x,2*N) cout << setw(2) << g1[0].ring[x] << ' ';
        cout << endl;
      }
      FOR(i,M/2) {
        FOR(i,6*N) cout << '-';
        cout << endl;
        FOR(x,2*N) cout << setw(2) << g2[i].ring1[x] << ' ';
        cout << endl;
        FOR(x,2*N) cout << setw(2) << g2[i].ring2[x] << ' ';
        cout << endl;
      }
      FOR(i,6*N) cout << '-';
      cout << endl;
    }
    
    void from_facelets(vector<i32> const& X) {
      FOR(i,M/2) {
        FOR(x,2*N) g2[i].ring1[x] = X[i*2*N+x];
      }
      if(IS_ODD) {
        FOR(x,2*N) g1[0].ring[x] = X[(M/2)*2*N+x];
      }
      FOR(i,M/2) {
        FOR(x,2*N) g2[M/2-1-i].ring2[x] = X[(M/2+IS_ODD+i)*2*N+x];
      }
      g1[0].normalize();
      FOR(i,M/2) g2[i].normalize();
    }

    vector<i32> to_facelets(){
      vector<i32> X(2*N*M);
      FOR(i,M/2) {
        FOR(x,2*N) X[i*2*N+x] = g2[i].ring1[x];
      }
      if(IS_ODD) {
        FOR(x,2*N) X[(M/2)*2*N+x] = g1[0].ring[x];
      }
      FOR(i,M/2) {
        FOR(x,2*N) X[(M/2+IS_ODD+i)*2*N+x] = g2[M/2-1-i].ring2[x];
      }
      return X;
    }

    void move(i32 x, i32 y) {
      if(x < 2*N) {
        rev(x);
        return;
      }
      x -= 2*N;
      if(x < M/2) {
        FOR(k,y) g2[x].rotate1();
      }else if(IS_ODD && x == M/2) {
        FOR(k,y) g1[0].rotate();
      }else{
        FOR(k,y) g2[M-1-x].rotate2();
      }
    }
    
    void move(move_seq<N> const& MS) {
      for(auto m : MS.moves) move(m[0],m[1]);
    }
  };

    
}
