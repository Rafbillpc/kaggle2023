#pragma once
#include "header.hpp"
#include "util.hpp"
#include <compare>

//           outer1
//
//           inner1
//   both1            both2
//           inner2
//
//           outer2

// both1 inner1 both1 outer1
// both1 outer2 both2 inner2

namespace wreath {
  inline i32 move_length(i32 N, i32 y) {
    return min(y, N-y);
  }
  
  template<i32 A, i32 B>
  struct move_seq {
    static const i32 N  = A+B+2;
    vector<array<i32,2>> moves;
    i32 length;

    move_seq() {
      reset();
    }
    
    void reset() {
      moves.clear();
      length = 0;
    }

    void move(i32 x, i32 y) {
      if(moves.empty() || moves.back()[0] != x) {
        moves.pb({x,y});
        length += move_length(N, y);
      }else{
        i32& z = moves.back()[1];
        length -= move_length(N, z);
        z = (z+y)%N;
        length += move_length(N, z);
        if(z == 0) moves.pop_back();
      }
    }

    void move(move_seq<A, B> const& o) {
      for(auto p : o.moves) move(p[0], p[1]);
    }

    void reverse(){
      std::reverse(all(moves));
      for(auto& p : moves) p[1] = (N-p[1]) % N;
    }
  };

  template<i32 A, i32 B>
  move_seq<A,B> convert_from_solution(vector<string> const& S){
    static const i32 N = A+B+2;
    move_seq<A,B> M;
    for(auto s : S) {
      bool inv = (s[0] == '-');
      char c = inv ? s[1] : s[0];
      M.move(c == 'l' ? 0 : 1, inv ? (N-1) : 1);
    }
    return M;
  }

  template<i32 A, i32 B>
  vector<string> convert_to_solution(move_seq<A,B> const& MS){
    static const i32 N = A+B+2;
    vector<string> S;
    for(auto const& m : MS.moves){
      if(m[0] == 0) {
        if(m[1] < N-m[1]) {
          FOR(i,m[1]) S.eb("l");
        }else {
          FOR(i,N-m[1]) S.eb("-l");
        }
      }else{
        if(m[1] < N-m[1]) {
          FOR(i,m[1]) S.eb("r");
        }else {
          FOR(i,N-m[1]) S.eb("-r");
        }
      }
    }
    return S;
  }
  
  template<i32 A, i32 B>
  struct wreath {
    static const i32 A1 = A+1;
    static const i32 B1 = B-1;
    static const i32 A2 = A;
    static const i32 B2 = B;
    static const i32 N  = A+B+2;

    union {
      struct {
        u8 outer1[A1], outer2[A2];
        u8 inner1[B1], inner2[B2];
        u8 both1, both2;
      };
      u8 data[A+B+A+B+2];
    };

    bool operator==(wreath<A,B> const& o) const {
      FOR(i,A+B+A+B+2) if(data[i] != o.data[i]) return 0;
      return 1;
    }
    bool operator<(wreath<A,B> const& o) const {
      FOR(i,A+B+A+B+2) {
        if(data[i] < o.data[i]) return 1;
        if(data[i] > o.data[i]) return 0;
      }
      return 0;
    }

    void reset(){
      both1 = both2 = 0;
      FOR(i,A1) outer1[i] = 1;
      FOR(i,B1) inner1[i] = 1;
      FOR(i,A2) outer2[i] = 2;
      FOR(i,B2) inner2[i] = 2;
    }
    
    void from_facelets(vector<i32> const& X){
      both2 = X[0];
      FOR(i,B1) inner1[B1-1-i] = X[1+i];
      both1 = X[B];
      FOR(i,A1) outer1[i] = X[B+1+i];
      FOR(i,A2) outer2[A2-1-i] = X[N+i];
      FOR(i,B2) inner2[i] = X[N+A2+i];
    }

    vector<i32> to_facelets(){
      vector<i32> X(N+N-2, -1);
      X[0] = both2;
      FOR(i,B1) X[1+i] = inner1[B1-1-i];
      X[B] = both1;
      FOR(i,A1) X[B+1+i] = outer1[i];
      FOR(i,A2) X[N+i] = outer2[A2-1-i];
      FOR(i,B2) X[N+A2+i] = inner2[i];
      return X;
    }
    
    void move1(){
      i32 tmp = outer1[0];
      FOR(i,A1-1) outer1[i] = outer1[i+1];
      outer1[A1-1] = both2;
      both2 = inner1[B1-1];
      FORD(i,B1-2,0) inner1[i+1] = inner1[i];
      inner1[0] = both1;
      both1 = tmp;
    }

    void move1_inv(){
      i32 tmp = outer1[A1-1];
      FORD(i,A1-2,0) outer1[i+1] = outer1[i];
      outer1[0] = both1;
      both1 = inner1[0];
      FOR(i,B1-1) inner1[i] = inner1[i+1];
      inner1[B1-1] = both2;
      both2 = tmp;
    }

    void move2(){
      i32 tmp = outer2[0];
      FOR(i,A2-1) outer2[i] = outer2[i+1];
      outer2[A2-1] = both2;
      both2 = inner2[B2-1];
      FORD(i,B2-2,0) inner2[i+1] = inner2[i];
      inner2[0] = both1;
      both1 = tmp;
    }

    void move2_inv(){
      i32 tmp = outer2[A2-1];
      FORD(i,A2-2,0) outer2[i+1] = outer2[i];
      outer2[0] = both1;
      both1 = inner2[0];
      FOR(i,B2-1) inner2[i] = inner2[i+1];
      inner2[B2-1] = both2;
      both2 = tmp;
    }

    void print() {
      FOR(i,2*A1+5) cout << '-';
      cout << endl;
      FOR(i,2) cout << ' ';
      FOR(i,A1) {
        cout << outer1[i] << ' ';
      } cout << endl;
      FOR(i,2+(A1-B1)) cout << ' ';
      FOR(i,B1) {
        cout << inner1[i] << ' ';
      } cout << endl;
      cout << both1;
      FOR(i,3+2*A) cout << ' ';
      cout << both2 << endl;
      FOR(i,3+(A2-B2)) cout << ' ';
      FOR(i,B2) {
        cout << inner2[i] << ' ';
      } cout << endl;
      FOR(i,3) cout << ' ';
      FOR(i,A2) {
        cout << outer2[i] << ' ';
      } cout << endl;
      FOR(i,2*A1+5) cout << '-';
      cout << endl;
    }
      
    void move(move_seq<A,B> const& M) {
      for(auto const& p : M.moves){
        if(p[0] == 0) {
          FOR(i,p[1]) move1();
        }else{
          FOR(i,p[1]) move2_inv();
        }
      }
    }
  };
}
