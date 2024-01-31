#include "rubik.hpp"
#include "rubik_tables.hpp"
#include "util.hpp"

namespace rubik{

  move_seq convert_from_solution(i32 N, vector<string> const& S){
    move_seq M(N);
    for(auto s : S) {
      bool inv = (s[0] == '-');
      char c = inv ? s[1] : s[0];
      i32 x = 0;
      FORU(i,inv?2:1,(i32)s.size()-1){
        x = 10*x + (s[i]-'0');
      }
      i32 axis = -1;
      if(c == 'd') axis = 0;
      if(c == 'f') axis = 1;
      if(c == 'r') axis = 2;
      i32 index = axis == 0 ? (N-1-x) : x;
      i32 dir = (inv ^ (axis == 0)) ? 3 : 1;
      M.move(axis, index, dir);
    }
    return M;
  }

  vector<string> convert_to_solution(move_seq const& MS){
    i32 N = MS.N;
    vector<string> S;
    for(auto const& M : MS.moves) {
      char c = ' ';
      if(M.axis == 0) c = 'd';
      if(M.axis == 1) c = 'f';
      if(M.axis == 2) c = 'r';
      string inv1,inv3;
      if(M.axis == 0) { inv1 = "-"; inv3 = ""; }
      else { inv1 = ""; inv3 = "-"; }
      FOR(i,N) {
        i32 index = M.axis == 0 ? N-1-i : i;
        if(M.p[i] == 1) {
          S.eb(inv1 + c + to_string(index));
        }else if(M.p[i] == 2) {
          S.eb(inv1 + c + to_string(index));
          S.eb(inv1 + c + to_string(index));
        }else if(M.p[i] == 3){
          S.eb(inv3 + c + to_string(index));
        }
      }
    }
    return S;
  }

