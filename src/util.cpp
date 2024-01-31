#include "util.hpp"

u64 fact[100];
u64 cnk[100][100];

void init_cnk(){
  fact[0] = 1;
  FOR(x,99) fact[x+1] = (x+1) * fact[x];

  FOR(x,100) {
    cnk[x][0] = cnk[x][x] = 1;
    FORU(y,1,x-1) cnk[x][y] = cnk[x-1][y-1] + cnk[x-1][y];
  }
}

void init_util(){
  init_cnk();
}
