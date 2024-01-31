#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <cstring>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <cmath>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <bitset>
#include <fstream>
#include <filesystem>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <random>
#include <ext/pb_ds/assoc_container.hpp>
// #include <x86intrin.h>

// #define BACKWARD_HAS_DW 1
// #include "backward.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

// Macros

using i8  = int8_t;
using u8  = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

#define FOR(i, n) for(i64 i = 0; i < (i64)(n); ++i)
#define FORU(i, j, k) for(i64 i = (j); i <= (i64)(k); ++i)
#define FORD(i, j, k) for(i64 i = (j); i >= (i64)(k); --i)

#define ALWAYS_INLINE __attribute__((always_inline))
#define FORCE_INLINE ALWAYS_INLINE inline

#define AS_STRING_IMPL(x) #x
#define AS_STRING(x) AS_STRING_IMPL(x)
#define runtime_assert(x) do {                                          \
    if(!(x)) {                                                          \
      throw runtime_error((std::filesystem::path(__FILE__).filename().string()) + ":" AS_STRING(__LINE__) " Assertion failed: " #x); \
    }                                                                   \
  } while(0)
#define impossible() do {                                               \
    throw runtime_error((std::filesystem::path(__FILE__).filename().string()) + ":" AS_STRING(__LINE__) " impossible"); \
  } while(0)
#define all(x) begin(x), end(x)
#define mp make_pair
#define mt make_tuple
#define pb push_back
#define eb emplace_back

// Types

template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T>
using max_queue = priority_queue<T>;

struct uint64_hash {
  static inline u64 rotr(u64 x, unsigned k) {
    return (x >> k) | (x << (8U * sizeof(u64) - k));
  }

  static inline u64 hash_int(u64 x) noexcept {
    auto h1 = x * (u64)(0xA24BAED4963EE407);
    auto h2 = rotr(x, 32U) * (u64)(0x9FB21C651E98DF25);
    auto h = rotr(h1 + h2, 32U);
    return h;
  }

  size_t operator()(u64 x) const {
    static const u64 FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
    return hash_int(x + FIXED_RANDOM);
  }
};

template <typename K, typename V, typename Hash = uint64_hash>
using hash_map = __gnu_pbds::gp_hash_table<K, V, Hash>;
template <typename K, typename Hash = uint64_hash>
using hash_set = hash_map<K, __gnu_pbds::null_type, Hash>;

// Printing

template<class T>
void print_collection(ostream& out, T const& x);

namespace std {

  template<class A>
  ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
  template<class A, size_t N>
  ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, deque<A> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, multiset<A> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, multimap<A, B> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, set<A> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, map<A, B> const& x) { print_collection(out, x); return out; }

} // namespace std

template <class T, size_t... I>
void print_tuple(ostream &out, T const &a, index_sequence<I...>);

namespace std {
  template<class... A>
  ostream& operator<<(ostream& out, tuple<A...> const& x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
  }
} // namespace std
  
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>){
  using swallow = int[];
  out << '(';
  (void)swallow{0, (void(out << (I == 0? "" : ", ") << get<I>(a)), 0)...};
  out << ')';
}

template<class T>
void print_collection(ostream& out, T const& x) {
  int f = 0;
  out << '[';
  for(auto const& i: x) {
    out << (f++ ? "," : "");
    out << i;
  }
  out << "]";
}

// Random

extern thread_local struct RNG {
  u32 x, y, z, w;

  RNG(u64 seed) {
    reset(seed);
  }

  RNG() {
    std::random_device rd;
    reset(rd());
  }

  inline void reset(u64 seed) {
    struct splitmix64_state {
      u64 s;

      u64 splitmix64() {
        u64 result = (s += 0x9E3779B97f4A7C15);
        result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
        result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
        return result ^ (result >> 31);
      }
    };

    splitmix64_state s { seed };

    x = (u32)s.splitmix64();
    y = (u32)s.splitmix64();
    z = (u32)s.splitmix64();
    w = (u32)s.splitmix64();
  }

  inline u32 xorshift128() {
    u32 t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
  }

  // UniformRandomBitGenerator interface
  using result_type = u32;
  constexpr u32 min(){ return numeric_limits<u32>::min(); }
  constexpr u32 max(){ return numeric_limits<u32>::max(); }
  u32 operator()() { return randomInt32(); }

  inline u32 randomInt32() {
    return xorshift128();
  }

  inline u64 randomInt64() {
    return (((u64)randomInt32())<<32ll) | ((u64)randomInt32());
  }

  inline u32 random32(u32 r) {
    return (((u64)randomInt32())*r)>>32;
  }

  inline u64 random64(u64 r) {
    return randomInt64()%r;
  }

  inline u32 randomRange32(u32 l, u32 r) {
    return l + random32(r-l+1);
  }

  inline u64 randomRange64(u64 l, u64 r) {
    return l + random64(r-l+1);
  }

  inline double randomDouble() {
    return (double)randomInt32() / 4294967296.0;
  }

  inline float randomFloat() {
    return (float)randomInt32() / 4294967296.0;
  }

  template<class T>
  void shuffle(vector<T>& v) {
    i32 sz = v.size();
    for(i32 i = sz; i > 1; i--) {
      i32 p = random32(i);
      swap(v[i-1],v[p]);
    }
  }

  template<class T>
  void shuffle(T* fr, T* to) {
    i32 sz = distance(fr,to);
    for(i32 i = sz; i > 1; i--) {
      i32 p = random32(i);
      swap(fr[i-1],fr[p]);
    }
  }

  template<class T>
  inline i32 sample_index(vector<T> const& v) {
    return random32(v.size());
  }

  template<class T>
  inline T sample(vector<T> const& v) {
    return v[sample_index(v)];
  }
} rng;

// Letrec

template<class Fun>
class letrec_result {
  Fun fun_;
  public:
    template<class T>
    explicit letrec_result(T &&fun): fun_(forward<T>(fun)) {}

    template<class ...Args>
    decltype(auto) operator()(Args &&...args) {
      return fun_(ref(*this), forward<Args>(args)...);
    }
};

template<class Fun>
decltype(auto) letrec(Fun &&fun) {
  return letrec_result<decay_t<Fun>>(forward<Fun>(fun));
}

// Timer

struct timer {
  chrono::high_resolution_clock::time_point t_begin;

  timer() {
    t_begin = chrono::high_resolution_clock::now();
  }

  void reset() {
    t_begin = chrono::high_resolution_clock::now();
  }

  float elapsed() const {
    return chrono::duration<float>(chrono::high_resolution_clock::now() - t_begin).count();
  }
};

// Util

template<class T>
T& smin(T& x, T const& y) { x = min(x,y); return x; }

template<class T>
T& smax(T& x, T const& y) { x = max(x,y); return x; }

template<typename T>
i32 sgn(T val) {
  if(val < 0) return -1;
  if(val > 0) return 1;
  return 0;
}

static inline
string int_to_string(i64 val, i32 digits = 0) {
  string s = to_string(val);
  reverse(all(s));
  while((i32)s.size() < digits) s.pb('0');
  reverse(all(s));
  return s;
}

struct separate_thousands : std::numpunct<char> {
  char_type do_thousands_sep() const override { return ','; }  // separate with commas
  string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

// Debug

static inline void debug_impl_seq() {
  cerr << "}";
}

template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}

#define debug(x...) do {                                                \
    cerr << (std::filesystem::path(__FILE__).filename().string()) + ":" AS_STRING(__LINE__) "  {" << #x << "} = {"; \
    debug_impl_seq(x);                                                  \
    cerr << endl << flush;                                              \
  } while(0)
