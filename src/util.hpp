#pragma once
#include "header.hpp"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern u64 fact[100];
extern u64 cnk[100][100];

static inline
u64 get_permutation_index(i32 n, u8 const* data) {
  u8 data2[n];
  FOR(i,n) {
    data2[i] = 0;
    FOR(j,n) data2[i] += data[j]<data[i];
  }

  u64 index = 0, f = fact[n];
  FOR(i, n) {
    f /= (n-i);
    i32 x = data2[i];
    FOR(j,i) if(data2[j] < data2[i]) x -= 1;
    index += x * f;
  }
  return index;
}

static inline
u8 get_permutation_parity(i32 n, u8 const* data) {
  u8 x = 0;
  FOR(j,n) FOR(i,j) if(data[i] > data[j]) x ^= 1;
  return x;
}

static inline
u64 get_binomial_index(u64 n, u64 k, u64 mask) {
  u64 index = 0;
  while(n != k) {
    n -= 1;
    if(mask&(1<<n)) {
      index += cnk[n][k];
      k -= 1;
    }else{
    }
  }
  return index;
}

template<class F>
void try_load_table_copy(vector<tuple<string, char*, size_t>> const& data, F&& f) {
  bool found_all = true;
  for(auto [filename, buf, sz] : data) {
    found_all = found_all && std::filesystem::exists(filename);
  }
  
  if(found_all) {
    cout << "Found ";
    for(auto [filename, buf, sz] : data) {
      cout << filename << "... ";
    }
    cout << endl;
    
    for(auto [filename, buf, sz] : data) {
      ifstream fin(filename, ios::in | ios::binary);
      fin.read(buf, sz);
      fin.close();
    }
  }else{
    cout << "Generating ";
    for(auto [filename, buf, sz] : data) {
      cout << filename << "... ";
    }
    cout << endl;
    f();
 
    for(auto [filename, buf, sz] : data) {
      ofstream fout(filename, ios::out | ios::binary);
      fout.write(buf, sz);
      fout.close();
    }
  }
}

template<class F>
void try_load_table(vector<tuple<string, char**, size_t>> const& data, F&& f) {
  bool found_all = true;
  for(auto [filename, buf, sz] : data) {
    found_all = found_all && std::filesystem::exists(filename);
  }
  
  if(found_all) {
    cout << "Found ";
    for(auto [filename, buf, sz] : data) {
      cout << filename << "... ";
    }
    cout << endl;
    
    for(auto [filename, buf, sz] : data) {
      int fd = open(filename.c_str(), O_RDONLY);
      *buf = (char*)mmap(nullptr, sz,
                         PROT_READ, MAP_SHARED,
                         fd, 0);
      if(*buf == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
      }
      close(fd);
      // ifstream fin(filename, ios::in | ios::binary);
      // fin.read(*buf, sz);
      // fin.close();
    }
  }else{
    cout << "Generating ";
    for(auto [filename, buf, sz] : data) {
      *buf = new char[sz];
      cout << filename << "... ";
    }
    cout << endl;
    f();

    for(auto [filename, buf, sz] : data) {
      ofstream fout(filename, ios::out | ios::binary);
      fout.write(*buf, sz);
      fout.close();
    }
  }
}

struct BIT {
  vector<i32> A;
  BIT(i32 n) : A(n+1, 0){ }

  i32 get(i32 i){
    i += 1;
    i32 r = 0;
    for(; i > 0; i -= i&-i) {
      r += A[i];
    }
    return r;
  }

  i32 getRange(i32 l, i32 r) {
    i32 ans = get(r);
    if(l) ans = ans - get(l-1);
    return ans;
  }

  void add(i32 i, i32 v){
    i += 1;
    for(; i < (i32)A.size(); i += i&-i){
      A[i] += v;
    }
  }
};


void init_cnk();
void init_util();

