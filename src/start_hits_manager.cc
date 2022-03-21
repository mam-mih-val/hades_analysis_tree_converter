//
// Created by mikhail on 11/17/20.
//

#include "start_hits_manager.h"

namespace Analysis{
StartHitsManager *StartHitsManager::instance_= nullptr;

void StartHitsManager::MakeBranch( AnalysisTree::Configuration &config, TTree* tree ){
  AnalysisTree::BranchConfig hit_branch("start_hits", AnalysisTree::DetType::kHit);
  hit_branch.AddField<int>("multiplicity");
  hit_branch.AddField<int>("module");
  hit_branch.AddField<int>("strip");
  for (int i=0; i<10; ++i){
    std::string field_name = "time_"+std::to_string(i);
    hit_branch.AddField<float>(field_name);
  }
  for (int i=0; i<10; ++i){
    std::string field_name = "width_"+std::to_string(i);
    hit_branch.AddField<float>(field_name);
  }

  fields_int_.insert( std::make_pair( MULTIPLICITY, hit_branch.GetFieldId("multiplicity") ) );
  fields_int_.insert( std::make_pair( MODULE, hit_branch.GetFieldId("module") ) );
  fields_int_.insert( std::make_pair( STRIP, hit_branch.GetFieldId("strip") ) );

  config.AddBranchConfig(hit_branch);
  hit_detector_ = new AnalysisTree::HitDetector( config.GetNumberOfBranches() );
  tree->Branch("start_hits", "AnalysisTree::HitDetector", &hit_detector_);
}
}