  vector<i32> rubik::to_facelets() const {
    vector<i32> X(6*N*N+1, -1);

    // TODO: generate color from pos + target
      
    // corners
    { i32 A = N-1;
      i32 corners_color[8][3] =
        { {0,4,3},
          {0,3,2},
          {0,2,1},
          {0,1,4},
          {5,3,4},
          {5,2,3},
          {5,1,2},
          {5,4,1},
        };
      i32 corners_pos[8][3] =
        { {0*N*N+0*N+0, 4*N*N+0*N+0, 3*N*N+0*N+A},
          {0*N*N+0*N+A, 3*N*N+0*N+0, 2*N*N+0*N+A},
          {0*N*N+A*N+A, 2*N*N+0*N+0, 1*N*N+0*N+A},
          {0*N*N+A*N+0, 1*N*N+0*N+0, 4*N*N+0*N+A},
          {5*N*N+A*N+0, 3*N*N+A*N+A, 4*N*N+A*N+0},
          {5*N*N+A*N+A, 2*N*N+A*N+A, 3*N*N+A*N+0},
          {5*N*N+0*N+A, 1*N*N+A*N+A, 2*N*N+A*N+0},
          {5*N*N+0*N+0, 4*N*N+A*N+A, 1*N*N+A*N+0},
        };

      FOR(i,8) {
        i32 p = corners.c[i] / 3, o = corners.c[i] % 3;
        FOR(j,3) X[corners_pos[p][(o+j)%3]] = corners_color[i][j];
      }
    }
      
    // edges1
    if(IS_ODD) {
      i32 A = N/2;
      i32 B = N-1;
      i32 edges1_color[12][2] =
        { {0,3},
          {0,2},
          {0,1},
          {0,4},
          {5,3},
          {5,2},
          {5,1},
          {5,4},
          {2,3},
          {2,1},
          {4,1},
          {4,3},
        };
      i32 edges1_pos[12][2] =
        { {0*N*N+0*N+A, 3*N*N+0*N+A},
          {0*N*N+A*N+B, 2*N*N+0*N+A},
          {0*N*N+B*N+A, 1*N*N+0*N+A},
          {0*N*N+A*N+0, 4*N*N+0*N+A},
          {5*N*N+B*N+A, 3*N*N+B*N+A},
          {5*N*N+A*N+B, 2*N*N+B*N+A},
          {5*N*N+0*N+A, 1*N*N+B*N+A},
          {5*N*N+A*N+0, 4*N*N+B*N+A},
          {2*N*N+A*N+B, 3*N*N+A*N+0},
          {2*N*N+A*N+0, 1*N*N+A*N+B},
          {4*N*N+A*N+B, 1*N*N+A*N+0},
          {4*N*N+A*N+0, 3*N*N+A*N+B},
        };

      FOR(i,12) {
        i32 p = edges1.c[i] / 2, o = edges1.c[i] % 2;
        FOR(j,2) X[edges1_pos[p][(o+j)%2]] = edges1_color[i][j];
      }
    }

    FOR(x,NHALF) {
      i32 X0 = 1+x, X1 = N-2-x, B = N-1;
      i32 edges2_color[24][2] =
        { {0,3},
          {0,3},
          {0,2},
          {0,2},
          {0,1},
          {0,1},
          {0,4},
          {0,4},
          {5,3},
          {5,3},
          {5,2},
          {5,2},
          {5,1},
          {5,1},
          {5,4},
          {5,4},

          {4,3},
          {2,3},
          {2,1},
          {4,1},
          {4,3},
          {2,3},
          {2,1},
          {4,1},
        };
      i32 edges2_pos[24][2] =
        { {0*N*N+0*N+X0, 3*N*N+0*N+X1},
          {0*N*N+0*N+X1, 3*N*N+0*N+X0},
          {0*N*N+X0*N+B, 2*N*N+0*N+X1},
          {0*N*N+X1*N+B, 2*N*N+0*N+X0},

          {0*N*N+B*N+X1, 1*N*N+0*N+X1},
          {0*N*N+B*N+X0, 1*N*N+0*N+X0},
          {0*N*N+X1*N+0, 4*N*N+0*N+X1},
          {0*N*N+X0*N+0, 4*N*N+0*N+X0},

          {5*N*N+B*N+X0, 3*N*N+B*N+X1},
          {5*N*N+B*N+X1, 3*N*N+B*N+X0},
          {5*N*N+X1*N+B, 2*N*N+B*N+X1},
          {5*N*N+X0*N+B, 2*N*N+B*N+X0},

          {5*N*N+0*N+X1, 1*N*N+B*N+X1},
          {5*N*N+0*N+X0, 1*N*N+B*N+X0},
          {5*N*N+X0*N+0, 4*N*N+B*N+X1},
          {5*N*N+X1*N+0, 4*N*N+B*N+X0},

          {4*N*N+X0*N+0, 3*N*N+X0*N+B},
          {2*N*N+X0*N+B, 3*N*N+X0*N+0},
          {2*N*N+X0*N+0, 1*N*N+X0*N+B},
          {4*N*N+X0*N+B, 1*N*N+X0*N+0},

          {4*N*N+X1*N+0, 3*N*N+X1*N+B},
          {2*N*N+X1*N+B, 3*N*N+X1*N+0},
          {2*N*N+X1*N+0, 1*N*N+X1*N+B},
          {4*N*N+X1*N+B, 1*N*N+X1*N+0},
        };


      FOR(i,24) {
        i32 p = edges2[x].c[i] / 2, o = edges2[x].c[i] % 2;
        FOR(j,2) X[edges2_pos[p][(o+j)%2]] = edges2_color[i][j];
      }
        
    }
      
    // centers 1
    if(IS_ODD) {
      i32 A = N/2;
      i32 center_color[6] = {
        0,1,2,3,4,5
      };
      i32 center_pos[6] = {
        0*N*N + A*N+A,
        1*N*N + A*N+A,
        2*N*N + A*N+A,
        3*N*N + A*N+A,
        4*N*N + A*N+A,
        5*N*N + A*N+A,
      };
      FOR(i,6) X[center_pos[centers1.c[i]]] = center_color[i];
    }

    if(IS_ODD) {
      FOR(x,NHALF) {
        i32 X0 = 1+x, X1 = N-2-x, A = N/2;
        i32 center_color[24] = {
          0,0,0,0,
          1,1,1,1,
          2,2,2,2,
          5,5,5,5,
          3,3,3,3,
          4,4,4,4,
        };
        i32 center_pos[24] = {
          0*N*N + X0*N + A, 0*N*N + A*N + X1, 0*N*N + X1*N + A, 0*N*N + A*N + X0,
          1*N*N + X0*N + A, 1*N*N + A*N + X1, 1*N*N + X1*N + A, 1*N*N + A*N + X0,
          2*N*N + X0*N + A, 2*N*N + A*N + X1, 2*N*N + X1*N + A, 2*N*N + A*N + X0,
          5*N*N + X0*N + A, 5*N*N + A*N + X1, 5*N*N + X1*N + A, 5*N*N + A*N + X0,
          3*N*N + X0*N + A, 3*N*N + A*N + X1, 3*N*N + X1*N + A, 3*N*N + A*N + X0,
          4*N*N + X0*N + A, 4*N*N + A*N + X1, 4*N*N + X1*N + A, 4*N*N + A*N + X0,
        };
        FOR(i,24) X[center_pos[centers2[x].c[i]]] = center_color[i];
      }
    }
      
    // centers3
    FOR(x,NHALF) {
      i32 X0 = 1+x, Y0 = 1+x, X1 = N-2-x, Y1 = N-2-x;
      // i32 A = N/2;
      i32 center_color[24] = {
        0,0,0,0,
        1,1,1,1,
        2,2,2,2,
        5,5,5,5,
        3,3,3,3,
        4,4,4,4,
      };
      i32 center_pos[24] = {
        0*N*N + X0*N + Y0, 0*N*N + Y0*N + X1, 0*N*N + X1*N + Y1, 0*N*N + Y1*N + X0,
        1*N*N + X0*N + Y0, 1*N*N + Y0*N + X1, 1*N*N + X1*N + Y1, 1*N*N + Y1*N + X0,
        2*N*N + X0*N + Y0, 2*N*N + Y0*N + X1, 2*N*N + X1*N + Y1, 2*N*N + Y1*N + X0,
        5*N*N + X0*N + Y0, 5*N*N + Y0*N + X1, 5*N*N + X1*N + Y1, 5*N*N + Y1*N + X0,
        3*N*N + X0*N + Y0, 3*N*N + Y0*N + X1, 3*N*N + X1*N + Y1, 3*N*N + Y1*N + X0,
        4*N*N + X0*N + Y0, 4*N*N + Y0*N + X1, 4*N*N + X1*N + Y1, 4*N*N + Y1*N + X0,
      };
      FOR(i,24) X[center_pos[centers3[x].c[i]]] = center_color[i];
    }
 
    // centers4
    FOR(x,NHALF) FOR(y,NHALF) if(x != y) {
      i32 X0 = 1+x, Y0 = 1+y, X1 = N-2-x, Y1 = N-2-y;
      i32 center_color[24] = {
        0,0,0,0,
        1,1,1,1,
        2,2,2,2,
        5,5,5,5,
        3,3,3,3,
        4,4,4,4,
      };
      i32 center_pos[24] = {
        0*N*N + X0*N + Y0, 0*N*N + Y0*N + X1, 0*N*N + X1*N + Y1, 0*N*N + Y1*N + X0,
        1*N*N + X0*N + Y0, 1*N*N + Y0*N + X1, 1*N*N + X1*N + Y1, 1*N*N + Y1*N + X0,
        2*N*N + X0*N + Y0, 2*N*N + Y0*N + X1, 2*N*N + X1*N + Y1, 2*N*N + Y1*N + X0,
        5*N*N + X0*N + Y0, 5*N*N + Y0*N + X1, 5*N*N + X1*N + Y1, 5*N*N + Y1*N + X0,
        3*N*N + X0*N + Y0, 3*N*N + Y0*N + X1, 3*N*N + X1*N + Y1, 3*N*N + Y1*N + X0,
        4*N*N + X0*N + Y0, 4*N*N + Y0*N + X1, 4*N*N + X1*N + Y1, 4*N*N + Y1*N + X0,
      };
      FOR(i,24) X[center_pos[centers4[x][y].c[i]]] = center_color[i];
    }


    X.resize(6*N*N);
    return X;
  }
  
