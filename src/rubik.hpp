#pragma once
#include "header.hpp"
#include "util.hpp"
#include "rubik_tables.hpp"
#include <compare>

namespace rubik {
  static const i32 NSYM_UD = 8;
  static const i32 sym_inv[8] = {0,7,2,3,4,5,6,1};
  
  static inline i32 move_opposite(i32 m) { return 3*(m/3) + (2 - (m%3)); }
  static inline i32 move_length(i32 m) { return (m%3 == 1) ? 2 : 1; }

  template<i32 MOVES_PER_AXIS_>
  struct with_moves {
    static const i32 MOVES_PER_AXIS = MOVES_PER_AXIS_;
    static const i32 MOVES = MOVES_PER_AXIS * 3 * 3;

    static i32 move_axis(i32 m) { return (m / (MOVES_PER_AXIS * 3)); }
    static i32 move_index(i32 m) { return (m / 3) % MOVES_PER_AXIS; }
    
    strong_ordering operator<=>(const with_moves&) const = default;
  };
  
  template<i32 N_, i32 O_, i32 MOVES_PER_AXIS_,
           u8 transition[MOVES_PER_AXIS_ * 9][N_ * O_]>
  struct cubies : with_moves<MOVES_PER_AXIS_> {
    static const i32 MOVES_PER_AXIS = MOVES_PER_AXIS_;
    static const i32 MOVES = MOVES_PER_AXIS * 3 * 3;
    static const i32 N = N_;
    static const i32 O = O_;

    array<u8, N> c;

    strong_ordering operator<=>(const cubies&) const = default;

    cubies() { reset(); }
    
    void reset(){
      FOR(i,N) c[i] = O * i;
    }

    inline void move(i32 m) {
      FOR(i, N) c[i] = transition[m][c[i]];
    }

    bool is_solved() const {
      FOR(i, N) if(c[i] != O * i) return false;
      return true;
    }
    
    i32 num_unsolved() const {
      i32 r = 0;
      FOR(i, N) if(c[i] != O * i) r += 1;
      return r;
    }
    
    i32 total_orientation() const {
      i32 o = 0;
      FOR(i,N) o = (o + c[i])%O;
      return o;
    }

    u8 parity() const {
      u8 x = 0;
      FOR(j,N) {
        FOR(i,j) {
          if(c[i] > c[j]) x ^= 1;
        }
      }
      return x;
    }
    
    void sym_base(i32 s) {
      if(s == 0) {
        FOR(i, MOVES_PER_AXIS) move((0 * MOVES_PER_AXIS + i)*3 + 0);
      }
      if(s == 1) {
        FOR(i, MOVES_PER_AXIS) move((1 * MOVES_PER_AXIS + i)*3 + 1);
      }
      if(s == 2) {
        FOR(i, MOVES_PER_AXIS) move((2 * MOVES_PER_AXIS + i)*3 + 1);
      }
    }

    void sym(i32 s) {
      FOR(i,3) if(s&(1<<i)) sym_base(i);
    }

    void mult(cubies const& b) {
      array<u8,N> res;
      FOR(i,N) {
        u8 p = b.c[c[i] / O] / O;
        u8 o = (b.c[c[i] / O] + c[i]) % O;
        res[i] = O*p+o;
      }
      c = res;
    }

    void sym_conj(i32 s) {
      auto x = *this;
      reset();
      sym(s);
      mult(x);
      sym(sym_inv[s]);
    }

    void sym_conj_01_12() {
      auto x = *this;
      reset();
      FOR(i, MOVES_PER_AXIS) move((1 * MOVES_PER_AXIS + i)*3 + 0);
      mult(x);
      FOR(i, MOVES_PER_AXIS) move((1 * MOVES_PER_AXIS + i)*3 + 2);
    }
    
    void print() const {
      FOR(i,N) cout << setw(2) << (int)(c[i]/O) << ":" << (int)(c[i]%O) << ' ';
      cout << endl;
    }

    void sym6(i32 s) {
      if(s%3 == 1) {
        FOR(i, MOVES_PER_AXIS) move((1 * MOVES_PER_AXIS + i)*3 + 0);
      }
      if(s%3 == 2) {
        FOR(i, MOVES_PER_AXIS) move((2 * MOVES_PER_AXIS + i)*3 + 0);
      }
      if(s/3 == 1) {
        FOR(i, MOVES_PER_AXIS) move((0 * MOVES_PER_AXIS + i)*3 + 0);
      }
    }

    void sym6_inv(i32 s) {
      if(s/3 == 1) {
        FOR(i, MOVES_PER_AXIS) move((0 * MOVES_PER_AXIS + i)*3 + 2);
      }
      if(s%3 == 1) {
        FOR(i, MOVES_PER_AXIS) move((1 * MOVES_PER_AXIS + i)*3 + 2);
      }
      if(s%3 == 2) {
        FOR(i, MOVES_PER_AXIS) move((2 * MOVES_PER_AXIS + i)*3 + 2);
      }
    }

    void sym6_conj(i32 s) {
      auto x = *this;
      reset();
      sym6(s);
      mult(x);
      sym6_inv(s);
    }

