#include "header.hpp"
#include "puzzle_info.hpp"
#include "rubik.hpp"
#include "rubik_init.hpp"
#include "rubik_solve.hpp"
#include "wreath.hpp"
#include "wreath_solve.hpp"
#include "globe_solve.hpp"
#include "util.hpp"

#include "argparse/argparse.hpp"

vector<solution> sample_solutions;
vector<solution> best_solutions;

void test_solution(puzzle_info const& pi, puzzle const& p, solution const& S) {
  runtime_assert(check_solution(pi,p,S));
#pragma omp critical
  {
    best_solutions = read_solutions("submission.csv");
    if(S.size() < best_solutions[p.id].size()) {
      cout << "New best for puzzle #" << p.id << ": " <<
        best_solutions[p.id].size() << " -> " << S.size() << endl;
      best_solutions[p.id] = S;
      write_solutions(best_solutions);
    }
  }
}

struct rubik_check {
  template<i32 N>
  static void work(puzzle_info const& pi, puzzle const& p){
    { auto MS = rubik::convert_from_solution(N, best_solutions[p.id]);
      cout << "length: " << MS.length << endl;
      cout << "alg_cubing_net: " << MS.to_alg_cubing_net() << endl;
    }

    auto MS = rubik::convert_from_solution(N, sample_solutions[p.id]);
    auto IMS = MS; IMS.reverse();
    rubik::rubik c(N); c.reset();
    c.move(IMS);

    i32 typ = 0;
    if(*max_element(all(p.src)) == 5) {
      if(p.tgt[1] == 0) typ = 0;
      else if(N&1) typ = 1;
      else typ = 2;
    }else{
      typ = 3;
    }
    if(typ != 3) {
      rubik::print_facelets(N, p.tgt);
    }
    
    auto S = rubik::solve(typ, p.num_wildcards, p.tgt, c);
    
    solution T = rubik::convert_to_solution(S);
    if(check_solution(pi,p,T)) {
      runtime_assert((i32)T.size() == S.length);
      cout << "Solution length = " << T.size() << " (against " << best_solutions[p.id].size() << ")" << endl;
      test_solution(pi, p, T);
    }
  }
};

struct wreath_check {
  template<i32 A, i32 B>
  static void work(puzzle_info const& pi, puzzle const& p){
    debug(pi.name);
    wreath::wreath<A,B> c;
    c.from_facelets(p.tgt);
    c.reset();
    runtime_assert(c.to_facelets() == p.tgt);
    c.from_facelets(p.src);
    c.print();
    runtime_assert(c.to_facelets() == p.src);
    
    c.from_facelets(p.src);
    auto S = wreath::solve_wreath(p.num_wildcards, c);
    debug(S.length);

    c.from_facelets(p.src);
    c.move(S);
    c.print();
    c.from_facelets(p.tgt);
    c.print();


    auto T = wreath::convert_to_solution<A,B>(S);
    runtime_assert(check_solution(pi,p,T));
    if(check_solution(pi,p,T)) {
      debug(T.size(), best_solutions[p.id].size());
      test_solution(pi,p,T);
    }
  }
};

struct globe_check {
  template<i32 N, i32 M>
  static void work(puzzle_info pi, puzzle p){
    // if(p.tgt[1] == 1) return;
    globe::globe<N,M> tgt,c;
    tgt.from_facelets(p.tgt);
    c.from_facelets(p.src);
    auto MS = globe::convert_from_solution<N>(sample_solutions[p.id]);
    c.move(MS);
    runtime_assert(c.to_facelets() == tgt.to_facelets());
    c.from_facelets(p.src);
    auto S = globe::solve_globe(p.num_wildcards, tgt, c);
    debug(S.length);

    auto T = globe::convert_to_solution<N>(S);
    debug(T.size(), best_solutions[p.id].size());
    test_solution(pi,p,T);
  }
};


