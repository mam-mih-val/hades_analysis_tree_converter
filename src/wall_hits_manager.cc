//
// Created by mikhail on 5/27/20.
//

#include "wall_hits_manager.h"

namespace Analysis {
WallHitsManager* WallHitsManager::instance_= nullptr;

void WallHitsManager::MakeBranch(AnalysisTree::Configuration &config, TTree* tree){
AnalysisTree::BranchConfig modules_branch("forward_wall_hits", AnalysisTree::DetType::kHit);
modules_branch.AddField<int>("ring");
modules_branch.AddField<int>("module_id");
modules_branch.AddField<int>("rnd_sub");
modules_branch.AddField<float>("beta");
modules_branch.AddField<float>("time");
modules_branch.AddField<float>("distance");
modules_branch.AddField<float>("parametrized_charge");
modules_branch.AddField<int>("discrete_charge");
modules_branch.AddField<bool>("has_passed_cuts");

fields_int_.insert( std::make_pair( RING, modules_branch.GetFieldId("ring")) );
fields_int_.insert( std::make_pair( MODULE_ID, modules_branch.GetFieldId("module_id")) );
fields_int_.insert( std::make_pair( RND_SUB, modules_branch.GetFieldId("rnd_sub")) );
fields_float_.insert( std::make_pair( BETA, modules_branch.GetFieldId("beta")) );
fields_float_.insert( std::make_pair( TIME, modules_branch.GetFieldId("time")) );
fields_float_.insert( std::make_pair( DISTANCE, modules_branch.GetFieldId("distance")) );
fields_float_.insert( std::make_pair( P_CHARGE, modules_branch.GetFieldId("parametrized_charge")) );
fields_int_.insert( std::make_pair( D_CHARGE, modules_branch.GetFieldId("discrete_charge")) );
fields_bool_.insert( std::make_pair( PASSED_CUTS, modules_branch.GetFieldId("has_passed_cuts")) );

config.AddBranchConfig(modules_branch);
wall_hits_ = new AnalysisTree::HitDetector(config.GetLastId());
tree->Branch("forward_wall_hits", "AnalysisTree::HitDetector", &wall_hits_);
}
}