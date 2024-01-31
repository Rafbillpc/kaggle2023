#include "header.hpp"
#include "puzzle_info.hpp"

int main(){
  auto PI = read_puzzle_info();
  auto PS = read_puzzles();

  auto solutions = read_solutions("submission.csv");
  auto old_solutions = read_solutions("old.csv");
  runtime_assert(old_solutions.size() == solutions.size());

  auto find_puzzle_info = [&](string const& name) -> puzzle_info const& {
    for(auto const& p : PI) {
      if(p.name == name) return p;
    }
    runtime_assert(false);
  };

  const i32 npuzzles = PS.size();
  runtime_assert(npuzzles == (i32)solutions.size());

  map<string, i32> puzzle_count;
  map<string, i32> puzzle_min;
  map<string, i32> puzzle_max;
  map<string, i32> puzzle_total;
  map<string, i32> puzzle_old;
  map<string, i32> puzzle_kind_total;
  for(auto const& p : PS) {
    puzzle_min[p.puzzle_type] = 1e9;
    puzzle_max[p.puzzle_type] = 0;
  }

  i64 total_score = 0;
  
  for(auto const& p : PS) {
    auto pi = find_puzzle_info(p.puzzle_type);
    auto sol = solutions[p.id];

    string puzzle_kind;
    for(char c : pi.name) {
      if(c == '_') break;
      puzzle_kind += c;
    }
    
    runtime_assert(check_solution(pi, p, sol));

    puzzle_count[pi.name] += 1;
    puzzle_min[pi.name] = min<i32>(puzzle_min[pi.name], sol.size());
    puzzle_max[pi.name] = max<i32>(puzzle_max[pi.name], sol.size());
    puzzle_old[pi.name] += old_solutions[p.id].size();
    puzzle_total[pi.name] += sol.size();
    puzzle_kind_total[puzzle_kind] += sol.size();
    total_score += sol.size();
    // debug(pi.name, p.id, sol.size());
  }

  for(auto const& p : PI) {
    i32 cnt = puzzle_count[p.name];
    i32 tot = puzzle_total[p.name];
    cout << std::left;
    cout << "Puzzle " << setw(15) << p.name << ": ";
    cout << "count = " << setw(3) << cnt << ", ";
    cout << "total = " << setw(5) << tot << ", ";
    cout << "average = " << setw(10) << (double)tot/cnt << ", ";
    cout << "range = " << setw(5) << puzzle_min[p.name] << ".. " << setw(6) << puzzle_max[p.name] << ", ";
    if(puzzle_old[p.name] != puzzle_total[p.name]) {
      cout << "improv = " << setw(4) << puzzle_old[p.name]-puzzle_total[p.name];
    }
    cout << endl;
  }
  for(auto const& p : puzzle_kind_total) {
    cout << std::left;
    cout << "Puzzle kind " << setw(10) << p.first << ": ";
    cout << "total = " << setw(6) << p.second << ", ";
    cout << endl;
  }
  cout << "Total score = " << total_score << endl;
  
  return 0;
}
