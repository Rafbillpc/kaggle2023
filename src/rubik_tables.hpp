#pragma once

#include "header.hpp"
namespace rubik {
  
  // const u64 CORNERS_CONFIGS = 88'179'840;
  // extern u8 corners_distance[CORNERS_CONFIGS];

  // const u64 PHASE1_CENTERS1_CONFIGS = 720;
  // extern u8 phase1_centers1_distance[PHASE1_CENTERS1_CONFIGS];

  // const u64 C_24_8 = 735471;
  
  // const u64 PHASE1_CENTERS2_CONFIGS = C_24_8;
  // extern u8 phase1_centers2_distance[PHASE1_CENTERS2_CONFIGS];

  // const u64 PHASE1_CENTERS3_CONFIGS = C_24_8 * 16;
  // extern u8 phase1_centers3_distance[PHASE1_CENTERS3_CONFIGS];

  // const u64  PHASE1_CENTERS4_CONFIGS = C_24_8 * 256;
  // const u64  PHASE1_CENTERS4_CLASSES = 23545977;
  // extern u32 phase1_centers4_sym_class[PHASE1_CENTERS4_CONFIGS];
  // extern u32 phase1_centers4_sym_which[PHASE1_CENTERS4_CONFIGS];
  // extern u8  phase1_centers4_distance[23545977];

  // const i32 PHASE1_CENTERS4_BACK10_DISTANCE = 10;
  // const u64 PHASE1_CENTERS4_BACK10_HASHSIZE = 500'000'000; // 262131993
  // extern u32 *phase1_centers4_back10_hashset;
  // extern u8 *phase1_centers4_back10_distance;

  // static inline
  // u64 phase1_centers4_back10_hashset_insert(u64 k) {
  //   u64 h = uint64_hash::hash_int(1+0xDE*k);
  //   u64 x = h % PHASE1_CENTERS4_BACK10_HASHSIZE;
  //   u32 y = h / PHASE1_CENTERS4_BACK10_HASHSIZE;
  //   if(y == 0) {
  //     debug(k, h, x, y);
  //     runtime_assert(y != 0);
  //   }
  //   while(phase1_centers4_back10_hashset[x] != 0) {
  //     runtime_assert(phase1_centers4_back10_hashset[x] != y);
  //     x = x+1;
  //     if(x == PHASE1_CENTERS4_BACK10_HASHSIZE) x = 0;
  //   }
  //   phase1_centers4_back10_hashset[x] = y;
  //   return x;
  // }
 
  // static inline
  // u8 phase1_centers4_back10_hashset_find(u64 k) {
  //   u64 h = uint64_hash::hash_int(1+0xDE*k);
  //   u64 x = h % PHASE1_CENTERS4_BACK10_HASHSIZE;
  //   u32 y = h / PHASE1_CENTERS4_BACK10_HASHSIZE;
  //   // runtime_assert(y != 0);
  //   while(phase1_centers4_back10_hashset[x] != 0) {
  //     if(phase1_centers4_back10_hashset[x] == y) return phase1_centers4_back10_distance[x];
  //     x = x+1;
  //     if(x == PHASE1_CENTERS4_BACK10_HASHSIZE) x = 0;
  //   }
  //   return 255;
  // }
  
  // const u64 PHASE2_CENTERS2_CONFIGS = 12870;
  // extern u8 phase2_centers2_distance[PHASE2_CENTERS2_CONFIGS];

  // const u64 PHASE2_CENTERS3_CONFIGS = 12870 * 4;
  // extern u8 phase2_centers3_distance[PHASE2_CENTERS3_CONFIGS];

  // const u64 PHASE3_CENTERS2_CONFIGS = 40320;
  // extern u8 phase3_centers2_distance[PHASE3_CENTERS2_CONFIGS];

  // const u64 PHASE3_CENTERS3_CONFIGS = 40320 * 16;
  // extern u8 phase3_centers3_distance[PHASE3_CENTERS3_CONFIGS];

  // const u64 PHASE4_CENTERS2_CONFIGS = 40320 * 16;
  // extern u8 phase4_centers2_distance[PHASE4_CENTERS2_CONFIGS];

  // const u64 PHASE4_CENTERS3_CONFIGS = 40320 * 16;
  // extern u8 phase4_centers3_distance[PHASE4_CENTERS3_CONFIGS];

  // const u64 PHASE5_CENTERS2_CONFIGS = 40320 * 16;
  // extern u8 phase5_centers2_distance[PHASE5_CENTERS2_CONFIGS];

  // const u64 PHASE5_CENTERS3_CONFIGS = 40320 * 16;
  // extern u8 phase5_centers3_distance[PHASE5_CENTERS3_CONFIGS];

}
