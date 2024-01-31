#include "header.hpp"
#include "puzzle_info.hpp"
#include <cctype>
#include <filesystem>

i32 parse_int(istream& is){
  while(isspace(is.peek())) is.get();

  i32 x = 0;
  while(isdigit(is.peek())) {
    x = 10*x + (is.get()-'0');
  }
  return x;
}


vector<i32> parse_array(istream& is){
  while(isspace(is.peek())) is.get();
  is.get();

  vector<i32> R;
  
  while(1) {
    i32 x = parse_int(is);
    R.eb(x);
    while(isspace(is.peek())) is.get();
    if(is.get() == ']') return R;
  }
}


string parse_name(istream& is){
  while(isspace(is.peek())) is.get();
  is.get();
  string name;
  while(is.peek() != '\'') name += is.get();
  is.get();
  return name;
}


vector<tuple<string, vector<i32>>> parse_arrays(istream& is){
  while(isspace(is.peek())) is.get();
  is.get();

  vector<tuple<string, vector<i32>>> R;
  
  while(1) {
    string name = parse_name(is);
    is.get();
    auto A = parse_array(is);
    R.eb(name,A);
    while(isspace(is.peek())) is.get();

    if(is.peek() == '}') return R;
    is.get();
    is.get();
  }
}

vector<puzzle_info> read_puzzle_info(){
  ifstream is("puzzle_info.csv");
  string line;
  getline(is, line);

  vector<puzzle_info> R;
  while(getline(is, line)) {
    istringstream iss(line);

    string name;
    while(iss.peek() != ',') {
      name += iss.get();
    }
    iss.get();
    iss.get();

    puzzle_info p;
    p.name = name;
    auto A = parse_arrays(iss);
    p.from_arrays(A);
    R.eb(p);
  }
  return R;
}

i32 parse_item(map<string, i32>& M, istream& is) {
  string name;
  while(is.peek() != ',' && is.peek() != ';') {
    name += is.get();
  }
  if(!M.count(name)) {
    i32 x = M.size();
    M[name] = x;
  }
  return M[name];
}

vector<i32> parse_state(map<string, i32>& M, istream& is) {
  vector<i32> A;
  A.eb(parse_item(M,is));
  while(is.peek() != ',') {
    is.get();
    A.eb(parse_item(M,is));
  }
  return A;
}

vector<puzzle> read_puzzles(){
  ifstream is("puzzles.csv");
  string line;
  getline(is, line);

  vector<puzzle> R;
  while(getline(is, line)) {
    istringstream iss(line);

    i32 id = parse_int(iss);
    iss.get();
    string puzzle_type;
    while(iss.peek() != ',') {
      puzzle_type += iss.get();
    }
    iss.get();
    map<string, i32> M;
    
    auto tgt = parse_state(M,iss);
    iss.get();
    auto src = parse_state(M,iss);
    iss.get();
    i32 num_wildcards = parse_int(iss);
    
    puzzle p;
    p.id = id;
    p.puzzle_type = puzzle_type;
    p.tgt = tgt;
    p.src = src;
    p.num_wildcards = num_wildcards;
    R.eb(p);
  }
  return R;
}

string parse_solution_item(istream& is) {
  string r;
  while(is.peek() != '.' && is.good() && !isspace(is.peek())) {
    r += is.get();
  }
  if(is.peek() == '.') is.get();
  return r;
}

solution parse_solution(istream& is) {
  solution A;
  while(is.peek() != '.' && is.good() && !isspace(is.peek())) {
    A.eb(parse_solution_item(is));
  }
  return A;
}


vector<solution> read_solutions(string const& filename){
  ifstream is(filename);
  string line;
  getline(is, line);

  vector<solution> R;
  while(getline(is, line)) {
    istringstream iss(line);

    i32 id = parse_int(iss);
    runtime_assert(id == (i32)R.size());
    iss.get();
    auto S = parse_solution(iss);
    R.eb(S);
  }
  return R;
}

bool check_solution(const puzzle_info &info, const puzzle &p, const solution &S) {
  vector<i32> A = p.src;
  for(auto const& m : S) {
    bool found = 0;
    for(auto const& im : info.moves) if(im.name == m) {
        found = 1;
        im.apply(A);
        break;
      }
    if(!found) return false;
  }
  i32 nmis = 0;
  FOR(i, info.n) {
    if(A[i] != p.tgt[i]) nmis += 1;
  }
  return nmis <= p.num_wildcards;
}

void write_solutions(const vector<solution> &solutions) {
  string filename = "submission.csv." + to_string(rng.randomInt64());
  ofstream out(filename);
  out << "id,moves" << endl;
  FOR(i,solutions.size()) {
    out << i << ",";
    FOR(j, solutions[i].size()) {
      if(j) out << ".";
      out << solutions[i][j];
    }
    out << endl;
  }
  out.close();
  filesystem::rename(filename, "submission.csv");
}
