#include "rubik.hpp"

namespace rubik{

  void print_facelet(i32 i) {
    const string BBLK = "\e[1;30m";
    const string BRED = "\e[1;31m";
    const string BGRN = "\e[1;32m";
    const string BYEL = "\e[1;33m";
    const string BBLU = "\e[1;34m";
    const string BMAG = "\e[1;35m";
    const string BCYN = "\e[1;36m";
    const string BWHT = "\e[1;37m";

    const string BLKB = "\e[40m";
    const string REDB = "\e[41m";
    const string GRNB = "\e[42m";
    const string YELB = "\e[43m";
    const string BLUB = "\e[44m";
    const string MAGB = "\e[45m";
    const string CYNB = "\e[46m";
    const string WHTB = "\e[47m";

    const string BLKHB = "\e[0;100m";
    const string REDHB = "\e[0;101m";
    const string GRNHB = "\e[0;102m";
    const string YELHB = "\e[0;103m";
    const string BLUHB = "\e[0;104m";
    const string MAGHB = "\e[0;105m";
    const string CYNHB = "\e[0;106m";
    const string WHTHB = "\e[0;107m";
    const string CRESET = "\e[0m";
    // if(i == 0) cout << BWHT << 'U' << CRESET;
    // else if(i == 1) cout << BGRN << 'F' << CRESET;
    // else if(i == 2) cout << BMAG << 'R' << CRESET;
    // else if(i == 3) cout << BBLU << 'B' << CRESET;
    // else if(i == 4) cout << BRED << 'L' << CRESET;
    // else if(i == 5) cout << BYEL << 'D' << CRESET;
    if(i == 0) cout << BWHT << '0' << CRESET;
    else if(i == 1) cout << BGRN << '1' << CRESET;
    else if(i == 2) cout << BMAG << '2' << CRESET;
    else if(i == 3) cout << BBLU << '3' << CRESET;
    else if(i == 4) cout << BRED << '4' << CRESET;
    else if(i == 5) cout << BYEL << '5' << CRESET;
    else cout << 'X';

  }
  
  void print_facelets(i32 N, const vector<i32> &A) {
    {
      cout << ' ';
      FOR(y,N) cout << ' ';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      cout << endl;
    }
    FOR(x,N) {
      cout << ' ';
      FOR(y,N) cout << ' ';
      cout << '|';
      FOR(y,N) print_facelet(A[x*N+y]);
      cout << '|';
      cout << endl;
    }
    {
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      cout << endl;
    }
    FOR(x,N) {
      cout << '|';
      FOR(y,N) print_facelet(A[4*N*N+x*N+y]);
      cout << '|';
      FOR(y,N) print_facelet(A[1*N*N+x*N+y]);
      cout << '|';
      FOR(y,N) print_facelet(A[2*N*N+x*N+y]);
      cout << '|';
      FOR(y,N) print_facelet(A[3*N*N+x*N+y]);
      cout << '|';
      cout << endl;
    }
    {
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      cout << endl;
    }
    FOR(x,N) {
      cout << ' ';
      FOR(y,N) cout << ' ';
      cout << '|';
      FOR(y,N) print_facelet(A[5*N*N+x*N+y]);
      cout << '|';
      cout << endl;
    }
    {
      cout << ' ';
      FOR(y,N) cout << ' ';
      cout << '+';
      FOR(y,N) cout << '-';
      cout << '+';
      cout << endl;
    }
  }

}