    static i32 sym6_move(i32 s, i32 m) {
      cubies c; c.reset();
      c.move(m);
      c.sym6_conj(s);
      FOR(mm, MOVES) {
        cubies d; d.reset(); d.move(mm);
        if(d == c) return mm;
      }
      impossible();
    }
 
  }; 
 
  const i32 CORNERS_COUNT = 8;
  const i32 CORNERS_MOVES_PER_AXIS = 2;
  extern u8 corners_transition[CORNERS_MOVES_PER_AXIS * 9][CORNERS_COUNT * 3];
 
  struct corners
    : cubies<CORNERS_COUNT, 3,
             CORNERS_MOVES_PER_AXIS,
             corners_transition>
  {

    static i32 move_real_index(i32 N, i32 m) {
      i32 table[2] = {0,N-1};
      return table[move_index(m)];
    }
  };

  const i32 EDGES1_COUNT = 12;
  const i32 EDGES1_MOVES_PER_AXIS = 3;
  extern u8 edges1_transition[EDGES1_MOVES_PER_AXIS * 9][2 * EDGES1_COUNT];
  struct edges1
    : cubies<EDGES1_COUNT, 2,
             EDGES1_MOVES_PER_AXIS,
             edges1_transition>
  {

    static constexpr bool is_ud[12] =
      {
        0,0,0,0,
        0,0,0,0,
        1,1,1,1,
      };

    static constexpr bool is_lr[12] =
      {
        1,0,1,0,
        1,0,1,0,
        0,0,0,0,
      };

    static constexpr bool is_fb[12] =
      {
        0,1,0,1,
        0,1,0,1,
        0,0,0,0,
      };

    static constexpr u8 to_phase4_555[12] =
      { 255,0,255,1,
        255,2,255,3,
        4,5,6,7,
      };
 
    static constexpr u8 to_phase5_555[12] =
      { 0,255,1,255,
        2,255,3,255,
        4,5,6,7,
      };
    
    static i32 move_real_index(i32 N, i32 m) {
      i32 table[3] = {0,N/2,N-1};
      return table[move_index(m)];
    }

    u32 fb_colors() const {
      u32 r = 0;
      FOR(i,12) if(is_fb[c[i]/2]) r |= 1<<i;
      return r;
    }

    u32 lr_colors() const {
      u32 r = 0;
      FOR(i,12) if(is_lr[c[i]/2]) r |= 1<<i;
      return r;
    }
  };

