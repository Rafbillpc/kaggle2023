#pragma once
#include "header.hpp"

static inline
vector<vector<i32>> perm_to_cycles(vector<i32> const& A) {
  i32 n = A.size();
  vector<i32> E(n,0);
  vector<vector<i32>> cycles;
  FOR(i,n) if(!E[i] && A[i] != i) {
    cycles.eb();
    for(i32 j = i; !E[j]; j = A[j]) {
      E[j] = 1;
      cycles.back().eb(j);
    }
  }
  return cycles;
}

struct puzzle_move {
  string name;
  vector<vector<i32>> cycles;
  i32 order;

  puzzle_move(string const& name_, vector<vector<i32>> const& cycles_) {
    name   = name_;
    cycles = cycles_;

    order = 1;
    for(auto const& c : cycles) order = (order/__gcd<i32>(order,c.size())) * c.size();
  }

  void apply(vector<i32>& A) const {
    for(auto const& c : cycles) {
      i32 tmp = A[c[0]];
      i32 n = c.size();
      FOR(i,n-1) {
        A[c[i]] = A[c[i+1]];
      }
      A[c[n-1]] = tmp;
    }
  }
};

struct puzzle_info {
  string name;
  vector<puzzle_move> moves;
  i32 n;

  void from_arrays(vector<tuple<string, vector<i32>>> const& A) {
    for(auto const& m : A) {
      auto cycles = perm_to_cycles(get<1>(m));
      moves.eb(get<0>(m), cycles);
      for(auto& c : cycles) reverse(all(c));
      moves.eb("-" + get<0>(m), cycles);
    }
    n = get<1>(A[0]).size();
  }
};

vector<puzzle_info> read_puzzle_info();

struct puzzle {
  i32 id;
  string puzzle_type;
  vector<i32> tgt, src;
  i32 num_wildcards;
};

vector<puzzle> read_puzzles();

using solution = vector<string>;

vector<solution> read_solutions(string const& filename);
bool check_solution(puzzle_info const& info, puzzle const& p, solution const& S);
void write_solutions(vector<solution> const& solutions);
