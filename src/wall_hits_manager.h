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
    RND_SUB, // 1 or 2
  };
  enum FIELDS_BOOL{
    PASSED_CUTS=0
  };
  static WallHitsManager *Instance() {
    if (!instance_)
      instance_ = new WallHitsManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override;

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