  const i32 EDGES2_COUNT = 24;
  const i32 EDGES2_MOVES_PER_AXIS = 4;
  extern u8 edges2_transition[EDGES2_MOVES_PER_AXIS * 9][2 * EDGES2_COUNT];
  struct edges2
    : cubies<EDGES2_COUNT, 2,
             EDGES2_MOVES_PER_AXIS,
             edges2_transition>
  {


  //             +-----------+
  //             |   0  1    |
  //             |7         2|
  //             |    d1     |
  //             |6         3|
  //             |   5  4    |
  // +-----------+-----------+-----------+-----------+
  // |   7  6    |   5  4    |   3  2    |   1  0    |
  // |16       19|19       18|18       17|17       16|
  // |    r1     |    f0     |    r0     |    f1     |
  // |20       23|23       22|22       21|21       20|
  // |   15 14   |   13 12   |   11 10   |   9  8    |
  // +-----------+-----------+-----------+-----------+
  //             |   13 12   |
  //             |14       11|
  //             |    d0     |
  //             |15       10|
  //             |   8  9    |
  //             +-----------+

    
    i32 cycle_distance(i32 typ, edges2 const& o) const {
      const i32 col[24] =
        {0,0,1,1,2,2,3,3,
         4,4,5,5,6,6,7,7,
         8,9,10,11,8,9,10,11};
      
      i32 at1[24], at2[24];
      FOR(i,N) at1[c[i]/2] = 2*col[i] + (c[i]&1);
      FOR(i,N) at2[o.c[i]/2] = 2*col[i] + (o.c[i]&1);

      i32 r = 0;
      FOR(i, N) if(at1[i] != at2[i]) r += 1;
      return r;
    }
    
    static constexpr i32 low[12]  = {0,3,4,7, 9,10,13,14, 17,22,19,20};
    static constexpr i32 high[12] = {1,2,5,6, 8,11,12,15, 21,18,23,16};
    static constexpr bool is_low[24] =
      {1,0,0,1, 1,0,0,1,
       0,1,1,0, 0,1,1,0,
       0,1,0,1, 1,0,1,0
      };
    static constexpr i32 unlow[24] =
      {0,0,1,1,2,2,3,3,
       4,4,5,5,6,6,7,7,
       8,9,10,11,8,9,10,11
      };

    bool is_split_lowhigh() const {
      u8 at[24];
      FOR(i,24) at[c[i]/2] = i;
      u32 mask = 0;
      FOR(i,12) mask |= 1<< unlow[at[low[i]]];
      return mask == 4095;
    }

    bool is_split_lowhigh_full() const {
      FOR(i,12) if(!is_low[c[low[i]]/2]) return 0;
      return 1;
    }

    static constexpr bool is_ud[24] =
      {
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        1,1,1,1,
        1,1,1,1,
      };

    static constexpr bool is_lr[24] =
      {
        1,1,0,0,
        1,1,0,0,
        1,1,0,0,
        1,1,0,0,
        0,0,0,0,
        0,0,0,0,
      };

    static constexpr bool is_fb[24] =
      {
        0,0,1,1,
        0,0,1,1,
        0,0,1,1,
        0,0,1,1,
        0,0,0,0,
        0,0,0,0,
      };

  //          +--------+
  //          |   0    |
  //          |3  d1  1|
  //          |   2    |
  // +--------+--------+--------+--------+
  // |   3    |   2    |   1    |    0   |
  // |11 r1 10|10 f0  9|9  r0  8|8  f1 11|
  // |   7    |   6    |   5    |    4   |
  // +--------+--------+--------+--------+
  //          |   6    |
  //          |7  d0  5|
  //          |   4    |
  //          +--------+

  //             +-----------+
  //             |   0  1    |
  //             |7         2|
  //             |    d1     |
  //             |6         3|
  //             |   5  4    |
  // +-----------+-----------+-----------+-----------+
  // |   7  6    |   5  4    |   3  2    |   1  0    |
  // |16       19|19       18|18       17|17       16|
  // |    r1     |    f0     |    r0     |    f1     |
  // |20       23|23       22|22       21|21       20|
  // |   15 14   |   13 12   |   11 10   |   9  8    |
  // +-----------+-----------+-----------+-----------+
  //             |   13 12   |
  //             |14       11|
  //             |    d0     |
  //             |15       10|
  //             |   8  9    |
  //             +-----------+

    u32 lr_colors() const {
      u32 r = 0;
      FOR(i,24) if(is_lr[c[i]/2]) r |= 1<<edges2_to_edges1[i];
      return r;
    }


  //          +--------+
  //          |   0    |
  //          |3  d1  1|
  //          |   2    |
  // +--------+--------+--------+--------+
  // |   3    |   2    |   1    |    0   |
  // |11 r1 10|10 f0  9|9  r0  8|8  f1 11|
  // |   7    |   6    |   5    |    4   |
  // +--------+--------+--------+--------+
  //          |   6    |
  //          |7  d0  5|
  //          |   4    |
  //          +--------+


  //             +-----------+
  //             |   0  1    |
  //             |7         2|
  //             |    d1     |
  //             |6         3|
  //             |   5  4    |
  // +-----------+-----------+-----------+-----------+
  // |   7  6    |   5  4    |   3  2    |   1  0    |
  // |16       19|19       18|18       17|17       16|
  // |    r1     |    f0     |    r0     |    f1     |
  // |20       23|23       22|22       21|21       20|
  // |   15 14   |   13 12   |   11 10   |   9  8    |
  // +-----------+-----------+-----------+-----------+
  //             |   13 12   |
  //             |14       11|
  //             |    d0     |
  //             |15       10|
  //             |   8  9    |
  //             +-----------+

    static constexpr i32 edges1_to_edges2[12] =
      {0,2,4,6, 8,10,12,14, 17,18,19,16};

    static constexpr i32 edges2_to_edges1[24] =
      {0,0,1,1,2,2,3,3,
       4,4,5,5,6,6,7,7,
       11,8,9,10,11,8,9,10};
    
    
    edges1 to_edges1() const {
      edges1 e;
      FOR(i,12) e.c[i] = 2 * edges2_to_edges1[c[low[i]]/2] + (c[low[i]]%2);
      return e;
    }
 
    edges1 to_edges1_low() const {
      edges1 e;
      FOR(i,12) e.c[i] = 2 * edges2_to_edges1[c[low[i]]/2] + (c[low[i]]%2);
      return e;
    }
  
    edges1 to_edges1_high() const {
      edges1 e;
      FOR(i,12) e.c[i] = 2 * edges2_to_edges1[c[high[i]]/2] + (c[high[i]]%2);
      return e;
    }
    
    static inline bool move_is_slice(i32 m) {
      i32 ix = move_index(m);
      return ix != 0 && ix != 1;
    }
       
    static i32 move_real_index(i32 N, i32 x, i32 m) {
      i32 X0 = 1+x, X1 = N-2-x;
      i32 table[4] = {0,N-1,X1,X0};
      return table[move_index(m)];
    }

    static inline u8 move_to_edges1(i32 m) {
      i32 axis = move_axis(m);
      i32 index = move_index(m);
      return (3 * axis + 2 * index) * 3 + (m%3);
    }
  };

  const i32 CENTERS1_COUNT = 6;
  const i32 CENTERS1_MOVES_PER_AXIS = 1;
  extern u8 centers1_transition[CENTERS1_MOVES_PER_AXIS * 9][CENTERS1_COUNT];
  struct centers1
    : cubies<CENTERS1_COUNT, 1,
             CENTERS1_MOVES_PER_AXIS,
             centers1_transition>
  {
   
    static i32 move_real_index(i32 N, i32) {
      return N/2;
    }
  };

