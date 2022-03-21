//
// Created by mikhail on 5/26/20.
//

#include "meta_hits_manager.h"
namespace Analysis{
MetaHitsManager *MetaHitsManager::instance_= nullptr;

void MetaHitsManager::MakeBranch( AnalysisTree::Configuration &config, TTree* tree ){
AnalysisTree::BranchConfig hit_branch("meta_hits", AnalysisTree::DetType::kHit);
hit_branch.AddField<float>("mass2");
hit_branch.AddField<float>("path_length");
hit_branch.AddField<float>("beta");
hit_branch.AddField<float>("time_of_flight");
hit_branch.AddField<float>("match_quality");
hit_branch.AddField<float>("match_radius");
hit_branch.AddField<float>("dEdx");

hit_branch.AddField<int>("charge");

hit_branch.AddField<bool>("is_tof_hit");
hit_branch.AddField<bool>("is_rpc_hit");

fields_int_.insert( std::make_pair( CHARGE, hit_branch.GetFieldId("charge") ) );

fields_float_.insert( std::make_pair( MASS2, hit_branch.GetFieldId("mass2") ) );
fields_float_.insert( std::make_pair( PATH_LENGTH, hit_branch.GetFieldId("path_length") ) );
fields_float_.insert( std::make_pair( BETA, hit_branch.GetFieldId("beta") ) );
fields_float_.insert( std::make_pair( TIME_OF_FLIGHT, hit_branch.GetFieldId("time_of_flight") ) );
fields_float_.insert( std::make_pair( MATCH_QUALITY, hit_branch.GetFieldId("match_quality") ) );
fields_float_.insert( std::make_pair( MATCH_RADIUS, hit_branch.GetFieldId("match_radius") ) );
fields_float_.insert( std::make_pair( DE_DX, hit_branch.GetFieldId("dEdx") ) );

fields_bool_.insert( std::make_pair( IS_TOF_HIT, hit_branch.GetFieldId("is_tof_hit") ) );
fields_bool_.insert( std::make_pair( IS_RPC_HIT, hit_branch.GetFieldId("is_rpc_hit") ) );

config.AddBranchConfig(hit_branch);
hit_detector_ = new AnalysisTree::HitDetector( config.GetNumberOfBranches() );
tree->Branch("meta_hits", "AnalysisTree::HitDetector", &hit_detector_);
}
}