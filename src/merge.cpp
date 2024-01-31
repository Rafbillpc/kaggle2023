#include "header.hpp"
#include "puzzle_info.hpp"
#include "util.hpp"

int main(int argc, char** argv){
  init_util();
 
  auto PI = read_puzzle_info();
  auto PS = read_puzzles();

  auto find_puzzle_info = [&](string const& name) -> puzzle_info const& {
    for(auto const& p : PI) {
      if(p.name == name) return p;
    }
    runtime_assert(false);
  };
  
  auto best_solutions = read_solutions("submission.csv");
  for(i32 i = 1; i < argc; ++i) {
    auto sols = read_solutions(argv[i]);
    FOR(j, sols.size()) {
      auto p = PS[j];
      auto pi = find_puzzle_info(p.puzzle_type);
      if(check_solution(pi,p,sols[j])) {
        if(sols[j].size() < best_solutions[j].size()) {
          best_solutions[j] = sols[j];
        }
      }
    }
  }

  write_solutions(best_solutions);
    
  return 0;
}
