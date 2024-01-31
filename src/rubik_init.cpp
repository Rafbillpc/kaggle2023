#include "rubik_init.hpp"
#include "rubik_222.hpp"
#include "optimal_333.hpp"
#include "kociemba_phase1.hpp"
#include "kociemba_phase2.hpp"
#include "rubik_444_phase1.hpp"
#include "rubik_444_phase2.hpp"
#include "rubik_444_phase3.hpp"
#include "rubik_555_phase1.hpp"
#include "rubik_555_phase2.hpp"
#include "rubik_555_phase3.hpp"
#include "rubik_555_phase4.hpp"
#include "rubik_555_phase5.hpp"

namespace rubik {
  void init(){
    init_transitions();
    corners_sym_table.init("corners");
    corners_pruning_table.init("corners");
    
    flipudslice_sym_table.init("flipudslice");
    corners_twist_move_table.init("corners_twist");
    kociemba_phase1_pruning_table.init("kociemba_phase1");
    corners_perm_sym_table.init("corners_perm");
    edges_perm_move_table.init("edges_perm");
    udslice_perm_move_table.init("udslice_perm");
    udslice_perm_pruning_table.init("udslice_perm");
    kociemba_phase2_pruning_table.init("kociemba_phase2");

    // optimal_333_back_table.init("optimal_333");
    
    centers3_phase1_444_sym_table.init("centers3_phase1_444");
    centers3_phase1_444_pruning_table.init("centers3_phase1_444");
    
    centers3_phase2_444_sym_table.init("centers3_phase2_444");
    centers3_phase2_444_pruning_table.init("centers3_phase2_444");

    centers3_phase3_444_move_table.init("centers3_phase3_444");
    centers3_phase3_444_pruning_table.init("centers3_phase3_444");
    edges2_phase3_444_pruning_table.init("edges2_phase3_444");
    phase3_444_back_table.init("phase3_444");
    
    centers2_phase1_555_sym_table.init("centers2_phase1_555");
    centers3_phase1_555_move_table.init("centers3_phase1_555");
    centers2_phase1_555_pruning_table.init("centers2_phase1_555");
    centers3_phase1_555_pruning_table.init("centers3_phase1_555");
    centers_phase1_555_back_table.init("centers_phase1_555");
    
    centers2_phase2_555_sym_table.init("centers2_phase2_555");
    centers3_phase2_555_move_table.init("centers3_phase2_555");
    centers_phase2_555_pruning_table.init("centers_phase2_555");

    centers_phase3_555_move_table.init("centers_phase3_555");
    edges2_phase3_555_sym_table.init("edges2_phase3_555");
    edges1_phase3_555_move_table.init("edges1_phase3_555");
    centers_phase3_555_pruning_table.init("centers_phase3_555");
    edges2_phase3_555_pruning_table.init("edges2_phase3_555");
    phase3_555_back_table.init("phase3_555");

    centers2_phase4_555_move_table.init("centers2_phase4_555");
    centers3_phase4_555_move_table.init("centers3_phase4_555");
    edges_low_phase4_555_move_table.init("edges_low_phase4_555");
    edges_high_phase4_555_move_table.init("edges_high_phase4_555");
    centers_phase4_555_pruning_table.init("centers_phase4_555");
    edges_low_phase4_555_pruning_table.init("edges_low_phase4_555");
    edges_high_phase4_555_pruning_table.init("edges_high_phase4_555");
    edges_phase4_555_back_table.init("edges_phase4_555");
    phase4_555_back_table.init("phase4_555");

    centers2_phase5_555_move_table.init("centers2_phase5_555");
    centers3_phase5_555_move_table.init("centers3_phase5_555");
    edges_low_phase5_555_move_table.init("edges_low_phase5_555");
    edges_high_phase5_555_move_table.init("edges_high_phase5_555");
    centers2_phase5_555_pruning_table.init("centers2_phase5_555");
    centers3_phase5_555_pruning_table.init("centers3_phase5_555");
    edges_low_phase5_555_pruning_table.init("edges_low_phase5_555");
    edges_high_phase5_555_pruning_table.init("edges_high_phase5_555");
    phase5_555_back_table.init("phase5_555");
    centers_phase5_555_back_table.init("centers_phase5_555");
    edges_phase5_555_back_table.init("edges_phase5_555");
    // centers2_edges_phase5_555_back_table.init("centers2_edges_phase5_555");
    // centers3_edges_phase5_555_back_table.init("centers3_edges_phase5_555");
  }
}
