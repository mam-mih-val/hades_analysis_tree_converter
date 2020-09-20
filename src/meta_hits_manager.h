//
// Created by mikhail on 5/26/20.
//

#ifndef HTREE_TO_AT_SRC_META_HITS_MANAGER_H_
#define HTREE_TO_AT_SRC_META_HITS_MANAGER_H_

#include "detector_manager.h"

namespace Analysis {
class MetaHitsManager : public DetectorManager {
public:
  enum FIELDS_INT{
    CHARGE=0
  };
  enum FIELDS_FLOAT{
    MASS2=0,
    PATH_LENGTH,
    BETA,
    TIME_OF_FLIGHT,
    MATCH_QUALITY,
    MATCH_RADIUS,
    DE_DX
  };
  enum FIELDS_BOOL{
    IS_TOF_HIT,
    IS_RPC_HIT
  };
  static MetaHitsManager * Instance(){
    if (!instance_)
      instance_ = new MetaHitsManager;
    return instance_;
  }
  void MakeBranch( AnalysisTree::Configuration &config, TTree* tree ) override;
  void NewHit(AnalysisTree::Configuration& config){
    hit_ = hit_detector_->AddChannel();
    hit_->Init(config.GetBranchConfig(hit_detector_->GetId() ) );
  }
  int GetHitId() { return hit_->GetId(); }
  void ClearDetector(){ hit_detector_->ClearChannels(); }
  void ReserveHits(int n_hits){ hit_detector_->Reserve(n_hits); }
  void SetField( const int& value, int idx ) override {
    hit_->SetField( value, fields_int_.at(idx) );
  }
  void SetField( const float& value, int idx ) override {
    hit_->SetField( value, fields_float_.at(idx) );
  }
  void SetField( const bool& value, int idx ) override {
    hit_->SetField( value, fields_bool_.at(idx) );
  }

private:
  static MetaHitsManager * instance_;
  MetaHitsManager() : hit_detector_{nullptr}, hit_{nullptr} {};
  ~MetaHitsManager() = default;
  AnalysisTree::HitDetector* hit_detector_;
  AnalysisTree::Hit* hit_;

};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_META_HITS_MANAGER_H_