template<class F, class... Args>
void dispatch_rubik(string name, Args&&... args){
  if(name[0] == 'c') rubik::init();
  if(name == "cube_2/2/2") F::template work<2>(forward<Args>(args)...);
  else if(name == "cube_3/3/3") F::template work<3>(forward<Args>(args)...);
  else if(name == "cube_4/4/4") F::template work<4>(forward<Args>(args)...);
  else if(name == "cube_5/5/5") F::template work<5>(forward<Args>(args)...);
  else if(name == "cube_6/6/6") F::template work<6>(forward<Args>(args)...);
  else if(name == "cube_7/7/7") F::template work<7>(forward<Args>(args)...);
  else if(name == "cube_8/8/8") F::template work<8>(forward<Args>(args)...);
  else if(name == "cube_9/9/9") F::template work<9>(forward<Args>(args)...);
  else if(name == "cube_10/10/10") F::template work<10>(forward<Args>(args)...);
  else if(name == "cube_19/19/19") F::template work<19>(forward<Args>(args)...);
  else if(name == "cube_33/33/33") F::template work<33>(forward<Args>(args)...);
}

template<class F, class... Args>
void dispatch_wreath(string name, Args&&... args){
  if(name == "wreath_6/6") F::template work<2,2>(forward<Args>(args)...);
  else if(name == "wreath_7/7") F::template work<3,2>(forward<Args>(args)...);
  else if(name == "wreath_12/12") F::template work<7,3>(forward<Args>(args)...);
  else if(name == "wreath_21/21") F::template work<13,6>(forward<Args>(args)...);
  else if(name == "wreath_33/33") F::template work<22,9>(forward<Args>(args)...);
  else if(name == "wreath_100/100") F::template work<73,25>(forward<Args>(args)...);
}

template<class F, class... Args>
void dispatch_globe(string name, Args&&... args){
  if(0){}
  else if(name == "globe_1/8") F::template work<8,2>(forward<Args>(args)...);
  else if(name == "globe_1/16") F::template work<16,2>(forward<Args>(args)...);
  else if(name == "globe_2/6") F::template work<6,3>(forward<Args>(args)...);
  else if(name == "globe_3/4") F::template work<4,4>(forward<Args>(args)...);
  else if(name == "globe_6/4") F::template work<4,7>(forward<Args>(args)...);
  else if(name == "globe_6/8") F::template work<8,7>(forward<Args>(args)...);
  else if(name == "globe_6/10") F::template work<10,7>(forward<Args>(args)...);
  else if(name == "globe_8/25") F::template work<25,9>(forward<Args>(args)...);
  else if(name == "globe_3/33") F::template work<33,4>(forward<Args>(args)...);
}

int main(int argc, char** argv){
  rng = RNG();
  init_util();
 
  argparse::ArgumentParser program("kaggle2023");

  program.add_argument("id")
    .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    cerr << err.what() << endl;
    cerr << program;
    return 1;
  }

  int opt_id = program.get<int>("id");


  auto PI = read_puzzle_info();
  auto PS = read_puzzles();
  sample_solutions = read_solutions("sample_submission.csv");
  best_solutions = read_solutions("submission.csv");
  
  runtime_assert(0 <= opt_id && opt_id <= (i32)PS.size());
  
  auto find_puzzle_info = [&](string const& name) -> puzzle_info const& {
    for(auto const& p : PI) {
      if(p.name == name) return p;
    }
    runtime_assert(false);
  };

  const i32 npuzzles = PS.size();
  runtime_assert(npuzzles == (i32)best_solutions.size());
  
  timer T;
  { auto p = PS[opt_id];
    auto pi = find_puzzle_info(p.puzzle_type);
    dispatch_rubik<rubik_check>(pi.name, pi, p);
    dispatch_wreath<wreath_check>(pi.name, pi, p);
    dispatch_globe<globe_check>(pi.name, pi, p);
  }
  cout << "Elapsed:" << T.elapsed() << "s" << endl;

  return 0;
}
