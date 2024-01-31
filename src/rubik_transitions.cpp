#include "rubik.hpp"

namespace rubik{

  u8 corners_transition[corners::MOVES][3 * CORNERS_COUNT];
  u8 edges1_transition[edges1::MOVES][2 * EDGES1_COUNT];
  u8 edges2_transition[edges2::MOVES][2 * EDGES2_COUNT];
  u8 centers1_transition[centers1::MOVES][CENTERS1_COUNT];
  u8 centers2_transition[centers2::MOVES][CENTERS2_COUNT];
  u8 centers3_transition[centers3::MOVES][CENTERS3_COUNT];
  u8 centers4_transition[centers4::MOVES][CENTERS4_COUNT];

  //          Positions                                Solution State

  //          +--------+                               +--------+
  //          | 0    1 |                               | A    A |
  //          |   d1   |                               |   d1   |
  //          | 2    3 |                               | A    A |
  // +--------+--------+--------+--------+    +--------+--------+--------+--------+
  // | 16  17 | 4    5 | 8   9  | 12  13 |    | E    E | B    B | C    C | D    D |
  // |   r1   |   f0   |   r0   |   f1   |    |   r1   |   f0   |   r0   |   f1   |
  // | 18  19 | 6    7 | 10  11 | 14  15 |    | E    E | B    B | C    C | D    D |
  // +--------+--------+--------+--------+    +--------+--------+--------+--------+
  //          | 20  21 |                               | F    F |
  //          |   d0   |                               |   d0   |
  //          | 22  23 |                               | F    F |
  //          +--------+                               +--------+


  //          +--------+
  //          | 0    1 |
  //          |   d1   |
  //          | 3    2 |
  // +--------+--------+--------+--------+
  // | 0    3 | 3    2 | 2   1  | 1   0  |
  // |   r1   |   f0   |   r0   |   f1   |
  // | 4    7 | 7    6 | 6   5  | 5   4  |
  // +--------+--------+--------+--------+
  //          | 7    6 |
  //          |   d0   |
  //          | 4    5 |
  //          +--------+