  void rubik::move(i32 axis, i32 index, i32 dir) {
    if(dir == 0) return;
    dir -= 1;
    if(index == 0) {
      corners.move((2*axis)*3 + dir);
      if(IS_ODD) edges1.move((3*axis)*3 + dir);
      FOR(x,NHALF) {
        edges2[x].move((4*axis)*3 + dir);
      }
      if(IS_ODD) FOR(x,NHALF) {
          centers2[x].move((5*axis)*3 + dir);
        }
      FOR(x,NHALF) FOR(y,NHALF) {
        if(x == y) {
          centers3[x].move((4*axis)*3 + dir);
        }else{
          centers4[x][y].move((6*axis)*3 + dir);
        }
      }
    }else if(index == N-1) {
      corners.move((2*axis+1)*3 + dir);
      if(IS_ODD) edges1.move((3*axis+2)*3 + dir);
      FOR(x,NHALF) {
        edges2[x].move((4*axis + 1)*3 + dir);
      }
      if(IS_ODD) FOR(x,NHALF) {
          centers2[x].move((5*axis + 3)*3 + dir);
        }
      FOR(x,NHALF) FOR(y,NHALF) {
        if(x == y) {
          centers3[x].move((4*axis + 1)*3 + dir);
        }else{
          centers4[x][y].move((6*axis + 5)*3 + dir);
        }
      }
    }else if(IS_ODD && index == N/2){
      centers1.move(axis*3+dir);
      edges1.move((3*axis+1)*3 + dir);
      FOR(x,NHALF) {
        centers2[x].move((5 * axis + 4)*3 + dir);
      }
    }else{
      bool low = (index < N/2);
      i32 x = (low ? index : N-1-index) - 1;
      edges2[x].move((4*axis + 2 + low)*3 + dir);
      if(IS_ODD) centers2[x].move((5*axis + 1 + low) * 3 + dir);
      FOR(y,NHALF) {
        if(x == y) {
          centers3[x].move((4*axis + 2 + low)*3 + dir);
        }else{
          centers4[x][y].move((6*axis + 1 + 2*low)*3 + dir);
          centers4[y][x].move((6*axis + 1 + 2*low + 1)*3 + dir);
        }
      }
    }
  }
  
}