  const i32 CENTERS2_COUNT = 24;
  const i32 CENTERS2_MOVES_PER_AXIS = 5;
  extern u8 centers2_transition[CENTERS2_MOVES_PER_AXIS * 9][CENTERS2_COUNT];
  struct centers2
    : cubies<CENTERS2_COUNT, 1,
             CENTERS2_MOVES_PER_AXIS,
             centers2_transition>
  {

    i32 cycle_distance(i32 typ, centers2 const& o) const {
      i32 at1[24], at2[24];
      FOR(i,N) at1[c[i]] = i;
      FOR(i,N) at2[o.c[i]] = i;
 
      i32 r = 0;
      if(typ == 0 || typ == 1 || typ == 2) {
        const i32 col[24] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5};
        FOR(i,N) if(col[at1[i]] != col[at2[i]]) r += 1;
      }else{
        FOR(i, N) if(at1[i] != at2[i]) r += 1;
      }
      return r;
    }

    static inline bool move_is_slice(i32 m) {
      i32 ix = move_index(m);
      return ix != 0 && ix != 3;
    }
    
    static i32 move_real_index(i32 N, i32 x, i32 m) {
      i32 X0 = 1+x, X1 = N-2-x;
      i32 table[5] = {0,X1,X0,N-1,N/2};
      return table[(m/3)%5];
    }

  };

  static inline
  u32 centers2_phase3_parity(centers2 const& c){
    u8 data[8] = {
      c.c[0],  c.c[1],  c.c[2],  c.c[3],
      c.c[12], c.c[13], c.c[14], c.c[15],
    };
    return get_permutation_parity(8, data);
  }
 
  static inline
  u32 centers2_phase4_parity(centers2 const& c){
    u8 data[8] = {
      c.c[4],  c.c[5],  c.c[6],  c.c[7],
      c.c[16], c.c[17], c.c[18], c.c[19],
    };
    return get_permutation_parity(8, data);
  }
  
  static inline
  u32 centers2_phase5_parity(centers2 const& c){
    u8 data[8] = {
      c.c[8],  c.c[9],  c.c[10], c.c[11],
      c.c[20], c.c[21], c.c[22], c.c[23],
    };
    return get_permutation_parity(8, data);
  }
 
  const i32 CENTERS3_COUNT = 24;
  const i32 CENTERS3_MOVES_PER_AXIS = 4;
  extern u8 centers3_transition[CENTERS3_MOVES_PER_AXIS * 9][CENTERS3_COUNT];
  struct centers3
    : cubies<CENTERS3_COUNT, 1,
             CENTERS3_MOVES_PER_AXIS,
             centers3_transition>
  {

    i32 cycle_distance(i32 typ, centers3 const& o) const {
      i32 at1[24], at2[24];
      FOR(i,N) at1[c[i]] = i;
      FOR(i,N) at2[o.c[i]] = i;
      
      i32 r = 0;
      if(typ == 0 || typ == 1) {
        const i32 col[24] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5};
        FOR(i,N) if(col[at1[i]] != col[at2[i]]) r += 1;
      }else if(typ == 2) {
        // TODO: depends on parity
        const i32 col[24] = {0,1,1,0, 2,3,3,2, 4,5,5,4, 6,7,7,6, 8,9,9,8, 10,11,11,10};
        FOR(i,N) if(col[at1[i]] != col[at2[i]]) r += 1;
      }else{
        FOR(i, N) if(at1[i] != at2[i]) r += 1;
      }
      return r;
    }

    static inline u8 move_to_centers2(i32 m) {
      i32 axis = move_axis(m);
      i32 index = move_index(m);
      const u8 table[4] = {0,3,1,2};
      return (5 * axis + table[index]) * 3 + (m%3);
    }

    static inline bool move_is_slice(i32 m) {
      i32 ix = move_index(m);
      return ix != 0 && ix != 1;
    }
  
    static i32 move_real_index(i32 N, i32 x, i32 m) {
      i32 X0 = 1+x, X1 = N-2-x;
      i32 table[4] = {0,N-1,X1,X0};
      return table[(m/3)%4];
    }
  };

  static inline
  u32 centers3_phase3_parity(centers3 const& c){
    u8 data[8] = {
      c.c[0],  c.c[1],  c.c[2],  c.c[3],
      c.c[12], c.c[13], c.c[14], c.c[15],
    };
    return get_permutation_parity(8, data);
  }
 
  static inline
  u32 centers3_phase4_parity(centers3 const& c){
    u8 data[8] = {
      c.c[4],  c.c[5],  c.c[6],  c.c[7],
      c.c[16], c.c[17], c.c[18], c.c[19],
    };
    return get_permutation_parity(8, data);
  }
  
  static inline
  u32 centers3_phase5_parity(centers3 const& c){
    u8 data[8] = {
      c.c[8],  c.c[9],  c.c[10], c.c[11],
      c.c[20], c.c[21], c.c[22], c.c[23],
    };
    return get_permutation_parity(8, data);
  }
  
  const i32 CENTERS4_COUNT = 24;
  const i32 CENTERS4_MOVES_PER_AXIS = 6;
  extern u8 centers4_transition[CENTERS4_MOVES_PER_AXIS * 9][CENTERS4_COUNT];
  struct centers4
    : cubies<CENTERS4_COUNT, 1,
             CENTERS4_MOVES_PER_AXIS,
             centers4_transition>
  {
 
    i32 cycle_distance(i32 typ, centers4 const& o) const {
      i32 at1[24], at2[24];
      FOR(i,N) at1[c[i]] = i;
      FOR(i,N) at2[o.c[i]] = i;
      
      i32 r = 0;
      if(typ == 0 || typ == 1) {
        const i32 col[24] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5};
        FOR(i,N) if(col[at1[i]] != col[at2[i]]) r += 1;
      }else if(typ == 2) { // only n = 6
        // TODO depends on parity / dimensions
        const i32 col[24] = {11,11,12,12, 21,21,22,22, 31,31,32,32, 41,41,42,42, 51,51,52,52, 61,61,62,62};
        FOR(i,N) if(col[at1[i]] != col[at2[i]]) r += 1;
      }else{
        FOR(i, N) if(at1[i] != at2[i]) r += 1;
      }
      return r;
    }
    
    static inline bool move_is_slice(i32 m) {
      i32 ix = move_index(m);
      return ix != 0 && ix != 5;
    }

    static i32 move_real_index(i32 N, i32 x, i32 y, i32 m) {
      i32 X0 = 1+x, X1 = N-2-x, Y0 = y+1, Y1 = N-2-y;
      i32 table[6] = {0,X1,Y1,X0,Y0,N-1};
      return table[(m/3)%6];
    }

    static inline
    i32 move_twin(i32 m) {
      switch((m/3)%6) {
      case 0: return m;
      case 1: return m+3;
      case 2: return m-3;
      case 3: return m+3;
      case 4: return m-3;
      case 5: return m;
      default: impossible();
      }
    }
  }; 

  struct centers2_brick_phase4 {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      i32 axis = centers2::move_axis(m);
      if(axis == 2 && centers2::move_index(m) == 1) mask ^= 0b01;
      if(axis == 2 && centers2::move_index(m) == 2) mask ^= 0b10;
    }

    void sym(i32 m) { }
  };
  
  struct centers2_brick_phase5 {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      i32 axis = centers2::move_axis(m);
      if(axis == 0 && centers2::move_index(m) == 1) mask ^= 0b0001;
      if(axis == 0 && centers2::move_index(m) == 2) mask ^= 0b0010;
      if(axis == 1 && centers2::move_index(m) == 1) mask ^= 0b0100;
      if(axis == 1 && centers2::move_index(m) == 2) mask ^= 0b1000;
    }

    void sym(i32 m) { }
  };
  
  struct centers3_brick_phase1 : with_moves<4> {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      if(m%3 == 1) return;
      i32 axis = centers3::move_axis(m);
      if(axis == 1 && centers3::move_index(m) == 2) mask ^= 1<< 0;
      if(axis == 1 && centers3::move_index(m) == 3) mask ^= 1<< 1;
      if(axis == 2 && centers3::move_index(m) == 2) mask ^= 1<< 2;
      if(axis == 2 && centers3::move_index(m) == 3) mask ^= 1<< 3;
    }

    void sym(i32 s) {
      if(s&1) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 3;
        if(mask & (1<< 1)) nmask ^= 1<< 2;
        if(mask & (1<< 2)) nmask ^= 1<< 0;
        if(mask & (1<< 3)) nmask ^= 1<< 1;
        mask = nmask;
      }
      if(s&2) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 0;
        if(mask & (1<< 1)) nmask ^= 1<< 1;
        if(mask & (1<< 2)) nmask ^= 1<< 3;
        if(mask & (1<< 3)) nmask ^= 1<< 2;
        mask = nmask;
      }
      if(s&4) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 1;
        if(mask & (1<< 1)) nmask ^= 1<< 0;
        if(mask & (1<< 2)) nmask ^= 1<< 2;
        if(mask & (1<< 3)) nmask ^= 1<< 3;
        mask = nmask;
      }
    }
  };

  struct centers3_brick_phase2 : with_moves<4> {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      if(m%3 == 1) return;
      i32 axis = centers3::move_axis(m);
      if(axis == 0 && centers3::move_index(m) == 2) mask ^= 1<< 0;
      if(axis == 0 && centers3::move_index(m) == 3) mask ^= 1<< 1;
    }

    void sym(i32 s) {
      if(s&2) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 1;
        if(mask & (1<< 1)) nmask ^= 1<< 0;
        mask = nmask;
      }
      if(s&4) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 1;
        if(mask & (1<< 1)) nmask ^= 1<< 0;
        mask = nmask;
      }
    }
  };

  struct centers3_brick_phase3 : with_moves<4> {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      i32 axis = centers3::move_axis(m);
      if(axis == 1 && centers3::move_index(m) == 2) mask ^= 1<< 0;
      if(axis == 1 && centers3::move_index(m) == 3) mask ^= 1<< 1;
      if(axis == 2 && centers3::move_index(m) == 2) mask ^= 1<< 2;
      if(axis == 2 && centers3::move_index(m) == 3) mask ^= 1<< 3;
    }

    void sym(i32 s) {
      if(s&1) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 3;
        if(mask & (1<< 1)) nmask ^= 1<< 2;
        if(mask & (1<< 2)) nmask ^= 1<< 0;
        if(mask & (1<< 3)) nmask ^= 1<< 1;
        mask = nmask;
      }
      if(s&2) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 0;
        if(mask & (1<< 1)) nmask ^= 1<< 1;
        if(mask & (1<< 2)) nmask ^= 1<< 3;
        if(mask & (1<< 3)) nmask ^= 1<< 2;
        mask = nmask;
      }
      if(s&4) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 1;
        if(mask & (1<< 1)) nmask ^= 1<< 0;
        if(mask & (1<< 2)) nmask ^= 1<< 2;
        if(mask & (1<< 3)) nmask ^= 1<< 3;
        mask = nmask;
      }
    }
  };

  struct centers3_brick_phase4 {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      i32 axis = centers3::move_axis(m);
      if(axis == 0 && centers3::move_index(m) == 2) mask ^= 0b0001;
      if(axis == 0 && centers3::move_index(m) == 3) mask ^= 0b0010;
      if(axis == 2 && centers3::move_index(m) == 2) mask ^= 0b0100;
      if(axis == 2 && centers3::move_index(m) == 3) mask ^= 0b1000;
    }

    void sym(i32){ }
  };

  struct centers3_brick_phase5 {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      i32 axis = centers3::move_axis(m);
      if(axis == 0 && centers3::move_index(m) == 2) mask ^= 0b0001;
      if(axis == 0 && centers3::move_index(m) == 3) mask ^= 0b0010;
      if(axis == 1 && centers3::move_index(m) == 2) mask ^= 0b0100;
      if(axis == 1 && centers3::move_index(m) == 3) mask ^= 0b1000;
    }

    void sym(i32){ }
  };


  struct centers4_brick_phase1 : with_moves<6> {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      if(m%3 == 1) return;
      i32 axis = centers4::move_axis(m);
      if(axis == 1 && centers4::move_index(m) == 1) mask ^= 1<< 0;
      if(axis == 1 && centers4::move_index(m) == 2) mask ^= 1<< 1;
      if(axis == 1 && centers4::move_index(m) == 3) mask ^= 1<< 2;
      if(axis == 1 && centers4::move_index(m) == 4) mask ^= 1<< 3;
      if(axis == 2 && centers4::move_index(m) == 1) mask ^= 1<< 4;
      if(axis == 2 && centers4::move_index(m) == 2) mask ^= 1<< 5;
      if(axis == 2 && centers4::move_index(m) == 3) mask ^= 1<< 6;
      if(axis == 2 && centers4::move_index(m) == 4) mask ^= 1<< 7;
    }

    void sym(i32 s) {
      if(s&1) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 6;
        if(mask & (1<< 1)) nmask ^= 1<< 7;
        if(mask & (1<< 2)) nmask ^= 1<< 4;
        if(mask & (1<< 3)) nmask ^= 1<< 5;
        if(mask & (1<< 4)) nmask ^= 1<< 0;
        if(mask & (1<< 5)) nmask ^= 1<< 1;
        if(mask & (1<< 6)) nmask ^= 1<< 2;
        if(mask & (1<< 7)) nmask ^= 1<< 3;
        mask = nmask;
      }
      if(s&2) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 0;
        if(mask & (1<< 1)) nmask ^= 1<< 1;
        if(mask & (1<< 2)) nmask ^= 1<< 2;
        if(mask & (1<< 3)) nmask ^= 1<< 3;
        if(mask & (1<< 4)) nmask ^= 1<< 6;
        if(mask & (1<< 5)) nmask ^= 1<< 7;
        if(mask & (1<< 6)) nmask ^= 1<< 4;
        if(mask & (1<< 7)) nmask ^= 1<< 5;
        mask = nmask;
      }
      if(s&4) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 2;
        if(mask & (1<< 1)) nmask ^= 1<< 3;
        if(mask & (1<< 2)) nmask ^= 1<< 0;
        if(mask & (1<< 3)) nmask ^= 1<< 1;
        if(mask & (1<< 4)) nmask ^= 1<< 4;
        if(mask & (1<< 5)) nmask ^= 1<< 5;
        if(mask & (1<< 6)) nmask ^= 1<< 6;
        if(mask & (1<< 7)) nmask ^= 1<< 7;
        mask = nmask;
      }
    }
  };

  struct centers4_brick_phase2 : with_moves<6> {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      if(m%3 == 1) return;
      i32 axis = centers4::move_axis(m);
      if(axis == 0 && centers4::move_index(m) == 1) mask ^= 1<< 0;
      if(axis == 0 && centers4::move_index(m) == 2) mask ^= 1<< 1;
      if(axis == 0 && centers4::move_index(m) == 3) mask ^= 1<< 2;
      if(axis == 0 && centers4::move_index(m) == 4) mask ^= 1<< 3;
    }

    void sym(i32 s) {
      if(s&2) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 2;
        if(mask & (1<< 1)) nmask ^= 1<< 3;
        if(mask & (1<< 2)) nmask ^= 1<< 0;
        if(mask & (1<< 3)) nmask ^= 1<< 1;
        mask = nmask;
      }
      if(s&4) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 2;
        if(mask & (1<< 1)) nmask ^= 1<< 3;
        if(mask & (1<< 2)) nmask ^= 1<< 0;
        if(mask & (1<< 3)) nmask ^= 1<< 1;
        mask = nmask;
      }
    }
  };

  struct centers4_brick_phase3 : with_moves<6> {
    u8 mask;

    void reset(){
      mask = 0;
    }

    void move(i32 m) {
      i32 axis = centers4::move_axis(m);
      if(axis == 1 && centers4::move_index(m) == 1) mask ^= 1<< 0;
      if(axis == 1 && centers4::move_index(m) == 2) mask ^= 1<< 1;
      if(axis == 1 && centers4::move_index(m) == 3) mask ^= 1<< 2;
      if(axis == 1 && centers4::move_index(m) == 4) mask ^= 1<< 3;
      if(axis == 2 && centers4::move_index(m) == 1) mask ^= 1<< 4;
      if(axis == 2 && centers4::move_index(m) == 2) mask ^= 1<< 5;
      if(axis == 2 && centers4::move_index(m) == 3) mask ^= 1<< 6;
      if(axis == 2 && centers4::move_index(m) == 4) mask ^= 1<< 7;
    }

    void sym(i32 s) {
      if(s&1) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 6;
        if(mask & (1<< 1)) nmask ^= 1<< 7;
        if(mask & (1<< 2)) nmask ^= 1<< 4;
        if(mask & (1<< 3)) nmask ^= 1<< 5;
        if(mask & (1<< 4)) nmask ^= 1<< 0;
        if(mask & (1<< 5)) nmask ^= 1<< 1;
        if(mask & (1<< 6)) nmask ^= 1<< 2;
        if(mask & (1<< 7)) nmask ^= 1<< 3;
        mask = nmask;
      }
      if(s&2) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 0;
        if(mask & (1<< 1)) nmask ^= 1<< 1;
        if(mask & (1<< 2)) nmask ^= 1<< 2;
        if(mask & (1<< 3)) nmask ^= 1<< 3;
        if(mask & (1<< 4)) nmask ^= 1<< 6;
        if(mask & (1<< 5)) nmask ^= 1<< 7;
        if(mask & (1<< 6)) nmask ^= 1<< 4;
        if(mask & (1<< 7)) nmask ^= 1<< 5;
        mask = nmask;
      }
      if(s&4) {
        u8 nmask = 0;
        if(mask & (1<< 0)) nmask ^= 1<< 2;
        if(mask & (1<< 1)) nmask ^= 1<< 3;
        if(mask & (1<< 2)) nmask ^= 1<< 0;
        if(mask & (1<< 3)) nmask ^= 1<< 1;
        if(mask & (1<< 4)) nmask ^= 1<< 4;
        if(mask & (1<< 5)) nmask ^= 1<< 5;
        if(mask & (1<< 6)) nmask ^= 1<< 6;
        if(mask & (1<< 7)) nmask ^= 1<< 7;
        mask = nmask;
      }
    }
  };

  const i32 move_length_table[4] = {0,1,2,1};
  
  struct rubik_move {
    i32 length;

    i32 N;
    u8 axis;
    u8 p[33];
    
    rubik_move() = delete;
    
    rubik_move(i32 N_, i32 axis_) {
      N = N_;
      reset(axis_);
    }

    void reset(i32 axis_){
      axis = axis_;
      FOR(i,N) p[i] = 0;
      length = 0;
    }
    
    void move(i32 index, i32 dir) {
      length -= move_length_table[p[index]];
      p[index] = (p[index]+dir)%4;
      length += move_length_table[p[index]];
    }

    void reverse(){
      FOR(i,N) p[i] = (4-p[i])%4;
    }

    strong_ordering operator<=>(const rubik_move& o) const {
      if(axis < o.axis) return strong_ordering::less;
      if(axis > o.axis) return strong_ordering::greater;
      FOR(i,N) {
        if(p[i] < o.p[i]) return strong_ordering::less;
        if(p[i] > o.p[i]) return strong_ordering::greater;
      }
      return strong_ordering::equal;
    }
    
    bool operator==(const rubik_move& o) const {
      if(axis != o.axis) return false;
      FOR(i,N) if(p[i] != o.p[i]) return false;
      return true;
    }

  };

  struct move_seq {
    i32 N;
    vector<rubik_move> moves;
    i32 length;

    move_seq() = delete;
    
    explicit move_seq(i32 N_) {
      N = N_;
      reset();
    }
    
    void reset(){
      moves.clear();
      length = 0;
    }

    void move(i32 axis, i32 index, i32 dir) {
      if(dir == 0) return;
      if(moves.empty() || moves.back().axis != axis){
        moves.eb(N,axis);
      }
      length -= moves.back().length;
      moves.back().move(index, dir);
      length += moves.back().length;
      if(moves.back().length == 0) moves.pop_back();
    }

    void move(rubik_move const& m) {
      FOR(i,N) move(m.axis, i, m.p[i]);
    }

    void move(move_seq const& M) {
      for(auto const& m : M.moves) move(m);
    }
      
    void reverse(){
      std::reverse(all(moves));
      for(auto& m : moves) m.reverse();
    }

    string to_alg_cubing_net() const {
      string out;
      for(auto const& m : moves) {
        FOR(i,N) if(m.p[i]) {
          out += to_string(i+1);
          out += "-";
          out += to_string(i+1);
          if(m.axis == 0) out += "u";
          if(m.axis == 1) out += "f";
          if(m.axis == 2) out += "r";
          if(m.p[i] == 2) out += "2";
          if(m.p[i] == 3) out += "'";
          out += " ";
        }
      }
      return out;
    }

    strong_ordering operator<=>(const move_seq& o) const {
      return moves <=> o.moves;
    }
    bool operator==(const move_seq& o) const {
      return moves == o.moves;
    }
  };

  move_seq convert_from_solution(i32 N, vector<string> const& S);
  vector<string> convert_to_solution(move_seq const& MS);
  void print_facelets(i32 N, vector<i32> const& A);
  
  struct rubik {
    i32 N;
    bool IS_ODD;
    i32 NHALF;
    
    struct corners corners;
    struct edges1 edges1;
    struct edges2 edges2[16];
    struct centers1 centers1;
    struct centers2 centers2[16];
    struct centers3 centers3[16];
    struct centers4 centers4[16][16];

    rubik(i32 N_) {
      N = N_;
      IS_ODD = N&1;
      NHALF = (N-2)/2;
      reset();
    }
    
    void reset(){
      corners.reset();
      if(IS_ODD) edges1.reset();
      FOR(i,NHALF) edges2[i].reset();
      if(IS_ODD) centers1.reset();
      if(IS_ODD) FOR(j,NHALF) centers2[j].reset();
      FOR(x,NHALF) centers3[x].reset();
      FOR(x,NHALF) FOR(y,NHALF) if(x != y) centers4[x][y].reset();
    }

    void print() {
      print_facelets(N, to_facelets());
    }
    
    vector<i32> to_facelets() const;
    i32 num_mismatches(i32 typ, rubik const& tgt) const {
      i32 x = 0;
      FOR(i,NHALF) x += 2 * edges2[i].cycle_distance(typ, tgt.edges2[i]);
      if(IS_ODD) FOR(j,NHALF) x += centers2[j].cycle_distance(typ, tgt.centers2[j]);
      FOR(i,NHALF) x += centers3[i].cycle_distance(typ, tgt.centers3[i]);
      FOR(i,NHALF) FOR(y,NHALF) if(i != y) x += centers4[i][y].cycle_distance(typ, tgt.centers4[i][y]);

      return x;
    }
   
    void move(i32 axis, i32 index, i32 dir);

    void move(rubik_move const& M) {
      FOR(i,N) move(M.axis, i, M.p[i]);
    }

    void move(move_seq const& M) {
      for(auto const& m : M.moves) move(m);
    }

    i32 centers4_distance(i32 typ) const {
      rubik solved(N); solved.reset();
      i32 d = 0;
      FOR(x,NHALF) FOR(y,NHALF) if(x != y) {
        d += centers4[x][y].cycle_distance(typ, solved.centers4[x][y]);
      }
      return d;
    }
  };

  struct fast_rubik_move {
    i32 length;

    u8  axis;
    u8  p[33];
    u64 current_date;
    u64 last[33];

    fast_rubik_move(){
      FOR(i,33) last[i] = 0;
    }

    void move(i32 index, i32 dir){
      if(last[index] != current_date) {
        last[index] = current_date;
        p[index] = dir;
        length += move_length_table[dir];
      }else{
        length -= move_length_table[p[index]];
        p[index] = (p[index] + dir) % 4;
        length += move_length_table[p[index]];
      }
    }
  };

  struct fast_move_seq {
    i32 N;
    vector<fast_rubik_move> moves;
    u64 nmoves;
    i32 length;
    u64 date;

    fast_move_seq() = delete;
    
    explicit fast_move_seq(i32 N_, i32 maxsz) {
      N = N_;
      nmoves = 0;
      length = 0;
      date = 0;
      moves.resize(maxsz);
    }

    void move(i32 axis, i32 index, i32 dir) {
      if(dir == 0) return;
      if(nmoves == 0 || moves[nmoves-1].axis != axis){
        // runtime_assert(nmoves + 1 < moves.size());
        // if(nmoves + 1 == moves.size()) moves.resize(1 + 2 * moves.size());
        date += 1;
        moves[nmoves].current_date = date;
        moves[nmoves].axis = axis;
        moves[nmoves].length = 0;
        nmoves += 1;
      }
      length -= moves[nmoves-1].length;
      moves[nmoves-1].move(index, dir);
      length += moves[nmoves-1].length;
      if(moves[nmoves-1].length == 0) nmoves -= 1;
    }

    void move(rubik_move const& m) {
      FOR(i,N) move(m.axis, i, m.p[i]);
    }

    void move(move_seq const& M) {
      for(auto const& m : M.moves) move(m);
    }

    move_seq to_move_seq() const {
      move_seq S(N);
      FOR(i,nmoves) {
        FOR(j, N) if(moves[i].last[j] == moves[i].current_date) {
          S.move(moves[i].axis, j, moves[i].p[j]);
        }
      }
      return S;
    }
  };
}