  static i32 corners_cycle[6][4] =
  { { 1,2,3,0 },
    { 4,5,6,7 },
    { 3,2,6,7 },
    { 0,1,5,4 },
    { 2,1,5,6 },
    { 4,7,3,0 },
  };
  static i32 corners_change[6][4] =
    { {0,0,0,0},
      {0,0,0,0},
      {1,2,1,2},
      {2,1,2,1},
      {1,2,1,2},
      {2,1,2,1},
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

  static i32 edges1_cycle[9][4] =
    { { 0,1,2,3 },
      { 9,10,11,8 },
      { 6,7,4,5 },
      { 2,9,6,10 },
      { 1,5,7,3 },
      { 8,4,11,0 },
      { 1,8,5,9 },
      { 0,4,6,2 },
      { 3,11,7,10 },
    };
 
  static i32 edges1_change[9][4] =
    { { 0,0,0,0 },
      { 1,1,1,1 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      { 1,1,1,1 },
      { 0,0,0,0 },
      { 1,1,1,1 },
      { 1,1,1,1 },
      { 1,1,1,1 },
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

  static i32 edges2_cycle1[6][2][4] =
    { { {0,2,4,6}, {1,3,5,7} },
      { {13,15,9,11}, {12,14,8,10} },
      { {5,18,12,23}, {4,22,13,19} },
      { {1,21,8,16}, {0,17,9,20} },
      { {3,17,10,22}, {2,21,11,18} },
      { {7,20,14,19}, {6,16,15,23} },
    };
 
  static i32 edges2_change1[6][2][4] =
    { { {0,0,0,0}, {0,0,0,0} },
      { {0,0,0,0}, {0,0,0,0} },
      { {0,0,0,0}, {0,0,0,0} },
      { {0,0,0,0}, {0,0,0,0} },
      { {1,1,1,1}, {1,1,1,1} },
      { {1,1,1,1}, {1,1,1,1} },
    };
 
  static i32 edges2_cycle2[6][4] =
    { {20,21,22,23},
      {16,17,18,19},
      {2,10,15,7},
      {3,11,14,6},
      {0,8,13,5},
      {1,9,12,4},
    };
  
  static i32 edges2_change2[6][4] =
    { {1,1,1,1},
      {1,1,1,1},
      {1,1,1,1},
      {1,1,1,1},
      {1,1,1,1},
      {1,1,1,1},
    };
  
  static i32 centers1_cycle[3][4] =
    { {1,4,3,2},
      {0,2,5,4},
      {0,3,5,1}
    };

  //               y      
  //             +-----------+
  //             |     0     |
  //         x   |           |
  //             | 3  d0   1 |
  //             |           |
  //             |     2     |
  // +-----------+-----------+-----------+-----------+
  // |    20     |     4     |     8     |    16     |
  // |           |           |           |           |
  // |23  r1   21| 7  f0   5 |11  r0   9 |19  f1  17 |
  // |           |           |           |           |
  // |    22     |     6     |    10     |    18     |
  // +-----------+-----------+-----------+-----------+
  //             |    12     |
  //             |           |
  //             |15  d1  13 |
  //             |           |
  //             |    14     |
  //             +-----------+

  static i32 centers2_cycle1[12][4] =
    { {0,1,2,3},
      {6,22,18,10},
      {4,20,16,8},
      {12,15,14,13},

      {4,5,6,7},
      {0,9,14,23},     
      {2,11,12,21},
      {16,19,18,17},

      {8,9,10,11},
      {3,17,15,7},
      {1,19,13,5},
      {20,23,22,21},
    };

  static i32 centers2_cycle2[3][2][4] =
    { {{17,9,5,21}, {19,11,7,23}},
      {{3,8,13,22}, {1,10,15,20}},
      {{2,16,14,6}, {0,18,12,4}},
    };

  //               y      
  //             +-----------+
  //             |           |
  //         x   |  0     1  |
  //             |    d0     |
  //             |  3     2  |
  //             |           |
  // +-----------+-----------+-----------+-----------+
  // |           |           |           |           |
  // | 20    21  |  4     5  |  8    9   | 16    17  |
  // |    r1     |    f0     |    r0     |    f1     |
  // | 23    22  |  7     6  |  11   10  | 19    18  |
  // |           |           |           |           |
  // +-----------+-----------+-----------+-----------+
  //             |           |
  //             |  12   13  |
  //             |    d1     |
  //             |  15   14  |
  //             |           |
  //             +-----------+

  static i32 centers3_cycle1[12][4] =
    { {0,1,2,3},
      {12,15,14,13},
      {4,5,6,7},
      {16,19,18,17},
      {8,9,10,11},
      {20,23,22,21},
    };
 
  static i32 centers3_cycle2[16][2][4] =
    {
      {{7,23,19,11}, {6,22,18,10}},
      {{4,20,16,8}, {5,21,17,9}},
      {{0,9,14,23}, {1,10,15,20}},
      {{3,8,13,22}, {2,11,12,21}},
      {{3,17,15,7}, {0,18,12,4}},
      {{2,16,14,6}, {1,19,13,5}},
    };
  
  //               y      
  //             +-----------+
  //             |        1  |
  //         x   | 0         |
  //             |    d0     |
  //             |         2 |
  //             |  3        |
  // +-----------+-----------+-----------+-----------+
  // |       21  |        5  |        9  |       17  |
  // |20         | 4         | 8         |16         |
  // |    r1     |    f0     |    r0     |    f1     |
  // |         22|         6 |         10|         18|
  // |  23       |   7       |  11       |  19       |
  // +-----------+-----------+-----------+-----------+
  //             |       13  |
  //             | 12        |
  //             |    d1     |
  //             |         14|
  //             |   15      |
  //             +-----------+
 
  static i32 centers4_cycle[18][4] =
    { {0,1,2,3},
      {18,10,6,22},
      {19,11,7,23},
      {16,8,4,20},
      {17,9,5,21},
      {15,14,13,12},
      
      {4,5,6,7},
      {0,9,14,23},
      {1,10,15,20},
      {2,11,12,21},
      {3,8,13,22},
      {17,16,19,18},

      {8,9,10,11},
      {7,3,17,15},
      {4,0,18,12},
      {5,1,19,13},
      {6,2,16,14},
      {21,20,23,22},
    };

  void init_transitions(){
    { // CORNERS
      FOR(m, corners::MOVES) FOR(i, 3 * corners::N) {
        corners_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(i, 2) {
          FOR(t, 3) {
            i32 m = (f*2 + i)*3 + t;

            FOR(j,4) {
              i32 a = corners_cycle[f*2+i][j];
              i32 b = corners_cycle[f*2+i][(j+t+1)%4];
              FOR(oa,3) {
                i32 ob = oa;
                if(t != 1) ob = (oa + corners_change[f*2+i][j]) % 3;
                corners_transition[m][a*3+oa] = b*3+ob;
              }
            }
          }
        }
      }
    }
    
    { // EDGES1
      FOR(m, edges1::MOVES) FOR(i, 2 * edges1::N) {
        edges1_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(i, 3) {
          FOR(t, 3) {
            i32 m = (f*3 + i)*3 + t;

            FOR(j,4) {
              i32 a = edges1_cycle[f*3+i][j];
              i32 b = edges1_cycle[f*3+i][(j+t+1)%4];
              FOR(oa,2) {
                i32 ob = oa;
                if(t != 1) ob = (oa + edges1_change[f*3+i][j]) % 2;
                edges1_transition[m][a*2+oa] = b*2+ob;
              }
            }
          }
        }
      }
    }

    { // EDGES2
      FOR(m, edges2::MOVES) FOR(i, 2 * edges2::N) {
        edges2_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(i, 2) {
          FOR(t, 3) {
            i32 m = (f*4 + i)*3 + t;

            FOR(x,2) FOR(j,4) {
              i32 a = edges2_cycle1[f*2+i][x][j];
              i32 b = edges2_cycle1[f*2+i][x][(j+t+1)%4];
              FOR(oa,2) {
                i32 ob = oa;
                if(t != 1) ob = (oa + edges2_change1[f*2+i][x][j]) % 2;
                edges2_transition[m][a*2+oa] = b*2+ob;
              }
            }
          }
        }

        FOR(i,2) FOR(t,3) {
          i32 m = (f*4+2+i)*3 + t;

          FOR(j,4) {
            i32 a = edges2_cycle2[f*2+i][j];
            i32 b = edges2_cycle2[f*2+i][(j+t+1)%4];
            FOR(oa,2) {
              i32 ob = oa;
              if(t != 1) ob = (oa + edges2_change2[f*2+i][j]) % 2;
              edges2_transition[m][a*2+oa] = b*2+ob;
            }
          }
        }
      }
    }


    { // CENTERS1
      FOR(m, centers1::MOVES) FOR(i, centers1::N) {
        centers1_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(t, 3) {
          i32 m = f*3 + t;

          FOR(j,4) {
            i32 a = centers1_cycle[f][j];
            i32 b = centers1_cycle[f][(j+t+1)%4];
            centers1_transition[m][a] = b;
          }
        }
      }
    }

    { // CENTERS2
      FOR(m, centers2::MOVES) FOR(i, centers2::N) {
        centers2_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(i,4) {
          FOR(t, 3) {
            i32 m = (5*f+i)*3 + t;
            
            FOR(j,4) {
              i32 a = centers2_cycle1[4*f+i][j];
              i32 b = centers2_cycle1[4*f+i][(j+t+1)%4];
              centers2_transition[m][a] = b;
            }
          }
        }

        FOR(t,3) {
          i32 m = (5*f+4)*3 + t;
          FOR(x,2) FOR(j,4) {
            i32 a = centers2_cycle2[f][x][j];
            i32 b = centers2_cycle2[f][x][(j+t+1)%4];
            centers2_transition[m][a] = b;
          }
        }

      }
    }
    { // CENTERS3
      // static u8 centers3_transition[centers3::MOVES][NCENTERS3];
      FOR(m, centers3::MOVES) FOR(i, centers3::N) {
        centers3_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(i,2) {
          FOR(t, 3) {
            i32 m = (4*f+i)*3 + t;
            
            FOR(j,4) {
              i32 a = centers3_cycle1[2*f+i][j];
              i32 b = centers3_cycle1[2*f+i][(j+t+1)%4];
              centers3_transition[m][a] = b;
            }
          }
        }

        FOR(i,2) {
          FOR(t,3) {
            i32 m = (4*f+2+i)*3 + t;
            FOR(x,2) FOR(j,4) {
              i32 a = centers3_cycle2[2*f+i][x][j];
              i32 b = centers3_cycle2[2*f+i][x][(j+t+1)%4];
              centers3_transition[m][a] = b;
            }
          }
        }
      }
    }
    
    { // CENTERS4
      // static u8 centers4_transition[centers4::MOVES][NCENTERS4];
      FOR(m, centers4::MOVES) FOR(i, centers4::N) {
        centers4_transition[m][i] = i;
      }

      FOR(f, 3) {
        FOR(i,6) {
          FOR(t, 3) {
            i32 m = (6*f+i)*3 + t;
            
            FOR(j,4) {
              i32 a = centers4_cycle[6*f+i][j];
              i32 b = centers4_cycle[6*f+i][(j+t+1)%4];
              centers4_transition[m][a] = b;
            }
          }
        }
      }
    }
  }

}
