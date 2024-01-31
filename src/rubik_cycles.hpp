#include "rubik.hpp"
#include "rubik_tables.hpp"
#include "util.hpp"

namespace rubik{

  struct small_array {
    small_array() {
      sz = 0;
    }

    void push(u8 x) {
      data[sz++] = x;
    }

    bool operator==(small_array const& o) const {
      if(sz != o.sz) return 0;
      return memcmp(data, o.data, sz) == 0;
    }
      
    bool operator<(small_array const& o) const {
      if(sz < o.sz) return 1;
      if(sz > o.sz) return 0;
      return memcmp(data, o.data, sz) < 0;
    }
    
    i32 sz;
    u8 data[16];
  };

  const size_t CORNERS_CYCLES_COUNT  = 6576;
  const size_t EDGES1_CYCLES_COUNT   = 29256;
  const size_t EDGES2_CYCLES_COUNT   = 56064;
  const size_t CENTERS2_CYCLES_COUNT = 718812;
  const size_t CENTERS3_CYCLES_COUNT = 483432;
  const size_t CENTERS4_CYCLES_COUNT = 224448;
 
  // const size_t CORNERS_CYCLES_COUNT  = 63216;
  // const size_t EDGES1_CYCLES_COUNT   = 359568;
  // const size_t EDGES2_CYCLES_COUNT   = 791904;
  // const size_t CENTERS2_CYCLES_COUNT = 2479356;
  // const size_t CENTERS3_CYCLES_COUNT = 1681344;
  // const size_t CENTERS4_CYCLES_COUNT = 1256064;
  
  extern small_array corners_cycles[CORNERS_CYCLES_COUNT];
  extern small_array edges1_cycles[EDGES1_CYCLES_COUNT];
  extern small_array edges2_cycles[EDGES2_CYCLES_COUNT];
  extern small_array centers2_cycles[CENTERS2_CYCLES_COUNT];
  extern small_array centers3_cycles[CENTERS3_CYCLES_COUNT];
  extern small_array centers4_cycles[CENTERS4_CYCLES_COUNT];

  void init_cycles();
  
  void solve_cycles_edges2(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c);
  void solve_cycles_centers2(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c);
  void solve_cycles_centers3(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c);
  void solve_cycles_centers4(i32 typ, i32 num_wildcards, vector<i32> const& tgt, move_seq &S, rubik& c);

}
