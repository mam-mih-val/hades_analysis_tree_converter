//
// Created by mikhail on 5/27/20.
//

#ifndef HTREE_TO_AT_SRC_WALL_HITS_MANAGER_H_
#define HTREE_TO_AT_SRC_WALL_HITS_MANAGER_H_

#include "detector_manager.h"

namespace Analysis {
class WallHitsManager : public DetectorManager {
public:
  enum FIELDS_FLOAT{
    BETA=0,
    TIME,
    DISTANCE,
    P_CHARGE, //Parametrized charge
  };
  enum FIELDS_INT{
    RING=0,
    MODULE_ID,
    D_CHARGE, // Discrete charge
  };
  enum FIELDS_BOOL{
    PASSED_CUTS=0
  };
  static WallHitsManager *Instance() {
    if (!instance_)
      instance_ = new WallHitsManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override {
    AnalysisTree::BranchConfig modules_branch("forward_wall_modules", AnalysisTree::DetType::kHit);
    modules_branch.AddField<int>("ring");
    modules_branch.AddField<int>("module_id");
    modules_branch.AddField<float>("beta");
    modules_branch.AddField<float>("time");
    modules_branch.AddField<float>("distance");
    modules_branch.AddField<float>("parametrized_charge");
    modules_branch.AddField<int>("discrete_charge");
    modules_branch.AddField<bool>("has_passed_cuts");

    fields_int_.insert( std::make_pair( RING, modules_branch.GetFieldId("ring")) );
    fields_int_.insert( std::make_pair( MODULE_ID, modules_branch.GetFieldId("module_id")) );
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
  void NewModule(AnalysisTree::Configuration &config){
    hit_ = wall_hits_->AddChannel();
    hit_->Init(config.GetBranchConfig(wall_hits_->GetId() ) );
  }
  void ClearDetector(){ wall_hits_->ClearChannels(); }
  void SetPosition( float x, float y, float z ){
    hit_->SetPosition(x, y, z);
  }
  void SetSignal(float signal){
    hit_->SetSignal(signal);
  }
  void SetField(const int& value, int idx) override{
    hit_->SetField( value, fields_int_.at(idx) );
  }
  void SetField(const float& value, int idx) override{
    hit_->SetField( value, fields_float_.at(idx) );
  }
  void SetField(const bool& value, int idx) override{
    hit_->SetField( value, fields_bool_.at(idx) );
  }

private:
  static WallHitsManager *instance_;
  WallHitsManager() : wall_hits_{nullptr}, hit_{nullptr} {};
  ~WallHitsManager() = default;

  AnalysisTree::HitDetector *wall_hits_;
  AnalysisTree::Hit *hit_;
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_WALL_HITS_MANAGER_H_
